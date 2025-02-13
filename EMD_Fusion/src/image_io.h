/*****************************************************************************
 * image_io.h
 *
 * Created on: February 2, 2025.
 * Author: Radislav Kosijer
 *
 * @brief Header file for image I/O functions.
 *****************************************************************************/

#ifndef IMAGE_IO_H_
#define IMAGE_IO_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "led.h"

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

#endif /* IMAGE_IO_H_ */
