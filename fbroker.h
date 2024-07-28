#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "functions.h"
#define ERROR 1

int ** create_array_pipes(int w);

void free_pipes(int **pipes, int w);

void send_data(int **pipes, int W, RGBPixel *data, int Npixels, int lastPixels);

void receive_data(int **pipes, int W, RGBPixel *data, int Npixels, int lastPixels);

void wait_for_workers(int W);

void pixels_per_worker(int alto, int W, int *pixels);

//void create_sons(int W, int **pipes1, int **pipes2, const char *argvW[], const char *argvLW[]);

/* Improvisando */

void CreateWorker(RGBPixel *pixels, const char *argvW[], int **pipesRead, int **pipesWrite, int i);

void getPixels(RGBPixel *data, int Npixels, int i, RGBPixel *pixels);

void create_sons(int Workers, int **pipesRead, int **pipesWrite, const char *argvW[], const char *argvLW[], RGBPixel *data, int Alto);