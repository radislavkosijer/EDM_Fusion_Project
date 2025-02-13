import struct

def read_fused_image(filename):
    """
    @brief Reads a binary file containing custom image data.
    
    The file format consists of:
      - First 4 bytes: image width (unsigned int, little-endian)
      - Next 4 bytes: image height (unsigned int, little-endian)
      - Remaining bytes: pixel data (1 byte per pixel, grayscale)
    
    @param filename Path to the binary file.
    
    @return A tuple (width, height, pixels) where:
            - width: image width,
            - height: image height,
            - pixels: byte array of pixel data.
    
    @exception ValueError If the file is too short or if the pixel data is insufficient.
    """
    with open(filename, 'rb') as f:
        header_data = f.read(8)
        if len(header_data) != 8:
            raise ValueError("File is too short: missing dimensions.")
        # Unpack width and height using little-endian format
        width, height = struct.unpack('<II', header_data)
        num_pixels = width * height
        pixels = f.read(num_pixels)
        if len(pixels) != num_pixels:
            raise ValueError("Insufficient number of pixels in the file.")
    return width, height, pixels

def load_bmp_header(template_filename, header_size=54):
    """
    @brief Loads the first 'header_size' bytes from a template BMP image.
    
    This function reads the BMP header from an existing file, which is then used
    as a template for creating a new BMP image.
    
    @param template_filename Path to the BMP template file.
    @param header_size Number of bytes to read from the header. Default is 54.
    
    @return A bytearray representing the BMP header.
    
    @exception ValueError If the loaded header does not have the correct size.
    """
    with open(template_filename, 'rb') as f:
        header = f.read(header_size)
        if len(header) != header_size:
            raise ValueError("Template header is invalid (not 54 bytes).")
    return bytearray(header)

def create_grayscale_palette():
    """
    @brief Creates a 256-color palette for an 8-bit grayscale BMP image.
    
    Each palette entry consists of 4 bytes: [Blue, Green, Red, Reserved].
    The Blue, Green, and Red values are equal (ranging from 0 to 255), and
    the Reserved byte is set to 0.
    
    @return A bytearray containing the 256-color grayscale palette.
    """
    palette = bytearray()
    for i in range(256):
        palette += struct.pack('BBBB', i, i, i, 0)
    return palette

def update_bmp_header_for_grayscale(header, width, height, pixel_data_size):
    """
    @brief Updates the BMP header for an 8-bit grayscale image.
    
    This function modifies the following header fields:
      - bfSize (offset 2, 4 bytes): total file size = header (54) + palette (1024) + pixel_data_size
      - bfOffBits (offset 10, 4 bytes): offset from file start to pixel data (54 + 1024)
      - biWidth (offset 18, 4 bytes): image width
      - biHeight (offset 22, 4 bytes): image height
      - biBitCount (offset 28, 2 by
