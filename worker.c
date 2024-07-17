#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "functions.h"

int main(int argc, char *argv[]){
    char buffer[100];
    int inicio, fin, alto;
    char nombreImg[200];

    while(fgets(buffer, sizeof(buffer), stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;

        //Leer informacion
        sscanf(buffer, "%d %d %d %s", &inicio, &fin, &alto, nombreImg);
        printf("%d %d %d %s\n", inicio, fin, alto, nombreImg);
    }

    

    printf("Hola. Soy un worker\n");
    /*
    Leer nombre de imagen para luego aplicar los filtros
    -->Usar fgets para esto
    
    Aplicar filtros (switch case)

    Enviar fragmentos de imagen al broker
    */
}