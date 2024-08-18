#include "fworker.h"

/*
Descripcion: Función que satura un pixel de una imagen
usando un factor de saturación
Entrada: Factor de saturacion, pixel
Salida: Pixel saturado
*/
RGBPixel saturate_pixel(float factor, RGBPixel pixel)
{
    RGBPixel new_pixel;

    // Multiplicamos los valores originales por el factor
    float r = pixel.r * factor;
    float g = pixel.g * factor;
    float b = pixel.b * factor;

    // Limitamos cada canal a un máximo de 255
    if (r > 255)
    {
        new_pixel.r = 255;
    }
    else
    {
        new_pixel.r = (unsigned char)r;
    }

    if (g > 255)
    {
        new_pixel.g = 255;
    }
    else
    {
        new_pixel.g = (unsigned char)g;
    }

    if (b > 255)
    {
        new_pixel.b = 255;
    }
    else
    {
        new_pixel.b = (unsigned char)b;
    }

    return new_pixel;
}



/*
Descripcion: Función que realiza la conversión de un pixel de una
imagen a escala de grises
Entrada: Pixel
Salida: Pixel en escala de grises
*/
RGBPixel grayScale_pixel(RGBPixel pixel)
{
    RGBPixel new_pixel;
    unsigned char gray = (unsigned char)(pixel.r * 0.3 + pixel.g * 0.59 + pixel.b * 0.11);
    if (gray > 255)
    {
        gray = 255;
    }
    new_pixel.r = gray;
    new_pixel.g = gray;
    new_pixel.b = gray;

    return new_pixel;
}

/*
Descripcion: Función que realiza la binarización de un pixel de una
imagen en base a un umbral de binarización
Entrada: Umbral de binarización, pixel
Salida: Pixel binarizado
*/
RGBPixel binarize_pixel(float factor, RGBPixel pixel)
{
    RGBPixel new_pixel;
    int umbral = (int)fabs(255 * factor);

    // Verificar si el pixel pasa el umbral
    if ((pixel.r > umbral && pixel.g > umbral) && pixel.b > umbral)
    {
        new_pixel.r = 255;
        new_pixel.g = 255;
        new_pixel.b = 255;
    }
    else
    {
        new_pixel.r = 0;
        new_pixel.g = 0;
        new_pixel.b = 0;
    }

    return new_pixel;
}

/*
Descripcion: Función que satura un conjunto de pixeles de una imagen
usando un factor de saturación
Entrada: Arreglo de pixeles, cantidad de pixeles y factor de saturacion,
Salida: Pixel saturado
*/
RGBPixel * saturate_pixels(RGBPixel *pixels, int Npixels, float factor)
{   
    RGBPixel *new_pixels = (RGBPixel*)malloc(sizeof(RGBPixel) * Npixels);
    for (int i = 0; i < Npixels; i++)
    {
        new_pixels[i] = saturate_pixel(factor, pixels[i]);
    }
    return new_pixels;
}

/*
Descripcion: Función que convierte a escala de grises un conjunto de
pixeles de una imagen
Entrada: Arreglo de pixeles y cantidad de pixeles
Salida: Pixel saturado
*/
RGBPixel * grayScale_pixels(RGBPixel *pixels, int Npixels)
{
    RGBPixel *new_pixels = (RGBPixel*)malloc(sizeof(RGBPixel) * Npixels);
    for (int i = 0; i < Npixels; i++)
    {
        new_pixels[i] = grayScale_pixel(pixels[i]);
    }
    return new_pixels;
}

/*
Descripcion: Función que binariza un conjunto de pixeles de una imagen
usando un umbral de binarización
Entrada: Arreglo de pixeles, cantidad de pixles y umbral de binarización
Salida: Pixel saturado
*/
RGBPixel * binarize_pixels(RGBPixel *pixels, int Npixels, float factor)
{
    RGBPixel *new_pixels = (RGBPixel*)malloc(sizeof(RGBPixel) * Npixels);
    for (int i = 0; i < Npixels; i++)
    {   
        new_pixels[i] = binarize_pixel(factor, pixels[i]);
    }

    return new_pixels;
}