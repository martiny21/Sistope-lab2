#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "functions.h"

int main(int argc, char *argv[]){
    int f = 3, W = 1;                       //f: cantidad de fitros(3), W: workers(1)
    float u = 0.5, p = 1.3, v = 0.5;        //u: umbral Binarizacion(0.5), p: factor de saturacion(1.3), v: Umbral clasificacion(0.5)
    char *C = (char *)malloc(100 * sizeof(char));   //Nombre carpeta resultante. Parametro obligatorio
    char *R = (char *)malloc(100 * sizeof(char));   //Nombre archivo CSV. Parametro obligatorio
    char *N = (char *)malloc(50 * sizeof(char));   //Nombre prefijo imagenes(Imagen)
    char fStr[20],WStr[20],uStr[20],pStr[20],vStr[20]; //Strings para convertir los valores a string

    if (N == NULL || C == NULL || R == NULL) {
        printf("Error en la asignacion de memoria\n");
        free(N);
        free(C);
        free(R);
        return 1;
    }

    int option;

    //Lectura de argumentos usando getopt()
    while((option = getopt(argc, argv, "N:f:p:u:v:W:C:R:l")) != -1) { //l simplemente es un parche para que pueda leer R
        switch (option)
        {
        case 'N':
            char *Prefix = optarg;
            if(strlen(Prefix) >= 50 )   
            {
                char *temp = (char *)realloc(N, (strlen(Prefix + 1) * sizeof(char))); // +1 para el carácter nulo
                if (temp == NULL) {
                    printf("Error en la reasignacion de memoria\n");
                    free(N);                                        // Liberar la memoria anterior si realloc falla
                    free(C);
                    free(R);
                    return 1;
                }
                N = temp;  
            } else {
                strcpy(N, Prefix); // Asignar el puntero realocado a la variable original
            }
            break;
        case 'f':
            f = atoi(optarg); //-f: cantidad de filtros a aplicar.
            break;
        case 'p':
            p = atof(optarg); //-p: factor de saturación del filtro.
            break;
        case 'u':
            u = atof(optarg); //-u: UMBRAL para binarizar la imagen.
            break;
        case 'v':
            v = atof(optarg); //-v: UMBRAL para clasificación.
            break;
        case 'W':
            W = atoi(optarg); //• -W: Cantidad de workers a crear.
            break;
        case 'C':
            if(strlen(optarg) >= 100 )   
            {
                char *temp3 = (char *)realloc(C, (strlen(optarg + 1) * sizeof(char))); // +1 para el carácter nulo
                if (temp3 == NULL) {
                    printf("Error en la reasignacion de memoria\n");
                    free(N);                                        // Liberar la memoria anterior si realloc falla
                    free(C);
                    free(R);
                    return 1;
                }
                C = temp3;  
            } else {
                strcpy(C, optarg); // Asignar el puntero realocado a la variable original
            }
            break;
        case 'R':
            if(strlen(optarg) >= 100 )   
            {
                char *temp2 = (char *)realloc(R, (strlen(optarg + 1) * sizeof(char))); // +1 para el carácter nulo
                if (temp2 == NULL) {
                    printf("Error en la reasignacion de memoria\n");
                    free(N);                                        // Liberar la memoria anterior si realloc falla
                    free(C);
                    free(R);
                    return 1;
                }
                N = temp2;  
            } else {
                strcpy(R, optarg); // Asignar el puntero realocado a la variable original
            }
            printf("R: %s\n", R);
            break;
            break;
        }
    }

    // --------- Datos para debugear ------------
    /*
    strcpy(N, "img"); 
    strcpy(C, "testC"); //Nombre de la carpeta resultante
    strcpy(R, "testR"); //Nombre del archivo CSV
    */
    //Verificar que f sea un valor valido
    if((f > 3) || f < 0){
        printf("Por favor ingrese un valor entero para f entre 1 y 3\n");
        return 0;
    }

    //Verificar que se entregan los argumentos olbigatorios
    if (strlen(R) == 0 && strlen(C) == 0){
        printf("Falto el ingreso de parametros obligatorios, nombre de la carpeta resultante o nombre del archivo CSV\n");
        return 0;
    }

    if(W <= 0) {
        printf("Por favor ingrese un valor entero mayor que 0 para W\n");
        return 0;
    }

    //Creacion de carpeta       Quizas podria ser una funcion aparte
    char *carpeta = C;
    if (mkdir(carpeta,0777) == 0) {
        printf("La carpeta se creó correctamente.\n");
    } else {
        printf("Error al crear la carpeta.\n");
    }

    //Creacion archivo CSV
    FILE *fileCSV;
    fileCSV = fopen(R, "w"); // Abre el archivo en modo escritura ("w"), se cierra al finalizar el programa

    if (fileCSV == NULL) {
        printf("Error al abrir el archivo.\n");
        return 0;
    }

    //Archivo CSV con 2 columnas
    fprintf(fileCSV, "Nombre Imagen,Clasificacion\n");

    sprintf(fStr,"%d",f);
    sprintf(WStr,"%d",W);
    sprintf(uStr,"%.2f",u);
    sprintf(pStr,"%.2f",p);
    sprintf(vStr,"%.2f",v);
    //Todo esto es para convertir los valores a string y podria hacerse en una funcion aparte
    
    const char *argv2[] = {"./broker", N, fStr, pStr, uStr, vStr, WStr, C, R, (char *)NULL};

    
    

    pid_t pid = fork();
    if(pid == 0) {
        // Intentar usar broker
        if(execv("./broker", (char * const *)argv2) == -1) {
            perror("execv falló");
            exit(EXIT_FAILURE); // Termina el proceso hijo si execv falla
        }
    } else {
        pid_t childPid = wait(NULL); //Proceso padre espera al proceso hijo

        if (childPid == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }

        printf("Proceso hijo %d terminó\n", childPid);
    }

    if (N != NULL) {
        free(N);
        N = NULL; // Evita el uso de punteros colgantes
    }
    if (R != NULL) {
        free(R);
        R = NULL; // Evita el uso de punteros colgantes
    }
    if (C != NULL) {
        free(C);
        C = NULL; // Evita el uso de punteros colgantes
    }
    

    fclose(fileCSV);
    printf("Proceso padre terminó\n");
    return 0;
}