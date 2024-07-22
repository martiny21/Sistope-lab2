#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "functions.h"

int ** create_array_pipes(int w);

void free_pipes(int **pipes, int w);

void send_data(int **pipes, int W, RGBPixel *data, int Npixels, int lastPixels);

void receive_data(int **pipes, int W, RGBPixel *data, int Npixels, int lastPixels);

void wait_for_workers(int W);