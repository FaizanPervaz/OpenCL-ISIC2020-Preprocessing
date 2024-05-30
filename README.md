# OpenCL-ISIC2020-Preprocessing
An OpenCL-based image preprocessing tool for converting the ISIC 2020 Challenge Dataset skin lesion images to grayscale to assist in melanoma detection

This repository hosts an OpenCL-based application designed to preprocess skin lesion images from the ISIC 2020 Challenge Dataset. By converting images to grayscale, the tool aids in the simplification of image data, enhancing further analysis for melanoma detection.

## Objective
The objective of this project is to leverage parallel computing capabilities of OpenCL to efficiently convert high-resolution dermatological images from color to grayscale, which is a crucial step in automated skin lesion analysis.

## Dataset
The tool works with the ISIC 2020 Challenge Dataset, which includes dermoscopic images of various skin lesions, essential for developing and testing melanoma detection algorithms.
Link to Dataset :
https://isic-challenge-data.s3.amazonaws.com/2020/ISIC_2020_Test_JPEG.zip

## Features
- **Parallel Image Processing**: Utilizes the parallel processing power of GPUs through OpenCL to handle large datasets efficiently.
- **Grayscale Conversion**: Converts RGB images to grayscale, reducing the complexity and size of the image data, focusing on the textural and structural attributes of the lesions.

## How to Run
- Install Docker Desktop and Build Docker using this Command docker build -t opencl-image .
- Run Docker Using this Command :
- docker run -v C:\Users\faiza\Downloads\Compressed\OpenCL-and-Docker-main\OpenCL-and-Docker-main:/app -it opencl-image
- OR
- docker run -v <path_to_project> -it opencl-image
- To Compile : 
- gcc -o rgb_to_grayscale host.c -lOpenCL -lm
- To Run :
- ./rgb_to_grayscale

