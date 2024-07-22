#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "functions.h"
#include "fworker.h"

int main(int argc, char *argv[]){
    char buffer[100];
    char nombreImg[200];
    int inicio, fin, alto;
    float f, p, u, v;

    while(fgets(buffer, sizeof(buffer), stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;

        //Leer informacion
        //sscanf(buffer, "%d %d %d %s", &inicio, &fin, &alto, nombreImg);
        //printf("%d %d %d %s\n", inicio, fin, alto, nombreImg);

    }

    
    printf("Hola. Soy un worker\n");
}