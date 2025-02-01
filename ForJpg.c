#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <math.h>
#include <stdint.h>

void sobel_edge_detection(uint8_t *gray_image, uint8_t *output, int width, int height) {
    int Gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    int Gy[3][3] = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sumX = 0, sumY = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    int pixel = gray_image[(y + i) * width + (x + j)];
                    sumX += pixel * Gx[i + 1][j + 1];
                    sumY += pixel * Gy[i + 1][j + 1];
                }
            }
            int magnitude = sqrt(sumX * sumX + sumY * sumY);
            if (magnitude > 255) magnitude = 255;
            output[y * width + x] = magnitude;
        }
    }
}

void save_jpeg(const char *filename, uint8_t *image, int width, int height) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *outfile = fopen(filename, "wb");
    if (!outfile) {
        perror("Error opening output file");
        return;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 1; // Grayscale
    cinfo.in_color_space = JCS_GRAYSCALE;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 90, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer;
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer = &image[cinfo.next_scanline * width];
        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input.jpg> <output.jpg>\n", argv[0]);
        return 1;
    }

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *infile = fopen(argv[1], "rb");
    if (!infile) {
        perror("Error opening input file");
        return 1;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    int width = cinfo.output_width;
    int height = cinfo.output_height;
    int channels = cinfo.output_components;

    uint8_t *raw_image = malloc(width * height * channels);
    uint8_t *gray_image = malloc(width * height);
    uint8_t *output_image = calloc(width * height, sizeof(uint8_t));

    JSAMPROW row_pointer[1];
    while (cinfo.output_scanline < height) {
        row_pointer[0] = &raw_image[cinfo.output_scanline * width * channels];
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    // Convert RGB to grayscale
    for (int i = 0; i < width * height; i++) {
        gray_image[i] = (uint8_t)(0.3 * raw_image[i * channels] + 0.59 * raw_image[i * channels + 1] + 0.11 * raw_image[i * channels + 2]);
    }

    sobel_edge_detection(gray_image, output_image, width, height);
    save_jpeg(argv[2], output_image, width, height);

    free(raw_image);
    free(gray_image);
    free(output_image);

    return 0;
}
