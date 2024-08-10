#include "fworker.h"
#define ERROR 1

int main(int argc, char *argv[]){

    if (argc < 6) {
        fprintf(stderr, "Uso: %s <f> <p> <u> <v> <Npixels>\n", argv[0]);
        fprintf(stderr, "Error: Insufficient command line arguments\n");
        exit(ERROR);
    }

    // Convertir los argumentos a los tipos correspondientes
    int f = atoi(argv[1]);
    float p = atof(argv[2]);
    float u = atof(argv[3]);
    float v = atof(argv[4]);
    int Npixels = atoi(argv[5]);
    
    fprintf(stderr, "Worker: Npixels=%d\n", Npixels);
    /* ----- RE-Implementacion ----- */
    
    RGBPixel *pixels = (RGBPixel*)malloc(Npixels * sizeof(RGBPixel));
    size_t dataSize = Npixels * sizeof(RGBPixel);


    ssize_t bytes_read = read(STDIN_FILENO, pixels, dataSize);
    
    fprintf(stderr, "Worker: rojo=%d\n", pixels[0].r);
    if (bytes_read == -1) {
        perror("Error al leer datos del pipe");
        exit(ERROR);
    } else if (bytes_read != dataSize) {
        fprintf(stderr, "Error: no se leyeron todos los datos del pipe\n");     //stderr es "standard error"
        exit(ERROR);
    }

    RGBPixel *new_pixels;

    // Procesar los pixeles

    if (f == 1) {
        new_pixels = saturate_pixels(pixels, Npixels, p);
        if(write(STDOUT_FILENO, new_pixels, dataSize) != dataSize){
            perror("Error al escribir datos en el pipe");
            exit(ERROR);
        }
    } else if (f == 2) {
        new_pixels = saturate_pixels(pixels, Npixels, p);
        new_pixels = grayScale_pixels(new_pixels, Npixels);
        if(write(STDOUT_FILENO, new_pixels, dataSize) != dataSize){
            perror("Error al escribir datos en el pipe");
            exit(ERROR);
        }
    } else {
        new_pixels =saturate_pixels(pixels, Npixels, p);
        new_pixels = grayScale_pixels(new_pixels, Npixels);
        new_pixels = binarize_pixels(new_pixels, Npixels, u);
        if(write(STDOUT_FILENO, new_pixels, dataSize) != dataSize){
            perror("Error al escribir datos en el pipe");
            exit(ERROR);
        }
    }

    
    return 0;

    /* -------------------------- */


    /* ----- Implementacion ----- 
    //Cosas necesarias para el worker (Recibir informacion)
    //float p, u, v; int Npixels, f;

    printf("Worker iniciado...\n");

    RGBPixel *data;
    data = (RGBPixel*)malloc(sizeof(RGBPixel) * Npixels);
    RGBPixel *new_pixels;
    new_pixels = (RGBPixel*)malloc(sizeof(RGBPixel) * Npixels);// esto talvez no haga falta

    fgets((char *)data, Npixels * sizeof(RGBPixel), stdin);

    printf("Worker recibio %d pixeles\n", Npixels);
    printf("rojo primer pixel: %d\n", data[0].r);
    

    saturate_pixels(new_pixels, Npixels, p);

    if (f == 1 ){
        write(STDOUT_FILENO, new_pixels, Npixels * sizeof(RGBPixel));
        free(data);
        return 0;
    }

    grayScale_pixels(new_pixels, Npixels);

    if (f == 2){
        write(STDOUT_FILENO, new_pixels, Npixels * sizeof(RGBPixel));
        free(data);
        return 0;
    }

    binarize_pixels(new_pixels, Npixels, u);

    write(STDOUT_FILENO, new_pixels, Npixels * sizeof(RGBPixel));

    free(data);
    free(new_pixels);
    return 0;
    /* -------------------------- */
}