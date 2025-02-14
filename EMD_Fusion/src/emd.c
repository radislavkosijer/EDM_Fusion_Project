/*
 * emd.c
 *
 * Created on: January 20, 2025.
 * Author: Radislav Kosijer
 *
 */

#include "emd.h"

// SDRAM buffers
#pragma section("seg_sdram1")
static int32_t upper_env_buffer[MAX_EXTREMA*8];

#pragma section("seg_sdram1")
static int32_t lower_env_buffer[MAX_EXTREMA*8];

#pragma section("seg_sdram1")
static int32_t mean_env[MAX_EXTREMA*8];

#pragma section("seg_sdram1")
static int32_t max_pos[MAX_EXTREMA];

#pragma section("seg_sdram1")
static int32_t max_val[MAX_EXTREMA];

#pragma section("seg_sdram1")
static int32_t min_pos[MAX_EXTREMA];

#pragma section("seg_sdram1")
static int32_t min_val[MAX_EXTREMA];

#pragma section("seg_sdram1")
static int32_t imf[MAX_SIGNAL_LEN];

static void linear_interp_simd(const int32_t* extrema_pos, const int32_t* extrema_val,
                               int num_extrema, int32_t* envelope, int signal_length)
{
    int i;
    int first_pos = extrema_pos[0];
    // Fill the portion of the signal to the left of the first extremum with its constant value.
	#pragma SIMD_for
    for (i = 0; i < first_pos && i < signal_length; i++) {
        envelope[i] = extrema_val[0];
    }

    // Process each segment between successive extrema.
    for (int seg = 0; seg < num_extrema - 1; seg++) {
        int pos1 = extrema_pos[seg];
        int pos2 = extrema_pos[seg + 1];
        int32_t val1 = extrema_val[seg];
        int32_t val2 = extrema_val[seg + 1];
        int seg_length = pos2 - pos1;

        // Skip invalid segments.
        if (seg_length <= 0)
            continue;

        // Calculate the delta and slope in Q31 format.
        int32_t delta_val = val2 - val1;
        int32_t slope = (int32_t)(((int64_t)delta_val << 31) / seg_length);

        // Process the segment in pairs for SIMD efficiency.
        int offset = pos1;
        int samples = seg_length;
        int j = 0;

        // Process in pairs.
        for (; j <= samples - 2; j += 2) {
            int32_t prod0 = (int32_t)(((int64_t)slope * j) >> 31);
            int32_t prod1 = (int32_t)(((int64_t)slope * (j + 1)) >> 31);
            envelope[offset + j]     = val1 + prod0;
            envelope[offset + j + 1] = val1 + prod1;
        }
        // Process the remaining sample, if any.
        if (j < samples) {
            int32_t prod = (int32_t)(((int64_t)slope * j) >> 31);
            envelope[offset + j] = val1 + prod;
        }
    }

    // Fill the portion of the signal to the right of the last extremum with its constant value.
    int last_pos = extrema_pos[num_extrema - 1];
	#pragma vector_for
    for (i = last_pos; i < signal_length; i++) {
        envelope[i] = extrema_val[num_extrema - 1];
    }
}

void emd_decompose(int32_t* signal, int length) {
    int num_max = 0, num_min = 0;

    // Process the first element separately.
    if (length > 1) {
        if (signal[0] > signal[1]) {
            max_pos[num_max] = 0;
            max_val[num_max++] = signal[0];
        } else if (signal[0] < signal[1]) {
            min_pos[num_min] = 0;
            min_val[num_min++] = signal[0];
        }
    }

    // Process inner elements.
    #pragma vector_for
    for (int i = 1; i < length - 1; i++) {
        if (signal[i] > signal[i - 1] && signal[i] > signal[i + 1]) {
            max_pos[num_max] = i;
            max_val[num_max++] = signal[i];
        } else if (signal[i] < signal[i - 1] && signal[i] < signal[i + 1]) {
            min_pos[num_min] = i;
            min_val[num_min++] = signal[i];
        }
    }

    // Process the last element separately.
    if (length > 1) {
        if (signal[length - 1] > signal[length - 2]) {
            max_pos[num_max] = length - 1;
            max_val[num_max++] = signal[length - 1];
        } else if (signal[length - 1] < signal[length - 2]) {
            min_pos[num_min] = length - 1;
            min_val[num_min++] = signal[length - 1];
        }
    }

    int32_t* upper_env = upper_env_buffer;
    int32_t* lower_env = lower_env_buffer;

    // Perform linear interpolation for speed.
    linear_interp_simd(max_pos, max_val, num_max, upper_env, length);
    linear_interp_simd(min_pos, min_val, num_min, lower_env, length);

    // Subtract the average of the upper and lower envelopes from the signal.
    #pragma vector_for
    for (int i = 0; i < length; i++) {
        signal[i] -= (upper_env[i] + lower_env[i]) >> 1;
    }
}

void convert_to_q16_16(const unsigned char* input, int32_t* output, int size) {
    #pragma SIMD_for
    for (int i = 0; i < size; i++) {

        output[i] = ((int32_t)input[i]) << 16;
    }

}

void convert_from_q16_16(const int32_t* input, unsigned char* output, int size) {
    #pragma SIMD_for
    for (int i = 0; i < size; i++) {
        int32_t val = (input[i] + (1 << 15)) >> 16; // Add rounding offset
        output[i] = (val < 0) ? 0 : (val > 255 ? 255 : val);
    }
}


