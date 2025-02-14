/*****************************************************************************
 * main.c
 *****************************************************************************/

/**
 * @file EMD_Fusion_Project.c
 * @brief Main project file for image fusion using Empirical Mode Decomposition (EMD).
 *
 * This file demonstrates an image fusion algorithm by performing the following steps:
 * 1. Retrieving image dimensions and input data.
 * 2. Converting 8-bit image data to Q16.16 fixed-point format.
 * 3. Applying EMD decomposition on each signal.
 * 4. Calculating local variance using a 3x3 window.
 * 5. Generating a decision mask based on the variance.
 * 6. Fusing the images using the decision mask.
 * 7. Performing linear histogram stretching to pixel value
 * 8. Saving the fused image to a binary file.
 *
 * Created on: January 20, 2025.
 * Author: Radislav Kosijer
 */

#include "main.h"
#include <sys/platform.h>
#include "adi_initialize.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "emd.h"            // Declarations for EMD functions and conversions.
#include "decision_mask.h" // Declaration for local variance and decision mask
#include "fusion.h"       // Declaration for image processing functions
#include "led.h"         // Declaration for LED control functions
#include "p27a.h"
#include "p27b.h"

// SDRAM buffers for intermediate processing
#pragma section("seg_sdram1")
static char alpha_mask_buffer[MAX_SIGNAL_LEN];

#pragma section("seg_sdram1")
static int32_t buffer_signal1[MAX_SIGNAL_LEN];

#pragma section("seg_sdram1")
static int32_t buffer_signal2[MAX_SIGNAL_LEN];

#pragma section("seg_sdram1")
static int32_t var_map1_buffer[MAX_SIGNAL_LEN];

#pragma section("seg_sdram1")
static int32_t var_map2_buffer[MAX_SIGNAL_LEN];

#pragma section("seg_sdram1")
static unsigned char buffer_fused_image[MAX_SIGNAL_LEN];

/**
 * @brief Main entry point for the image fusion project.
 *
 * This function performs the following steps:
 *   - Assumes both input images have the same dimensions.
 *   - Converts 8-bit image data to Q16.16 fixed-point format.
 *   - Applies EMD decomposition to each signal.
 *   - Calculates local variance for each signal using a 3x3 window.
 *   - Generates a decision mask based on the variance maps.
 *   - Fuses the two images using the decision mask.
 *   - Saves the fused image to a binary file.
 *
 * @return int Exit status.
 */


int main(void) {

    led_init();         // Initialize LE diodes.
    led_all_off();      // Turn off all LED at start.

    // Assume both images have the same dimensions.
    unsigned int width = p27a_width;
    unsigned int height = p27a_height;
    int num_pixels = width * height;

    // Pointers to the input image data from header files.
    const unsigned char* vector1 = p27a;
    const unsigned char* vector2 = p27b;

    // Use pre-allocated buffers for Q16.16 representation of signals.
    int32_t* signal1 = buffer_signal1;
    int32_t* signal2 = buffer_signal2;

    // Convert 8-bit image data to Q16.16 fixed-point format.
    convert_to_q16_16(vector1, signal1, num_pixels);
    convert_to_q16_16(vector2, signal2, num_pixels);

    // Apply EMD decomposition to each signal.
    emd_decompose(signal1, num_pixels);
    emd_decompose(signal2, num_pixels);

    // Use pre-allocated buffers for local variance maps.
    int32_t* var_map1 = var_map1_buffer;
    int32_t* var_map2 = var_map2_buffer;

    // Calculate local variance (using a 3x3 window) for both signals.
    calculate_local_variance(signal1, width, height, var_map1);
    calculate_local_variance(signal2, width, height, var_map2);

    // Generate a decision mask based on the local variance of both images.
    char* alpha_mask = alpha_mask_buffer;
    generate_decision_mask(var_map1, var_map2, width, height, alpha_mask);

    // Fuse the images using the decision mask.
    unsigned char* fused_img = buffer_fused_image;
    fuse_images(vector1, vector2, alpha_mask, width, height, fused_img);

    // Perform linear histogram stretching
    histogram_stretch(fused_img, width, height);

    // Save the fused image to a binary file.
    save_fused_image("fused_image.bin", width, height, fused_img);

    printf("Image fusion successfully completed!\n");

    return 0;
}
