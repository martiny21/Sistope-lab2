#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "functions.h"
#define ERROR 1
#define READ 0
#define WRITE 1

int ** create_array_pipes(int w);

void free_pipes(int **pipes, int w);


void wait_for_workers(int W);

void pixels_per_worker(int alto, int ancho, int W, int *pixels);

/* Improvisando */

//void CreateWorker(RGBPixel *pixels, char *argvW[], int **pipesRead, int **pipesWrite, int i, int Npixels);

void getPixels(RGBPixel *data, int Npixels, int i, RGBPixel *pixels);

void create_sons(int Workers, int **pipesRead, int **pipesWrite, char *argvW[], char *argvLW[], RGBPixel *data, int Alto, int ancho);

BMPImage * addImage(BMPImage *Images, int *count, BMPImage newImage);

BMPImage *formatImage(RGBPixel *data, BMPImage *image);

//void sendImages(BMPImage *Images, int count);

void freeImages(BMPImage *images, int count);

void readAllPixels(int **pipes1, int W, RGBPixel **ArrayPixels , int totalPixels, int pixelsPerWorker, int lastPixels);

void AllPixelsToOne(RGBPixel ** pixelsArray, int W, RGBPixel * NewData,int pixelsPerWorker, int lastPixels, int totalPixels);

void freePixelsArray(RGBPixel ** pixelsArray, int W);

void *AddImage(BMPImage *Images[], int count, BMPImage *newImage);

void printPixels(RGBPixel *pixels, int Npixels);


/* Todo bonito*/

BMPImage processImage(BMPImage *image, int W, char *argv[]);

void processPixels(RGBPixel *data, int totalPixels, int W, int *pixelsPerWorker, RGBPixel *NewData, char *argv[]);

void CreateWorker(RGBPixel *data, int NumberPixels, char *argv[], int iterator, RGBPixel *NewData);

void workerProcess(int *BrokerToWorker, int *WorkerToBroker, int numberPixels ,RGBPixel *data, RGBPixel *NewData, char *argv[]);

void brokerProcess(int *BrokerToWorker, int *WorkerToBroker, int numberPixels, RGBPixel *data, RGBPixel *NewData, char *argv[]);

void putPixels(RGBPixel *SubData, RGBPixel *data, int numberPixels, int PixelsReady);

//void sendImages(BMPImage images[], int *pipe, int numImages);

void sendImages2(BMPImage images[], int numImages);

void SendImages(BMPImage *Images, int *fd, int *fd2, int N);