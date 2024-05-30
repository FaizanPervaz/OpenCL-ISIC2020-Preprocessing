#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <dirent.h> // Library for directory operations
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// OpenCL kernel source code for converting RGB to grayscale
const char *kernelSource =
    "__kernel void rgb_to_grayscale(__global const uchar *inputImage, __global uchar *outputImage, const int width, const int height) {\n"
    "    int x = get_global_id(0);\n"
    "    int y = get_global_id(1);\n"
    "    int i = (y * width + x) * 3;\n"
    "    int gray_idx = y * width + x;\n"
    "    uchar r = inputImage[i];\n"
    "    uchar g = inputImage[i + 1];\n"
    "    uchar b = inputImage[i + 2];\n"
    "    outputImage[gray_idx] = (uchar)(0.299f * r + 0.587f * g + 0.114f * b);\n"
    "}\n";

int main() {
    DIR *dir;
    struct dirent *ent;
    char folderPath[] = "ISIC_2020_Test_Input";

    cl_int err;
    cl_platform_id platform_id;
    cl_device_id device_id;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;

    // Obtain platform and device IDs
    err = clGetPlatformIDs(1, &platform_id, NULL);
    err |= clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, NULL);
    // Create OpenCL context
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    // Create command queue
    // queue = clCreateCommandQueue(context, device_id, 0, &err);
    queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
    const char *sources[] = { kernelSource };
    // Create OpenCL program from kernel source
    program = clCreateProgramWithSource(context, 1, sources, NULL, &err);
    clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    // Create OpenCL kernel
    kernel = clCreateKernel(program, "rgb_to_grayscale", &err);

    if ((dir = opendir(folderPath)) != NULL) {
        // Iterate through files in the directory
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) { // Only process regular files
                char filePath[1024];
                // Generate full file path
                sprintf(filePath, "%s/%s", folderPath, ent->d_name);

                // Load the RGB image using stb_image library
                int width, height, bpp;
                unsigned char* rgb_image = stbi_load(filePath, &width, &height, &bpp, 0);
                if (!rgb_image) {
                    fprintf(stderr, "Failed to load image %s\n", filePath);
                    continue;
                }

                // Allocate memory for the grayscale image
                unsigned char* gray_image = malloc(width * height);
                if (!gray_image) {
                    fprintf(stderr, "Failed to allocate memory for the grayscale image\n");
                    stbi_image_free(rgb_image);
                    continue;
                }

                size_t globalSize[2] = {width, height};
                // Create OpenCL buffer for input image
                cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, width * height * 3, rgb_image, &err);
                // Create OpenCL buffer for output (grayscale) image
                cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, width * height, NULL, &err);

                // Set kernel arguments
                clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBuffer);
                clSetKernelArg(kernel, 1, sizeof(cl_mem), &outputBuffer);
                clSetKernelArg(kernel, 2, sizeof(int), &width);
                clSetKernelArg(kernel, 3, sizeof(int), &height);

                // Execute the kernel
                clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, NULL, 0, NULL, NULL);
                clFinish(queue);
                // Read back the grayscale image from the GPU
                clEnqueueReadBuffer(queue, outputBuffer, CL_TRUE, 0, width * height, gray_image, 0, NULL, NULL);

                // Generate output file path
                char outputImagePath[1024];
                sprintf(outputImagePath, "%s_gray.jpg", filePath);
                // Save the grayscale image
                stbi_write_jpg(outputImagePath, width, height, 1, gray_image, 100);

                // Release OpenCL and memory resources
                clReleaseMemObject(inputBuffer);
                clReleaseMemObject(outputBuffer);
                stbi_image_free(rgb_image);
                free(gray_image);
            }
        }
        closedir(dir);
    } else {
        perror("Failed to open directory");
        return EXIT_FAILURE;
    }

    // Release OpenCL resources
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return EXIT_SUCCESS;
}
