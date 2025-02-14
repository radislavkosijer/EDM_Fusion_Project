/*
 * fusion.c
 *
 *  Created on: February 13, 2025.
 *      Author: Radislav Kosijer
 */

#include "fusion.h"
#include "led.h"

/** Named constants for alpha mask decisions. */
#define ALPHA_A    0
#define ALPHA_B    1
#define ALPHA_AVG  2

void fuse_images(const unsigned char* imgA, const unsigned char* imgB,
                 const char* alpha_mask, int width, int height, unsigned char* fused_img) {
	#pragma vector_for
    for (int i = 0; i < width * height; i++) {
        switch (alpha_mask[i]) {
            case ALPHA_A:
                fused_img[i] = imgA[i];
                break;
            case ALPHA_B:
                fused_img[i] = imgB[i];
                break;
            case ALPHA_AVG:
                fused_img[i] = (imgA[i] + imgB[i] + 1) >> 1;
                break;
        }
    }
}

void histogram_stretch(unsigned char* img, int width, int height){
    int num_pixels = width * height;
    unsigned char minVal = 255;
    unsigned char maxVal = 0;

    /* Find the minimum and maximum pixel values. */
    for (int i = 0; i < num_pixels; i++) {
        unsigned char val = img[i];
        if (val < minVal) {
            minVal = val;
        }
        if (val > maxVal) {
            maxVal = val;
        }
    }

    /* If all pixels are identical, no stretching is needed. */
    int range = maxVal - minVal;
    if (range == 0) {
        return;
    }

    /* Linearly stretch the pixel range to [0..255]. */
    #pragma vector_for
    for (int i = 0; i < num_pixels; i++) {
        int val = (img[i] - minVal) * 255 / range;
        if (val < 0)   val = 0;
        if (val > 255) val = 255;
        img[i] = (unsigned char)val;
    }
}

void save_fused_image(const char *filename, unsigned int width, unsigned int height, const unsigned char *fused_img) {
	FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Error: Cannot open file %s for writing.\n", filename);
        return;
    }

    // Write image width and height (4 bytes each)
    if (fwrite(&width, sizeof(width), 1, fp) != 1) {
        printf("Error: Failed to write image width.\n");
        fclose(fp);
        return;
    }
    if (fwrite(&height, sizeof(height), 1, fp) != 1) {
        printf("Error: Failed to write image height.\n");
        fclose(fp);
        return;
    }

    size_t num_pixels = width * height;
    size_t groups = num_pixels / 4;
    size_t leftovers = num_pixels % 4;
    const unsigned char *p = fused_img;
    size_t segment_size = groups / 8; // Velicina jednog segmenta
    // Process pixel data in groups of 4 bytes
    for (size_t i = 0; i < groups; i++) {
        uint32_t pixelGroup = ((uint32_t)p[0])        |
                              (((uint32_t)p[1]) << 8)   |
                              (((uint32_t)p[2]) << 16)  |
                              (((uint32_t)p[3]) << 24);
        if (fwrite(&pixelGroup, sizeof(pixelGroup), 1, fp) != 1) {
            printf("Error: Failed to write grouped pixel data.\n");
            fclose(fp);
            return;
        }
        // Turning on LE diodes one by one as saving progresses
        if (segment_size > 0 && (i % segment_size == 0) && (i / segment_size < 7)) {
            switch(i / segment_size) {
                case 0:
                    led_on(7);  // LED8
                    break;
                case 1:
                    led_on(6);  // LED7
                    break;
                case 2:
                    led_on(5);  // LED6
                    break;
                case 3:
                    led_on(4);  // LED5
                    break;
                case 4:
                    led_on(3);  // LED4
                    break;
                case 5:
                    led_on(2);  // LED3
                    break;
                case 6:
                    led_on(1);  // LED2
                    break;
                case 7:
                    led_on(0);  // LED1
                    break;
                default:
                    break;
            }
        }
        p += 4;
    }

    // Process any remaining pixels individually
    for (size_t i = 0; i < leftovers; i++) {
        if (fputc(p[i], fp) == EOF) {
            printf("Error: Failed to write remaining pixel data.\n");
            fclose(fp);
            return;
        }
    }

    fclose(fp);
    led_on(0);
}
