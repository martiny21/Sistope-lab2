#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "BMPImage.h"
#include "BMPHeader.h"
#include "BMPInfoHeader.h"


RGBPixel saturate_pixel(float factor, RGBPixel pixel);
RGBPixel grayScale_pixel(RGBPixel pixel);
RGBPixel binarize_pixel(float factor, RGBPixel pixel);

RGBPixel* saturate_pixels(RGBPixel *pixels, int Npixels, float factor);
RGBPixel* grayScale_pixels(RGBPixel *pixels, int Npixels);
RGBPixel* binarize_pixels(RGBPixel *pixels, int Npixels, float factor);