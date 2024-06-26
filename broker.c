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

    /*
    //Crear pipes
    int pipes[W][2];

    for(int i = 0; i < W; i++) {
        if(pipe(pipes[i] == -1)) {
            printf("Error al crear el pipe\n");
            exit(1);
        }
    }

    //Crear hijos
    for(int i = 0; i < W; i++) {
        //Crear hijos
        pid_t pid = fork();

        if(pid == 0) {
            close(pipes[i][1]); //Cerrar escritura en worker
            dup2(pipes[i][0], STDIN_FILENO);
            execv("./worker", argv, (char *)NULL);
            exit(1);

        } else if (pid > 0) {
            close(pipes[i][1]); //Cerrar extremo lectura en broker
        } else {
            printf("Error al crear worker");
            exit(1);
        }
    }
    */
    
}