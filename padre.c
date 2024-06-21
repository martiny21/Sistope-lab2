#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

int main(int argc, char *argv[]){
    int f = 3, W = 1;                       //f: cantidad de fitros(3), W: workers(1)
    float u = 0.5, p = 1.3, v = 0.5;        //u: umbral Binarizacion(0.5), p: factor de saturacion(1.3), v: Umbral clasificacion(0.5)
    char *C = (char *)malloc(100 * sizeof(char));   //Nombre carpeta resultante. Parametro obligatorio
    char *R = (char *)malloc(100 * sizeof(char));   //Nombre archivo CSV. Parametro obligatorio
    char *N = (char *)malloc(50 * sizeof(char));   //Nombre prefijo imagenes(Imagen)

    int option;

    //Lectura de argumentos usando getopt()
    while((option = getopt(argc, argv, "N:f:p:u:v:W:C:R:l")) != -1) { //l simplemente es un parche para que pueda leer R
        switch (option)
        {
        case 'N':
            char *Prefix = optarg;
            if(strlen(Prefix) >= 50 )   
            {
                char *temp = (char *)realloc(N, (strlen(Prefix + 1) * sizeof(char)); // +1 para el carácter nulo
                if (temp == NULL) {
                    printf("Error en la reasignacion de memoria\n");
                    free(N);                                        // Liberar la memoria anterior si realloc falla
                    free(C);
                    free(R);
                    return 1;
                }
            }
            N = temp;                                               // Asignar el puntero realocado a la variable original
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
        case 'C':
            C = optarg; //-C: nombre de la carpeta resultante con las imágenes, con los filtros aplicados.
            break;
        case 'R':
            R = optarg; //-R: nombre del archivo CSV con las clasificaciones resultantes.
            break;
        }
    }

    

    //Verificar que f sea un valor valido
    if((f > 3) || f < 0){
        printf("Por favor ingrese un valor entero para f entre 1 y 3\n");
        return 0;
    }

    //Verificar que se entregan los argumentos olbigatorios
    if (R == NULL && C == NULL){
        printf("Falto el ingreso de parametros obligatorios, nombre de la carpeta resultante o nombre del archivo CSV\n");
        return 0;
    }

    if(W <= 0) {
        printf("Por favor ingrese un valor entero mayor que 0 para W\n");
        return 0;
    }

    free(N);
    free(C);
    free(R);
    return 0;
}