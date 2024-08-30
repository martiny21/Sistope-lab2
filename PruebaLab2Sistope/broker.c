#include "fbroker.h"

int main(int argc, char *argv[])
{
    // Obtener datos
    int W = atoi(argv[6]);
    char *UnsgdN = argv[1];                      
    char *N = (char *)malloc(50 * sizeof(char)); // Nombre prefijo imagenes(Imagen)
    int v = atoi(argv[5]);
    int nImages = atoi(argv[7]);
    int loop = 1; // Contador de imagenes
    char *R = argv[8];
    char *C = argv[9];

    if (N == NULL) {
        printf("Error en la asignacion de memoria\n");
        fflush(stdout);
        return 1;
    }


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

    // Variables para concatenacion de cadenas de caracteres
    char Final[20] = "Final";
    char bmp[20] = ".bmp";
    char signo = '_';
    char resultado[100]; // Se mantiene la declaración de resultado
    char resultadoFInal[100];

    resultado[0] = '\0';

    // Crear un arreglo de estructuras de imágenes
    BMPImage NewImages[nImages];

    int fd[2];
    pipe(fd);

    int fd2[2];
    pipe(fd2);

    while (loop <= nImages)
    {

        // Concatenar prefijo (N) y símbolo _
        snprintf(resultado, sizeof(resultado), "%s%c", N, signo);

        // Concatenar el número de loop con resultado
        snprintf(resultado + strlen(resultado), sizeof(resultado) - strlen(resultado), "%d", loop);

        // Concatenar la extension del archivo
        snprintf(resultado + strlen(resultado), sizeof(resultado) - strlen(resultado), "%s", bmp);

        // Lectura de archivo
        BMPImage *image = read_bmp(resultado);

        if (image == NULL)
        {
            printf("No se pudo encontrar otra imagen o termino proceso\n");
            fflush(stdout);
            free(N);
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

    int *clasificaciones = (int *)malloc(nImages * sizeof(int));

    FILE *fileCSV;
    fileCSV = fopen(R, "w"); // Abre el archivo en modo escritura ("w"), se cierra al finalizar el programa

    if (fileCSV == NULL) {
        printf("Error al abrir el archivo.\n");
        return 0;
    }

    // Escribir en el archivo CSV
    fprintf(fileCSV, "Imagen\tClasificacion\n\n");
    for(int i = 0; i < nImages; i++){
        clasificaciones[i] = nearly_black(&NewImages[i], v);
        
        char* resultado = ConcatenateStringWithInt("ImagenModificada", i + 1);
        fprintf(fileCSV, "%s\t%d\n", resultado, clasificaciones[i]);
        write_bmp(resultado, &NewImages[i]);
        moveFileToDirectory(resultado, C);

        free(NewImages[i].data);
    }

    // fin de escritura en archivo CSV
    fclose(fileCSV);
    free(clasificaciones);
    free(N);
    printf("Fin del programa broker\n");
    fflush(stdout);
    return 0;
}