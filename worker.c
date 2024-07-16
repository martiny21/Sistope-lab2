#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "functions.h"

int main(int argc, char *argv[]){
    char buffer[100];

    printf("Hola. Soy un worker\n");
    /*
    Leer nombre de imagen para luego aplicar los filtros
    -->Usar fgets para esto
    
    Aplicar filtros (switch case)

    Enviar fragmentos de imagen al broker
    */
}