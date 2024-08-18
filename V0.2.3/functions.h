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
#define ERROR 1
#define READ 0
#define WRITE 1



BMPImage* read_bmp(const char* filename); //Leer imagenes

void free_bmp(BMPImage* image);                         //Liberar espacio de imagen

void write_bmp(const char* filename, BMPImage* image); //Escribir imagenes

int nearly_black(BMPImage* image,float factor); //Clasificacion de imagenes

BMPImage * receiveImages(int pipe[2], int numImages); //Recibir imagenes

int CountImages(char* prefix); //Contar imagenes

/*--- Lectura de imagenes ---*/

void ReadImages(BMPImage *NewImages,int *fd, int N);

void ReadImages2(BMPImage *NewImages,int *fd, int *fd2, int N);

void ReadImages3(BMPImage *NewImages, int *fd, int *fd2, int N);