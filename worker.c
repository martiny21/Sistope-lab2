#include "fworker.h"
#define ERROR 1

int main(int argc, char *argv[]){

    if (argc < 6) {
        fprintf(stderr, "Uso: %s <f> <p> <u> <v> <Npixels>\n", argv[0]);
        fprintf(stderr, "Error: Insufficient command line arguments\n");
        exit(ERROR);
    }

    // Imprimir los argumentos recibidos
    fprintf(stdout, "Uso: %s %s %s %s %s\n", argv[1], argv[2], argv[3], argv[4], argv[5]);
    fflush(stdout);
    fprintf(stdout, "Worker iniciado...\n");
    fflush(stdout);

    // Convertir los argumentos a los tipos correspondientes
    int f = atoi(argv[1]);
    float p = atof(argv[2]);
    float u = atof(argv[3]);
    float v = atof(argv[4]);
    int Npixels = atoi(argv[5]);
    fprintf(stderr, "Worker recibio %d pixeles\n", Npixels);
    // Imprimir las variables convertidas
    fprintf(stdout, "f = %d\n", f);
    fprintf(stdout, "p = %f\n", p);
    fprintf(stdout, "u = %f\n", u);
    fprintf(stdout, "v = %f\n", v);
    fprintf(stdout, "Npixels = %d\n", Npixels);
    
    
    /* ----- RE-Implementacion ----- */
    
    RGBPixel pixels[Npixels];
    size_t dataSize = Npixels * sizeof(RGBPixel);


    ssize_t bytes_read = read(STDIN_FILENO, pixels, dataSize);
    fprintf(stderr, "pixel rojo en worker: %d\n", pixels[0].r);

    if (bytes_read == -1) {
        perror("Error al leer datos del pipe");
        exit(ERROR);
    } else if (bytes_read != sizeof(pixels)) {
        fprintf(stderr, "Error: no se leyeron todos los datos del pipe\n");     //stderr es "standard error"
        exit(ERROR);
    }

    // Procesar los pixeles

    if (f == 1) {
        saturate_pixels(pixels, Npixels, p);
        write(STDOUT_FILENO, pixels, sizeof(pixels));
    } else if (f == 2) {
        saturate_pixels(pixels, Npixels, p);
        grayScale_pixels(pixels, Npixels);
        write(STDOUT_FILENO, pixels, sizeof(pixels));
    } else {
        saturate_pixels(pixels, Npixels, p);
        grayScale_pixels(pixels, Npixels);
        binarize_pixels(pixels, Npixels, u);
        write(STDOUT_FILENO, pixels, sizeof(pixels));
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