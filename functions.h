#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "BMPImage.h"
#include "BMPHeader.h"
#include "BMPInfoHeader.h"

BMPImage* read_bmp(const char* filename); //Leer imagenes

void free_bmp(BMPImage* image);                         //Liberar espacio de imagen

void write_bmp(const char* filename, BMPImage* image); //Escribir imagenes

BMPImage* grayScale_bmp(BMPImage* image);               //Escala de grises

BMPImage* saturate_bmp(BMPImage* image, float factor);  //Saturacion

BMPImage* binarize_bmp(BMPImage* image,float factor);    //Binarizacion

int nearly_black(BMPImage* image,float factor); //Clasificacion de imagenes