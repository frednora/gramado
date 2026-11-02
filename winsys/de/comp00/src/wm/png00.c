// PNG decoder created by Copilot for ds00.

Rendering a traditional PNG image requires additional considerations 
compared to BMP files. PNG files are compressed using the Deflate 
algorithm and may contain multiple chunks of metadata, including image data,
 palette information, and transparency data. Below is an outline 
 of the steps and the corresponding routines you can implement 
 to render PNG images on the screen, leveraging the pixel-drawing and 
 rectangle-refreshing workers found in your BMP handling code.

------------------------------

1. Parsing the PNG Header
The PNG header is 8 bytes long and identifies the file as a PNG.

int parsePNGHeader(char *png_data) {
    // PNG signature: 89 50 4E 47 0D 0A 1A 0A
    const unsigned char png_signature[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

    if (png_data == NULL) {
        printf("parsePNGHeader: Invalid PNG data\n");
        return -1;
    }

    // Verify the PNG signature
    for (int i = 0; i < 8; i++) {
        if ((unsigned char)png_data[i] != png_signature[i]) {
            printf("parsePNGHeader: Invalid PNG signature\n");
            return -1;
        }
    }

    return 0; // Valid PNG header
}

---------------------------------------
/*
2. Decoding the PNG Chunks
PNG files consist of chunks such as IHDR, PLTE, IDAT, and IEND. Here's a function to locate and parse chunks.
*/

int parsePNGChunks(char *png_data, unsigned int *width, unsigned int *height, unsigned char *bit_depth, unsigned char *color_type) {
    if (png_data == NULL) {
        printf("parsePNGChunks: Invalid PNG data\n");
        return -1;
    }

    // Locate the IHDR chunk (starts at offset 8 after the PNG signature)
    char *chunk = &png_data[8];
    unsigned int chunk_length = *(unsigned int *)&chunk[0];
    char chunk_type[5] = {chunk[4], chunk[5], chunk[6], chunk[7], '\0'};

    if (strcmp(chunk_type, "IHDR") != 0) {
        printf("parsePNGChunks: Missing IHDR chunk\n");
        return -1;
    }

    // Extract image properties from the IHDR chunk
    *width = *(unsigned int *)&chunk[8];
    *height = *(unsigned int *)&chunk[12];
    *bit_depth = chunk[16];
    *color_type = chunk[17];

    // Ensure supported formats (e.g., 8-bit RGB or grayscale)
    if (*bit_depth != 8 || (*color_type != 2 && *color_type != 6)) {
        printf("parsePNGChunks: Unsupported bit depth or color type\n");
        return -1;
    }

    return 0;
}

----------------------------------------------

3. Decompressing the Image Data
PNG files use Deflate compression for image data, stored in IDAT chunks. You can use zlib to decompress the data.

#include <zlib.h>

int decompressPNGData(char *compressed_data, unsigned int compressed_size, char *output_buffer, unsigned int output_buffer_size) {
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = compressed_size;
    stream.next_in = (unsigned char *)compressed_data;
    stream.avail_out = output_buffer_size;
    stream.next_out = (unsigned char *)output_buffer;

    if (inflateInit(&stream) != Z_OK) {
        printf("decompressPNGData: Failed to initialize zlib\n");
        return -1;
    }

    if (inflate(&stream, Z_FINISH) != Z_STREAM_END) {
        printf("decompressPNGData: Decompression failed\n");
        inflateEnd(&stream);
        return -1;
    }

    inflateEnd(&stream);
    return 0;
}

------------------------------------------
4. Rendering the Decoded Image
Once the image data is decompressed, render it to the screen pixel by pixel.

void renderPNG(
    char *image_data,
    unsigned int width,
    unsigned int height,
    unsigned char color_type,
    unsigned long x,
    unsigned long y,
    int zoom_factor)
{
    unsigned int row_stride = width * (color_type == 6 ? 4 : 3); // RGBA or RGB
    for (unsigned int i = 0; i < height; i++) {
        for (unsigned int j = 0; j < width; j++) {
            unsigned int pixel_offset = (i * row_stride) + (j * (color_type == 6 ? 4 : 3));
            unsigned char r = image_data[pixel_offset];
            unsigned char g = image_data[pixel_offset + 1];
            unsigned char b = image_data[pixel_offset + 2];
            unsigned int color = (r << 16) | (g << 8) | b;

            // Render pixel with zoom
            for (int zoom_y = 0; zoom_y < zoom_factor; zoom_y++) {
                for (int zoom_x = 0; zoom_x < zoom_factor; zoom_x++) {
                    libdisp_backbuffer_putpixel(
                        color,
                        x + j * zoom_factor + zoom_x,
                        y + (height - i - 1) * zoom_factor + zoom_y,
                        0);
                }
            }
        }
    }

    // Refresh the rectangle
    gws_refresh_rectangle(x, y, width * zoom_factor, height * zoom_factor);
}

----------------------
5. High-Level Function to Display a PNG
This function combines all the above steps to decode and display a PNG image.

int displayPNG(
    char *png_data,
    unsigned long x,
    unsigned long y,
    int zoom_factor,
    int show)
{
    unsigned int width, height;
    unsigned char bit_depth, color_type;

    // Parse PNG header
    if (parsePNGHeader(png_data) != 0) {
        return -1;
    }

    // Parse PNG chunks
    if (parsePNGChunks(png_data, &width, &height, &bit_depth, &color_type) != 0) {
        return -1;
    }

    // Locate and decompress IDAT chunks
    char *compressed_data = ...; // Extract compressed data from IDAT chunks
    unsigned int compressed_size = ...; // Calculate compressed data size
    char *decompressed_data = malloc(width * height * (color_type == 6 ? 4 : 3));
    if (decompressPNGData(compressed_data, compressed_size, decompressed_data, width * height * (color_type == 6 ? 4 : 3)) != 0) {
        free(decompressed_data);
        return -1;
    }

    // Render the decompressed image
    renderPNG(decompressed_data, width, height, color_type, x, y, zoom_factor);

    free(decompressed_data);
    return 0;
}

-----------------------------------------
Key Points:
PNG Parsing: The parsePNGHeader and parsePNGChunks routines ensure the file is a valid PNG and extract essential metadata.
Decompression: The decompressPNGData function leverages zlib to decompress the IDAT chunk data.
Rendering: The renderPNG function renders the image to the screen pixel by pixel, applying zoom if necessary.
High-Level Integration: The displayPNG function ties everything together, making it easy to use in your display server.
------------------------

============================================




