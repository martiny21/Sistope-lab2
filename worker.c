#include "fworker.h"

int main(int argc, char *argv[]){
    
    int Npixels = atoi(argv[5]);
    int f = atoi(argv[1]);
    float p = atof(argv[2]);
    float u = atof(argv[3]);
    float v = atof(argv[4]);

    /* ----- Implementacion ----- */
    //Cosas necesarias para el worker (Recibir informacion)
    //float p, u, v; int Npixels, f;

    printf("Worker iniciado...\n");

    RGBPixel *data;

    data = (RGBPixel*)malloc(sizeof(RGBPixel) * Npixels);

    read(STDIN_FILENO, data, Npixels * sizeof(RGBPixel));

    printf("Worker recibio %d pixeles\n", Npixels);
    printf("rojo primer pixel: %d\n", data[0].r);
    //RGBPixel *new_pixels;
    //new_pixels = (RGBPixel*)malloc(sizeof(RGBPixel) * Npixels); esto talvez no haga falta

    saturate_pixels(data, Npixels, p);

    if (f == 1 ){
        write(STDOUT_FILENO, data, Npixels * sizeof(RGBPixel));
        free(data);
        return 0;
    }

    grayScale_pixels(data, Npixels);

    if (f == 2){
        write(STDOUT_FILENO, data, Npixels * sizeof(RGBPixel));
        free(data);
        return 0;
    }

    binarize_pixels(data, Npixels, u);

    write(STDOUT_FILENO, data, Npixels * sizeof(RGBPixel));

    free(data);
    return 0;
    /* -------------------------- */
}