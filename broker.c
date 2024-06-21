#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "functions.h"

int main(int argc, char *argv[]) {
    //Obtener datos
    int f = atoi(argv[2]), W = atoi(argv[6]);
    float u = atof(argv[4]), p = atof(argv[3]), v = atof(argv[5]);
    char *N = argv[1] , *C = argv[7], *R = argv[8];
}