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
    
    printf("Hola. Soy un broker\n");
    
    //Crear pipes (Comunicacion bidereccional)
    int pipes1[W][2]; //pipes de los hijos, envia datos de cada worker al broker (hijo escribe, padre lee)
    int pipes2[W][2]; //pipes del padre, envia datos a los workers (hijo lee, padre escribe)

    for (int i = 0; i < W; i++) {
        if(pipe(pipes1[i]) == -1) {
            printf("Error al crear pipe\n");
            exit(1);
        }
        if(pipe(pipes2[i]) == -1) {
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
    //Crear hijos
    for(int i = 0; i < W; i++) {
        pid_t pid = fork();

        if(pid == 0) {
            close(pipes2[i][1]);
            close(pipes1[i][0]);

            dup2(pipes2[i][0], STDIN_FILENO); // Redirigir stdin al pipe broker-to-worker
            dup2(pipes2[i][1], STDOUT_FILENO);

            execv("./worker", argv); // Ejecutar worker
            exit(1);

        } else if (pid > 0) {
            close(pipes2[i][0]); // Cerrar el extremo de lectura en el broker
            close(pipes1[i][1]);
        } else {
            printf("Error al crear worker");
            exit(1);
        }
    }

    //Enviar nombres de archivos para aplicar los filtros (usar bucle(?))
    
}