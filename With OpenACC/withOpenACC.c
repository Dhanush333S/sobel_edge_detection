#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stb_image.h"
#include "stb_image_write.h"

#define SOBEL_KERNEL_SIZE 3

// Sobel Kernels
const int Gx[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
};

const int Gy[3][3] = {
    {-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}
};

// Sobel Edge Detection using OpenACC
void sobel_filter(unsigned char *input, unsigned char *output, int width, int height, int channels) {
    #pragma acc data copyin(input[0:width*height*channels]) copyout(output[0:width*height])
    {
        #pragma acc parallel loop collapse(2)
        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                int Gx_sum = 0;
                int Gy_sum = 0;

                // Compute gradient for grayscale image
                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        int pixel = input[((y + ky) * width + (x + kx)) * channels]; // Use red channel
                        Gx_sum += pixel * Gx[ky + 1][kx + 1];
                        Gy_sum += pixel * Gy[ky + 1][kx + 1];
                    }
                }

                // Compute gradient magnitude
                int magnitude = sqrtf(Gx_sum * Gx_sum + Gy_sum * Gy_sum);
                if (magnitude > 255) magnitude = 255;  // Normalize
                output[y * width + x] = (unsigned char)magnitude;
            }
        }
    }
}

int main() {
    // Load image
    int width, height, channels;
    unsigned char *input_image = stbi_load("input.jpg", &width, &height, &channels, 0);
    if (!input_image) {
        printf("Error loading image!\n");
        return -1;
    }

    printf("Image loaded: %dx%d, Channels: %d\n", width, height, channels);

    // Allocate memory for output
    unsigned char *output_image = (unsigned char *)malloc(width * height);

    // Apply Sobel filter
    sobel_filter(input_image, output_image, width, height, channels);

    // Save output image as grayscale PNG
    stbi_write_png("output.png", width, height, 1, output_image, width);

    // Free memory
    stbi_image_free(input_image);
    free(output_image);

    printf("Sobel edge detection completed! Output saved as 'output.png'\n");
    return 0;
}
