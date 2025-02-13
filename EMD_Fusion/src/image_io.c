#include "image_io.h"

/**
 * @brief Save a fused image to a binary file.
 *
 * This function writes the fused image to a file in binary format.
 * It writes the image width and height as 32-bit unsigned integers,
 * followed by the pixel data. The pixel data is processed in groups of
 * 4 bytes, where each group is packed into a single 32-bit unsigned integer.
 *
 * @param filename  Name of the file to write.
 * @param width     Image width.
 * @param height    Image height.
 * @param fused_img Pointer to the fused image data (8-bit values).
 *
 * Created on: February 2, 2025.
 * Author: Radislav Kosijer
 */
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
}
