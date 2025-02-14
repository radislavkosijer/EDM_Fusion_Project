/*
 * decision_mask.c
 *
 *  Created on: February 13, 2025.
 *      Author: Radislav Kosijer
 */

#include "decision_mask.h"
#include "led.h"

/** Named constants for alpha mask decisions. */
#define ALPHA_A    0
#define ALPHA_B    1
#define ALPHA_AVG  2

void calculate_local_variance(const int32_t* imf, int width, int height, int32_t* variance_map) {
	const int half_window = WINDOW_SIZE / 2;
    #pragma vector_for
    for (int y = 0; y < height; y++) {
        // Precompute vertical window boundaries.
        int y_start = (y - half_window < 0) ? 0 : (y - half_window);
        int y_end   = (y + half_window >= height) ? (height - 1) : (y + half_window);

        for (int x = 0; x < width; x++) {
            // Precompute horizontal window boundaries.
            int x_start = (x - half_window < 0) ? 0 : (x - half_window);
            int x_end   = (x + half_window >= width) ? (width - 1) : (x + half_window);

            int64_t sum = 0;
            int64_t sum_sq = 0;
            int count = 0;
			#pragma SIMD_for
            for (int j = y_start; j <= y_end; j++) {
                for (int k = x_start; k <= x_end; k++) {
                    int32_t val = imf[j * width + k];
                    sum += val;
                    sum_sq += ((int64_t)val * val) >> 16; // Adjust for Q16.16 format
                    count++;
                }
            }

            int32_t mean = (int32_t)(sum / count);
            int32_t var = (int32_t)((sum_sq / count) - (((int64_t)mean * mean) >> 16));
            variance_map[y * width + x] = var;
        }
    }
}

void generate_decision_mask(const int32_t* var_map1, const int32_t* var_map2,
                            int width, int height, char* alpha_mask) {
    int64_t sum_var = 0;
    int total_pixels = width * height;
    for (int i = 0; i < total_pixels; i++) {
        sum_var += var_map1[i] + var_map2[i];
    }
    int32_t avg_var = (int32_t)(sum_var / (2 * total_pixels));
    // Set threshold at 20% of average variance.
    int32_t adaptive_epsilon = (avg_var * 20) / 100;

    #pragma vector_for
    for (int i = 0; i < total_pixels; i++) {
        // Convert the Q16.16 difference to an integer.
        const int32_t diff = (var_map1[i] - var_map2[i] + 0x8000) >> 16;
        // Choose ALPHA_A if image A has higher variance, ALPHA_B if lower, otherwise ALPHA_AVG.
        alpha_mask[i] = (diff > adaptive_epsilon)  ? ALPHA_A :
                        (diff < -adaptive_epsilon) ? ALPHA_B : ALPHA_AVG;
    }
}
