/*
 * fusion.h
 *
 *  Created on: February 13, 2025.
 *      Author: Radislav Kosijer
 *
 * @brief Header file for fusion and saving images.
 */

#ifndef FUSION_H_
#define FUSION_H_



/**
 * @brief Fuse two images based on a decision mask.
 *
 * This function fuses two 8-bit images according to the provided decision mask.
 *
 * @param imgA       Pointer to the first image data.
 * @param imgB       Pointer to the second image data.
 * @param alpha_mask Decision mask determining pixel selection.
 * @param width      Image width.
 * @param height     Image height.
 * @param fused_img  Output array to store the fused image.
 */
void fuse_images(const unsigned char* imgA, const unsigned char* imgB, const char* alpha_mask, int width, int height, unsigned char* fused_img);


/**
 * @brief Perform linear histogram stretching on an 8-bit grayscale image.
 *
 * This function scans the image to find the minimum and maximum pixel values,
 * and then linearly stretches all pixel values to cover the full range [0..255].
 * If all pixels have the same value, no stretching is applied.
 *
 * @param img    Pointer to the input 8-bit grayscale image.
 * @param width  Image width.
 * @param height Image height.
 */
void histogram_stretch(unsigned char* img, int width, int height);

/**
 * @brief Save the fused image to a binary file.
 *
 * This function writes the fused image to a file in binary format. The file will contain
 * the image width and height (each stored as a 32-bit unsigned integer) followed by the pixel data.
 *
 * @param filename  Name of the file to write.
 * @param width     Image width.
 * @param height    Image height.
 * @param fused_img Pointer to the fused image data (8-bit values).
 */
void save_fused_image(const char *filename, unsigned int width, unsigned int height, const unsigned char *fused_img);

#endif /* FUSION_H_ */
