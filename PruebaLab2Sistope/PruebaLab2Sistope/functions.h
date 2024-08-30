#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <getopt.h>
#include "BMPImage.h"
#include "BMPHeader.h"
#include "BMPInfoHeader.h"
#define READ 0
#define WRITE 1



BMPImage* read_bmp(const char* filename); //Leer imagenes

void write_bmp(const char* filename, BMPImage* image); //Escribir imagenes

int nearly_black(BMPImage* image,float factor); //Clasificacion de imagenes

int CountImages(char* prefix); //Contar imagenes





