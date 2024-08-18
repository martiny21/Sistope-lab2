#include "fbroker.h"

int main(int argc, char *argv[])
{
    // Obtener datos
    int W = atoi(argv[6]);
    char *UnsgdN = argv[1];                      // falta la asignacion de memoria con malloc
    char *N = (char *)malloc(50 * sizeof(char)); // Nombre prefijo imagenes(Imagen)
    int nImages = atoi(argv[7]);
    int loop = 1; // Verdadero, es una bandera para continuar un ciclo

    if (strlen(UnsgdN) >= 50)
    {
        char *temp = (char *)realloc(N, (strlen(UnsgdN + 1) * sizeof(char))); // +1 para el carácter nulo
        if (temp == NULL)
        {
            printf("Error en la reasignacion de memoria\n");
            fflush(stdout);
            free(N); // Liberar la memoria anterior si realloc falla
            return 1;
        }
        N = temp;
    }
    else
    {
        strcpy(N, UnsgdN); // Asignar el puntero realocado a la variable original
    }

    printf("Hola. Soy un broker\n");
    fflush(stdout);

    // Variables para concatenacion de cadenas de caracteres
    char Final[20] = "Final";
    char bmp[20] = ".bmp";
    char signo = '_';
    char resultado[100]; // Se mantiene la declaración de resultado
    char resultadoFInal[100];

    resultado[0] = '\0';

    BMPImage *Images = NULL;

    BMPImage NewImages[nImages];

    int fd[2];
    pipe(fd);

    int fd2[2];
    pipe(fd2);

    while (loop <= nImages)
    {

        // Concatenar prefijo (N) y símbolo _
        snprintf(resultado, sizeof(resultado), "%s%c", N, signo);
        //snprintf(resultadoFInal, sizeof(resultadoFInal), "%s", Final);

        // Concatenar el número de loop con resultado
        snprintf(resultado + strlen(resultado), sizeof(resultado) - strlen(resultado), "%d", loop);
        //snprintf(resultadoFInal + strlen(resultadoFInal), sizeof(resultadoFInal) - strlen(resultadoFInal), "%d", loop);

        // Concatenar la extension del archivo
        snprintf(resultado + strlen(resultado), sizeof(resultado) - strlen(resultado), "%s", bmp);
        //snprintf(resultadoFInal + strlen(resultadoFInal), sizeof(resultadoFInal) - strlen(resultadoFInal), "%s", bmp);

        // Lectura de archivo
        BMPImage *image;
        image = read_bmp(resultado);

        if (image == NULL)
        {
            printf("No se pudo encontrar otra imagen o termino proceso\n");
            fflush(stdout);
            return 0;
        }

        int ancho = image->width;
        int alto = image->height;

        BMPImage NewImage = processImage(image, W, argv);

        NewImages[loop - 1] = NewImage;

        loop++;

        free(image->data);
        free(image);
    }

    write_bmp("resultad_1.bmp", &NewImages[0]);
    write_bmp("resultad_2.bmp", &NewImages[1]);

    //SendImages(NewImages, fd, fd2, nImages);

    // freeImages(Images,loop);
    /* - - - - - */

    free(N);
    printf("Fin del programa\n");
    fflush(stdout);
}