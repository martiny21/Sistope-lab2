#include "fbroker.h"

/*
Descripcion: Función que crea un arreglo de pipes para
comunicar el proceso broker con los workers
Entrada: Cantiad de workers
Salida: Arreglo de pipes
*/
int** create_array_pipes(int w) {
    // Reservar memoria para un arreglo de n pointers a arrays de 2 enteros (file descriptors)
    int **pipes = (int**)malloc(w * sizeof(int*));
    if (pipes == NULL) {
        printf("malloc error\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < w; i++) {
        // Reservar memoria para cada par de file descriptors
        pipes[i] = (int*)malloc(2 * sizeof(int));
        if (pipes[i] == NULL) {
            printf("Error al asignar memoria para pipes[%d]\n", i);
            exit(EXIT_FAILURE);
        }
        // Crear el pipe
        if (pipe(pipes[i]) == -1) {
            printf("Error al crear pipe\n");
            exit(EXIT_FAILURE);
        }
    }
    return pipes;
}



/*
Descripcion: Función que hace que el proceso broker
espere a que los workers terminen
Entrada: Cantidad de workers
Salida: N/A
*/
void wait_for_workers(int W) {
    for (int i = 0; i < W; i++) {
        pid_t childPid = wait(NULL); //Proceso padre espera al proceso hijo

        if (childPid == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
    }
}

// Muy probablemente esto este malo, hay que revisarlo -Martin
void receive_data(int** pipes, int W, RGBPixel* data, int Npixels, int lastPixels) {

    RGBPixel* dataReceived = (RGBPixel*)malloc(Npixels * sizeof(RGBPixel));
    int sizeUsed = 0;
    int i = 0;
    while(i < W-1) {

        //cerrar el descriptor de escritura
        close(pipes[i][1]);
        read(pipes[i][0], dataReceived, Npixels * sizeof(RGBPixel));
        memcpy(data + sizeUsed*i, dataReceived, Npixels * sizeof(RGBPixel));
        close(pipes[i][0]);
        
        i++;
    }
    
    if(lastPixels == Npixels) {
        close(pipes[i][1]);
        read(pipes[i][0], dataReceived, Npixels * sizeof(RGBPixel));
        memcpy(data + sizeUsed*i, dataReceived, Npixels * sizeof(RGBPixel));
        close(pipes[i][0]);
    } else {
        close(pipes[i][1]);
        read(pipes[i][0], dataReceived, lastPixels * sizeof(RGBPixel));
        memcpy(data + sizeUsed*i, dataReceived, lastPixels * sizeof(RGBPixel));
        close(pipes[i][0]);
    }

    free(dataReceived);
}


/*
Descripcion: Funcion que calcula la cantidad de pixeles para cada worker basado en en M % W
    - Pixels[0] = Cantidad de pixeles que le corresponden a cada worker a excepcion del ultimo
    - Pixels[1] = Cantidad de pixeles que le corresponden al ultimo worker
Entrada: Alto de la imagen, cantidad de workers, arreglo de numeros de pixeles
Salida: N/A
*/
void pixels_per_worker(int alto, int W, int *pixels){ 
    int fragmentoWorkers = alto / W; //Cantidad de pixeles que corresponden a cada worker
    int UltimoWorker ; //Cantidad de pixeles que le corresponden al ultimo worker (Se encarga de las columnas restantes)
    
    if (alto % W == 0) {
        UltimoWorker = fragmentoWorkers;
    }else{
        UltimoWorker = alto % W;
    }
    
    pixels[0] = fragmentoWorkers;
    pixels[1] = UltimoWorker;
}
/* ---- RE-RE-Implementacion ---- */
void CreateWorker(RGBPixel *pixels, char *argv[], int **pipes1, int **pipes2, int pipeNumber, int NumberPixels){
    pid_t pid = fork();
    if(pid == -1){
        printf("Error al crear worker\n");
        exit(ERROR);
    } else if (pid == 0){       //Proceso hijo
        printf("Creando worker\n");
        fflush(stdout);
        printf("Rojo pixel 0: %d\n", pixels[0].r);
        fflush(stdout);

        close(pipes1[pipeNumber][WRITE]); // Cerrar el extremo de escritura en el broker-to-worker
        close(pipes2[pipeNumber][READ]);   // Cerrar el extremo de lectura en el worker-to-broker

        // Duplicar el descriptor de lectura
        dup2(pipes1[pipeNumber][READ], STDIN_FILENO);    // Duplicar el descriptor de lectura
        dup2(pipes2[pipeNumber][WRITE], STDOUT_FILENO);  // Duplicar el descriptor de escritura

        // Ejecutar el worker
        if(execv("./worker", argv) == -1){            //Echar ojo a los argumentos
            printf("Error al ejecutar worker\n");
            exit(ERROR);
        }

        exit(ERROR);

    } else {                //Proceso padre
        size_t dataSize = NumberPixels * sizeof(RGBPixel);
        close(pipes1[pipeNumber][READ]); // Cerrar el extremo de lectura en el broker-to-worker
        close(pipes2[pipeNumber][WRITE]); // Cerrar el extremo de escritura en el worker-to-broker

        if(write(pipes1[pipeNumber][WRITE], pixels, dataSize ) != dataSize){
            printf("Error al escribir en el pipe\n");
            exit(ERROR);
            
        }
        close(pipes1[pipeNumber][WRITE]); // Cerrar el extremo de escritura en el broker-to-worker
    }
}


/* Idea crear una funcion para que cree un worker y solamente llamarlo n veces
Entradas para la funcion: arreglo de los pixeles, los argumentos que necesita el worker
Salidas para la funcion: N/A*/
/* ----- Idea funcion para un worker -----

/* Parte de improvisacion */
void getPixels(RGBPixel *data, int Npixels, int i, RGBPixel *pixels) {
    for (int j = 0; j < Npixels; j++) {
        pixels[j] = data[i * Npixels + j];
    }
}
/*--------------------------*/



/* ----- Re-Creacion de create_sons ----- */

void create_sons(int Workers, int **pipesRead, int **pipesWrite,  char *argvW[], char *argvLW[], RGBPixel *data, int Alto) {

    int PixelsForWorkers[2];
    pixels_per_worker(Alto, Workers, PixelsForWorkers);
    
    RGBPixel pixels[PixelsForWorkers[0]];
    for (int i = 0; i < Workers-1; i++){
        getPixels(data, PixelsForWorkers[0], i, pixels);
        CreateWorker(pixels, argvW, pipesRead, pipesWrite, i, PixelsForWorkers[0]);
    }
    getPixels(data, PixelsForWorkers[1], Workers-1, pixels);
    CreateWorker(pixels, argvLW, pipesRead, pipesWrite, Workers-1,PixelsForWorkers[1]);

}

/* --------------------------------------- */


void free_pipes(int** pipes, int w) {
    for (int i = 0; i < w; i++) {
        free(pipes[i]);
    }
    free(pipes);
}



BMPImage * addImage(BMPImage *Images, int *count, BMPImage newImage){
    BMPImage *temp = (BMPImage*)realloc(Images, sizeof(BMPImage) * (*count + 1));
    if (temp == NULL){
        fprintf(stderr,"Error al reasignar memoria\n");
        free(Images);
        return NULL;
    }
    Images = temp;
    Images[*count] = newImage;
    *count += 1;
    return Images;
}

void freeImages(BMPImage *images, int count) {
    for (int i = 0; i < count; i++) {
        free(images[i].data); // Liberar datos de la imagen si fueron asignados dinámicamente
    }
    free(images); // Liberar el arreglo de imágenes
}