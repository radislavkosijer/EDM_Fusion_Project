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

/** Named constants for alpha mask decisions. */
#define ALPHA_A    0
#define ALPHA_B    1
#define ALPHA_AVG  2

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
		#pragma vector_for
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

void histogram_stretch(unsigned char* img, int width, int height){
    int num_pixels = width * height;
    unsigned char minVal = 255;
    unsigned char maxVal = 0;

    /* Find the minimum and maximum pixel values. */
    #pragma vector_for
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
