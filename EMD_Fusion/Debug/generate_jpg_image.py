import struct
from PIL import Image

def read_fused_image(filename):
    """
    @brief Reads a binary file containing custom image data.
    
    The file format consists of:
      - First 4 bytes: image width (unsigned int, little-endian)
      - Next 4 bytes: image height (unsigned int, little-endian)
      - Remaining bytes: pixel data (1 byte per pixel, grayscale)
    
    @param filename Path to the binary file.
    
    @return A tuple (width, height, pixels) where:
            - width: the image width,
            - height: the image height,
            - pixels: a byte array containing the pixel data.
    
    @exception ValueError If the file is too short or the pixel data is insufficient.
    """
    with open(filename, 'rb') as f:
        header_data = f.read(8)
        if len(header_data) != 8:
            raise ValueError("File is too short: missing dimensions.")
        # Unpack the width and height using little-endian format
        width, height = struct.unpack('<II', header_data)
        num_pixels = width * height
        pixels = f.read(num_pixels)
        if len(pixels) != num_pixels:
            raise ValueError("Insufficient number of pixels in the file.")
    return width, height, pixels

def create_grayscale_jpg(binary_filename, output_filename, quality=95):
    """
    @brief Creates an 8-bit grayscale JPG image from a binary file.
    
    The process involves:
      1. Loading the image dimensions and pixel data from the binary file.
      2. Creating a PIL Image object from the pixel data.
      3. Saving the image as a JPG file with the specified quality.
    
    @param binary_filename Path to the binary file containing the image data.
    @param output_filename Path for the output JPG image.
    @param quality Quality setting for the output JPG image (default is 95).
    
    @return None
    """
    # Step 1: Load image dimensions and pixel data
    width, height, pixels = read_fused_image(binary_filename)
    print(f"Loaded data: width={width}, height={height}, number of pixels={len(pixels)}")

    # Step 2: Create a PIL Image object from the pixel data
    img = Image.frombytes('L', (width, height), pixels)
    
    # Step 3: Save the image as a JPG with the specified quality and optimization
    img.save(output_filename, quality=quality, optimize=True)
    print(f"Grayscale JPG image created: {output_filename}")

def main():
    """
    @brief Main function that sets up file paths and initiates the JPG image creation process.
    
    The function defines the paths for the input binary file and the output JPG image,
    and then calls create_grayscale_jpg to perform the conversion.
    
    @return None
    """
    # Configuration - adjust paths as necessary
    binary_filename = 'fused_image.bin'       # Input binary file
    output_filename = 'output_grayscale.jpg'    # Output JPG image
    
    create_grayscale_jpg(binary_filename, output_filename)

if __name__ == '__main__':
    main()
