FROM ubuntu:18.04

RUN apt-get update && \
    apt-get install -y pocl-opencl-icd ocl-icd-opencl-dev gcc clinfo \
    && apt-get install -y libjpeg-dev

WORKDIR /app

COPY host.c /app/
COPY Makefile /app/
COPY stb_image_write.h /app/
COPY stb_image.h /app/
