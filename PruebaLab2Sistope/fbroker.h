#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "functions.h"
#define READ 0
#define WRITE 1


void pixels_per_worker(int alto, int ancho, int W, int *pixels);

void getPixels(RGBPixel *data, int Npixels, int i, RGBPixel *pixels);

BMPImage processImage(BMPImage *image, int W, char *argv[]);

void processPixels(RGBPixel *data, int totalPixels, int W, int *pixelsPerWorker, RGBPixel *NewData, char *argv[]);

void CreateWorker(RGBPixel *data, int NumberPixels, char *argv[], int iterator, RGBPixel *NewData);

void workerProcess(int *BrokerToWorker, int *WorkerToBroker, int numberPixels ,RGBPixel *data, RGBPixel *NewData, char *argv[]);

void brokerProcess(int *BrokerToWorker, int *WorkerToBroker, int numberPixels, RGBPixel *data, RGBPixel *NewData);

void putPixels(RGBPixel *SubData, RGBPixel *data, int numberPixels, int PixelsReady);

char* ConcatenateStringWithInt(const char* base, int i);

void moveFileToDirectory(const char* filename, const char* directory);