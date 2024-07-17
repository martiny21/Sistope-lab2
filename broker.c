#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "functions.h"

int main(int argc, char *argv[])
{
    // Obtener datos
    int f = atoi(argv[2]), W = atoi(argv[6]);
    float u = atof(argv[4]), p = atof(argv[3]), v = atof(argv[5]);
    char *N = argv[1], *C = argv[7], *R = argv[8];

    int loop = 1; // Verdadero, es una bandera para continuar un ciclo

    printf("Hola. Soy un broker\n");

    // Crear pipes (Comunicacion bidereccional)
    int pipes1[W][2]; // pipes de los hijos, envia datos de cada worker al broker (hijo escribe, padre lee)
    int pipes2[W][2]; // pipes del padre, envia datos a los workers (hijo lee, padre escribe)

    for (int i = 0; i < W; i++)
    {
        if (pipe(pipes1[i]) == -1)
        {
            printf("Error al crear pipe\n");
            exit(1);
        }
        if (pipe(pipes2[i]) == -1)
        {
            printf("Error al crear pipe\n");
            exit(1);
        }
    }
    /*
    for(int i = 0; i < W; i++) {
        if(pipe(pipes1[i] == -1 || pipe(pipes2[i]) == -1)) {
            printf("Error al crear pipe\n");
            exit(1);
        }
    }
    */
    // Crear hijos
    for (int i = 0; i < W; i++)
    {
        pid_t pid = fork();

        if (pid == 0)
        {
            close(pipes2[i][1]);
            close(pipes1[i][0]);

            dup2(pipes2[i][0], STDIN_FILENO); // Redirigir stdin al pipe broker-to-worker
            dup2(pipes2[i][1], STDOUT_FILENO);

            execv("./worker", argv); // Ejecutar worker
            exit(1);
        }
        else if (pid > 0)
        {
            close(pipes2[i][0]); // Cerrar el extremo de lectura en el broker
            close(pipes1[i][1]);
        }
        else
        {
            printf("Error al crear worker");
            exit(1);
        }
    }

    // Variables para concatenacion de cadenas de caracteres
    char bmp[20] = ".bmp";
    char *name1;
    name1 = N;
    char signo = '_';
    char resultado[100]; // Se mantiene la declaración de resultado
    char slash = '/';
    char newPath[50]; // Se crea una arreglo de caracteres para guardar el directorio a llegar

    char PathSatured[100];
    char PathGray[100];
    char PathBinary[100];

    // Inicializar variables como una cadena vacía
    PathSatured[0] = '\0';
    PathGray[0] = '\0';
    PathBinary[0] = '\0';
    resultado[0] = '\0';
    newPath[0] = '\0';

    while (loop < 3)
    {
        // Concatenar prefijo (N) y símbolo _
        snprintf(resultado, sizeof(resultado), "%s%c", name1, signo);

        // Concatenar el número de loop con resultado
        snprintf(resultado + strlen(resultado), sizeof(resultado) - strlen(resultado), "%d", loop);

        // Concatenar la extension del archivo
        snprintf(resultado + strlen(resultado), sizeof(resultado) - strlen(resultado), "%s", bmp);

        printf("%s\n", resultado);
        // Lectura de archivo
        BMPImage *image;
        image = read_bmp(resultado);

        if(image == NULL){
            printf("No se pudo encontrar otra imagen o hubo un error en el proceso\n");   
            //fclose(fileCSV);
            return 0;
        }

        int ancho = image->width;
        int alto = image->height;
        int fragmentoWorkers = ancho / W; //Cantidad de columnas que corresponden a cada worker
        int UltimoWorker ; //Cantidad de columnas que le corresponden al ultimo worker (Se encarga de las columnas restantes)
        
        if (ancho % W == 0) {
            UltimoWorker = fragmentoWorkers;
        }

        for(int i = 0; i < W; i++) {
            //Definir las columnas que procesará el worker
            int inicio = fragmentoWorkers * i;
            int fin = inicio + fragmentoWorkers;
            //Columnas asigmadas al último worker
            if (W -1 == i) {
                fin = UltimoWorker;
            }

            char info[200];
            snprintf(info, sizeof(info), "%d %d %d %s\n", inicio, fin, alto, resultado);
            write(pipes2[i][1], info, strlen(info));
        }

        /*
        for(int i = 0; i < W; i++) {
            wait(NULL);
        }
        */
        

        loop++;
    }

    // Enviar nombres de archivos para aplicar los filtros (usar bucle(?))
}