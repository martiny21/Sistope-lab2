#include "fworker.h"

int main(int argc, char *argv[]){
    char buffer[100];
    char nombreImg[200];
    int inicio, fin, alto;
    float f, p, u, v;

    /* ----- Implementacion ----- */
    //Cosas necesarias para el worker (Recibir informacion)
    //float p, u, v; int Npixels, f;
    int Npixels;


    RGBPixel *data;

    data = (RGBPixel*)malloc(sizeof(RGBPixel) * Npixels);

    read(STDIN_FILENO, data, Npixels * sizeof(RGBPixel));

    //RGBPixel *new_pixels;
    //new_pixels = (RGBPixel*)malloc(sizeof(RGBPixel) * Npixels); esto talvez no haga falta

    saturate_pixels(data, Npixels, p);

    if (f == 1 ){
        write(STDOUT_FILENO, data, Npixels * sizeof(RGBPixel));
        return 0;
    }

    grayScale_pixels(data, Npixels);

    if (f == 2){
        write(STDOUT_FILENO, data, Npixels * sizeof(RGBPixel));
        return 0;
    }

    binarize_pixels(data, Npixels, u);

    write(STDOUT_FILENO, data, Npixels * sizeof(RGBPixel));

    free(data);
    return 0;
    /* -------------------------- */

    while(fgets(buffer, sizeof(buffer), stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;

        //Leer informacion
        //sscanf(buffer, "%d %d %d %s", &inicio, &fin, &alto, nombreImg);
        //printf("%d %d %d %s\n", inicio, fin, alto, nombreImg);

    }

    
    printf("Hola. Soy un worker\n");
}