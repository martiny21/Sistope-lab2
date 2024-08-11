#include "fbroker.h"

int main(int argc, char *argv[])
{
    // Obtener datos
    int  W = atoi(argv[6]);
    char *UnsgdN = argv[1];  //falta la asignacion de memoria con malloc
    char *N = (char *)malloc(50 * sizeof(char));   //Nombre prefijo imagenes(Imagen)
    int nImages = atoi(argv[7]);
    int loop = 1; // Verdadero, es una bandera para continuar un ciclo

    if(strlen(UnsgdN) >= 50 ){
        char *temp = (char *)realloc(N, (strlen(UnsgdN + 1) * sizeof(char))); // +1 para el carácter nulo
        if (temp == NULL) {
            printf("Error en la reasignacion de memoria\n");
            fflush(stdout);
            free(N);                                        // Liberar la memoria anterior si realloc falla
            return 1;
        }
    N = temp;
    } else {
        strcpy(N, UnsgdN); // Asignar el puntero realocado a la variable original
    }

    printf("Hola. Soy un broker\n");
    fflush(stdout);


    // Variables para concatenacion de cadenas de caracteres
    char bmp[20] = ".bmp";
    char signo = '_';
    char resultado[100]; // Se mantiene la declaración de resultado

    
    resultado[0] = '\0';

    BMPImage *Images = NULL;

    BMPImage *NewImages[nImages + 1];

    while (loop <= nImages)
    {
        /**
        // Crear pipes (Comunicacion bidereccional) // Taria bueno cambiar los nombres para que sean mas descriptivas
        int **pipes1 = create_array_pipes(W); // pipes de los hijos, envia datos de cada worker al broker (hijo escribe, padre lee)
        int **pipes2 = create_array_pipes(W); // pipes del padre, envia datos a los workers (hijo lee, padre escribe)
        */


        // Concatenar prefijo (N) y símbolo _
        snprintf(resultado, sizeof(resultado), "%s%c", N, signo);

        // Concatenar el número de loop con resultado
        snprintf(resultado + strlen(resultado), sizeof(resultado) - strlen(resultado), "%d", loop);

        // Concatenar la extension del archivo
        snprintf(resultado + strlen(resultado), sizeof(resultado) - strlen(resultado), "%s", bmp);
        //printf("%s\n", resultado);
        // Lectura de archivo
        BMPImage *image;
        image = read_bmp(resultado);

        if(image == NULL){
            printf("No se pudo encontrar otra imagen o termino proceso\n");
            fflush(stdout);
            return 0;
        }

        int ancho = image->width;
        int alto = image->height;

        BMPImage *NewImage = processImage(image, W, argv);
        
        
        int pixelsPerWorker[2];
        pixels_per_worker(alto, ancho,W, pixelsPerWorker);

        char StrNPixels[40];
        char StrLPixels[40];

        sprintf(StrNPixels, "%d", pixelsPerWorker[0]);
        sprintf(StrLPixels, "%d", pixelsPerWorker[1]);

        fprintf(stderr, "Pixeles por worker: %d\n", pixelsPerWorker[0]);
        fprintf(stderr, "Pixeles por ultimo worker: %d\n", pixelsPerWorker[1]);
        char *argvWorker[] = {"./worker", argv[2], argv[3], argv[4], argv[5], StrNPixels, (char *)NULL};
        char *argvLWorker[] = {"./worker", argv[2], argv[3], argv[4], argv[5], StrLPixels, (char *)NULL};
        int Npixels = pixelsPerWorker[0];
        RGBPixel NewData[ancho * alto];


        
        /* ---- RE-ideacion de la solucion --- */

        //Aqui
        create_sons(W, pipes1, pipes2, argvWorker, argvLWorker, image->data, alto, ancho);
        wait_for_workers(W);
        

        RGBPixel **pixelsArray = malloc(W * sizeof(RGBPixel *));
        for (int i = 0; i < W-1; i++) {
            pixelsArray[i] = malloc(Npixels * sizeof(RGBPixel));
        }
        pixelsArray[W-1] = malloc(pixelsPerWorker[1] * sizeof(RGBPixel));

        readAllPixels(pipes2, W, pixelsArray ,ancho * alto, pixelsPerWorker[0] ,pixelsPerWorker[1]);
        AllPixelsToOne(pixelsArray, W, NewData ,pixelsPerWorker[0], pixelsPerWorker[1], ancho * alto);
        //Aqui
        fprintf(stderr, "Rojo imagen original %d: %d\n", loop, image->data[0].r);
        fprintf(stderr, "Rojo imagen modificada: %d\n", NewData[0].r);
        /*Funciones a implementar*/
        BMPImage nuevaImagen;
        nuevaImagen.width = ancho;
        nuevaImagen.height = alto;

        nuevaImagen.data = malloc(sizeof(RGBPixel) * ancho * alto);
        if (nuevaImagen.data == NULL) {
            fprintf(stderr, "Error al asignar memoria para la imagen\n");
            freePixelsArray(pixelsArray, W);
            free_pipes(pipes1, W);
            free_pipes(pipes2, W);
            free(image->data);
            free(image);
            return 1;
        }

        memcpy(nuevaImagen.data, NewData, sizeof(RGBPixel) * ancho * alto);
        
        write_bmp("ImgDespues.bmp", &nuevaImagen);
        

        //addImage(Images,&loop, New);
        //AddImage(NewImages, loop, NewImage);
        //printf("Pixel imagen 1: R: %d  G: %d  B: %d", NewImages[1]->data[0].r, NewImages[1]->data[0].b, NewImages[1]->data[0].g);

        if (NewImages == NULL){
            fprintf(stderr, "Error al añadir la imagen\n");
            freePixelsArray(pixelsArray, W);
            free_pipes(pipes1, W);
            free_pipes(pipes2, W);
            free(image->data);
            free(image);
            return 1;
        }

        /* - - - - - */
        //printf("Rojo imagen modificada %d: %d\n", loop, Images[0].data[0].r);
        loop++;
        
        freePixelsArray(pixelsArray, W);
        free_pipes(pipes1, W);
        free_pipes(pipes2, W);
        free(image->data);
        free(image);
        
    }

    
    //sendImages(Images, loop);
    //freeImages(Images,loop);
    /* - - - - - */

    free(N);
    printf("Fin del programa\n");
    fflush(stdout);
}