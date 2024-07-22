#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "BMPImage.h"
#include "BMPHeader.h"
#include "BMPInfoHeader.h"

RGBPixel *saturate_pixel(float factor, RGBPixel pixel);
RGBPixel *grayScale_pixel(RGBPixel pixel);
RGBPixel *binarize_pixel(float factor, RGBPixel pixel);