
#include "functions.h"

int main(int argc, char *argv[]){
    int f = 3, W = 1;                               //f: cantidad de fitros(3), W: workers(1)
    float u = 0.5, p = 1.3, v = 0.5;                //u: umbral Binarizacion(0.5), p: factor de saturacion(1.3), v: Umbral clasificacion(0.5)
    char *C = (char *)malloc(100 * sizeof(char));   //Nombre carpeta resultante. Parametro obligatorio
    char *R = (char *)malloc(100 * sizeof(char));   //Nombre archivo CSV. Parametro obligatorio
    char *N = (char *)malloc(50 * sizeof(char));    //Nombre prefijo imagenes(Imagen)
    char fStr[20],WStr[20],uStr[20],pStr[20],vStr[20]; //Strings para convertir los valores a string

    int IngresedN = -1;

    int fd[2], fd2[2]; // File descriptor para el pipe

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
                IngresedN = 1;  
            } else {
                strcpy(N, Prefix); // Asignar el puntero realocado a la variable original
                IngresedN = 1;
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
            break;
        }
    }

    //Verificar que se ingresen valores validos para f
    if(f != 3 && f != 2 && f != 1){
        printf("Por favor ingrese un valor entero para f entre 1 y 3\n");
        return 0;
    }

    //Verificar que se entregan los argumentos olbigatorios
    if (strlen(R) == 0 || strlen(C) == 0){
        printf("Falto el ingreso de parametros obligatorios, nombre de la carpeta resultante o nombre del archivo CSV\n");
        return 0;
    }

    //Verificar que se ingrese una cantidad de Workers valida
    if(W <= 0) {
        printf("Por favor ingrese un valor entero mayor que 0 para W\n");
        return 0;
    }

    //Verificar que se ingrese una factor de saturación valido
    if(p <= 0) {
        printf("Por favor ingrese un valor entero mayor que 0 para el factor de saturación (p)\n");
        return 0;
    }

    //Verificar que el umbral de binarización esté dentro de un rango valido
    if(u > 1 || u < 0) {
        printf("Por favor ingrese un valor entre 0 y 1 para el umbral de binarización (u)\n");
        return 0;
    }

    //Verificar que el umbral de clasificación esté dentro de un rango valido
    if(v >= 1 || v < 0) {
        printf("Por favor ingrese un valor entre 0 y 1 para el umbral de clasificación (v)\n");
        return 0;
    }

    if(IngresedN == -1){
        printf("Por favor ingrese el prefijo de las imagenes a trabajar (-N)\n");
        return 0;
    }

    //Creacion de carpeta
    char *carpeta = C;
    if (mkdir(carpeta,0777) == 0) {
        printf("La carpeta se creó correctamente.\n");
        fflush(stdout);
    } else {
        printf("Error al crear la carpeta.\n");
    }

    //Creacion de pipe
    if(pipe(fd) == -1) {
        fprintf(stderr,"pipe");
        exit(EXIT_FAILURE);
    }

    if(pipe(fd2) == -1) {
        fprintf(stderr,"pipe");
        exit(EXIT_FAILURE);
    }

    //Contar imágenes
    int nImages = CountImages(N);
    char StrImages[100];
    sprintf(StrImages, "%d", nImages);
    

    sprintf(fStr,"%d",f);
    sprintf(WStr,"%d",W);
    sprintf(uStr,"%.2f",u);
    sprintf(pStr,"%.2f",p);
    sprintf(vStr,"%.2f",v);

    
    const char *argv2[] = {"./broker", N, fStr, pStr, uStr, vStr, WStr, StrImages, R,C,(char *)NULL};

    strcat(R, ".csv");


    pid_t pid = fork();
    if(pid == 0) {

        // Intentar usar broker
        if(execv("./broker", (char * const *)argv2) == -1) {
            perror("execv falló");
            exit(EXIT_FAILURE); // Termina el proceso hijo si execv falla
        }
    } else {
        int status;
        pid_t childPid = waitpid(pid,&status,0); //Proceso padre espera al proceso hijo

        if (childPid == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }

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
    

    printf("Proceso padre terminó\n");
    return 0;
}