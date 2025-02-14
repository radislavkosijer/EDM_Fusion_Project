/*****************************************************************************
 * emd.h
 *
 * Created on: January 20, 2025.
 * Author: Radislav Kosijer
 *
 * @brief Header file for Empirical Mode Decomposition (EMD) and image processing.
 *****************************************************************************/

#ifndef EMD_H
#define EMD_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "led.h"

/** @brief Maximum signal length (width * height). */
#define MAX_SIGNAL_LEN (200 * 200)

/** @brief Maximum number of extrema per signal. */
#define MAX_EXTREMA 1024


/*==============================================================================
 * Function Declarations
 *============================================================================*/

/**
 * @brief Perform Empirical Mode Decomposition (EMD) on a signal.
 *
 * This function decomposes the input signal into its intrinsic mode functions (IMFs).
 *
 * @param signal Pointer to the signal data in Q16.16 fixed-point format.
 * @param length Length of the signal.
 */
void emd_decompose(int32_t* signal, int length);


/**
 * @brief Convert an 8-bit image to Q16.16 fixed-point format.
 *
 * @param input  Pointer to the input 8-bit image.
 * @param output Pointer to the output image in Q16.16 format.
 * @param size   Number of pixels in the image.
 */
void convert_to_q16_16(const unsigned char* input, int32_t* output, int size);

/**
 * @brief Convert a Q16.16 fixed-point image to 8-bit format.
 *
 * This function converts the fixed-point representation back to an 8-bit image,
 * applying appropriate rounding and clamping.
 *
 * @param input  Pointer to the input image in Q16.16 format.
 * @param output Pointer to the output 8-bit image.
 * @param size   Number of pixels in the image.
 */
void convert_from_q16_16(const int32_t* input, unsigned char* output, int size);

/**
 * @brief Perform linear interpolation using SIMD for envelope calculation.
 *
 * This internal function interpolates between extrema points in a signal using linear interpolation.
 *
 * @param extrema_pos  Array of positions of extrema.
 * @param extrema_val  Array of values at the extrema.
 * @param num_extrema  Number of extrema.
 * @param envelope     Output envelope array.
 * @param signal_length Length of the signal.
 */
static void linear_interp_simd(const int32_t* extrema_pos, const int32_t* extrema_val,
                               int num_extrema, int32_t* envelope, int signal_length);


#endif /* EMD_H */
