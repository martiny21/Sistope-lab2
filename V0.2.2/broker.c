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

        // Concatenar prefijo (N) y símbolo _
        snprintf(resultado, sizeof(resultado), "%s%c", N, signo);

        // Concatenar el número de loop con resultado
        snprintf(resultado + strlen(resultado), sizeof(resultado) - strlen(resultado), "%d", loop);

        // Concatenar la extension del archivo
        snprintf(resultado + strlen(resultado), sizeof(resultado) - strlen(resultado), "%s", bmp);
        
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

        BMPImage NewImage = processImage(image, W, argv);
        
        write_bmp("resultado.bmp", &NewImage);
        
       

        /* - - - - - */
        //printf("Rojo imagen modificada %d: %d\n", loop, Images[0].data[0].r);
        loop++;
        

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