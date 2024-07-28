#include "fbroker.h"

/*
Descripcion: Función que crea un arreglo de pipes para
comunicar el proceso broker con los workers
Entrada: Cantiad de workers
Salida: Arreglo de pipes
*/
int** create_array_pipes(int w) {
    // Reservar memoria para un arreglo de n pointers a arrays de 2 enteros (file descriptors)
    int** pipes = (int**)malloc(w * sizeof(int*));
    if (pipes == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < w; i++) {
        // Reservar memoria para cada par de file descriptors
        pipes[i] = (int*)malloc(2 * sizeof(int));
        if (pipes[i] == NULL) {
            printf("Error al asignar memoria para pipes[%d]\n", i);
            exit(1);
        }

        // Crear el pipe
        if (pipe(pipes[i]) == -1) {
            printf("Error al crear pipe\n");
            exit(1);
        }
    }

    return pipes;
}
/*
Descripcion: Función que envia los datos necesarios para procesar una imagen a los workers
Entrada: Arreglo de pipes, cantidad de workers, arreglo de pixeles, cantidad de pixeles, 
        cantidad de pixeles que le corresponden al ultimo worker
Salida: N/A
*/
void send_data(int** pipes, int W, RGBPixel* data, int Npixels, int lastPixels) {
    /*
    Esto no esta terminado, falta dividir los datos y enviarlos a los workers
    */
    
    //Falta hecharle un ojo paso por paso antes de probarlo, es muy probable que no funcione como esperamos -Martin
    RGBPixel* dataToSend = (RGBPixel*)malloc(Npixels * sizeof(RGBPixel));

    int iter = 0;
    int iter2 = 0;
    while(iter < W-1) {
        
        // Cerrar el descriptor de lectura
        close(pipes[iter][0]);

        for(int k = 0; k < Npixels; k++) {
            dataToSend[k] = data[iter2];
            iter2++;
        }

        // Escribir en el descriptor de escritura
        write(pipes[iter][1], dataToSend, Npixels * sizeof(RGBPixel));
        
        close(pipes[iter][1]);
        
        iter++;
    }
    if(lastPixels == Npixels) {
        
        for(int k = 0; k < Npixels; k++) {
            dataToSend[k] = data[iter2];
            iter2++;
        }
        
        // Escribir en el descriptor de escritura
        write(pipes[iter][1], dataToSend, Npixels * sizeof(RGBPixel));
    } else {

        for(int k = 0; k < lastPixels; k++) {
            dataToSend[k] = data[iter2];
            iter2++;
        }

        // Escribir en el descriptor de escritura
        write(pipes[iter][1], dataToSend, lastPixels * sizeof(RGBPixel));
    }

    // Cerrar el descriptor de escritura
    close(pipes[W-1][0]);
    
    
    for(int j = 0; j < W; j++) {

        // Cerrar el descriptor de escritura
        close(pipes[j][1]);
    }

    free(dataToSend);
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

/* Idea crear una funcion para que cree un worker y solamente llamarlo n veces
Entradas para la funcion: arreglo de los pixeles, los argumentos que necesita el worker
Salidas para la funcion: N/A*/
/* ----- Idea funcion para un worker -----*/
void CreateWorker(RGBPixel *pixels, const char *argv[], int **pipesRead, int **pipesWrite, int pipeNumber){
    pid_t pid = fork();
    if(pid == -1){
        perror("fork");
        exit(ERROR);
    }

    if (pid > 0){
        close(pipesWrite[pipeNumber][0]); // Cerrar el extremo de lectura en el broker 
        
        write(pipesWrite[pipeNumber][1], pixels, sizeof(pixels)); // Se necesita tener los pixeles para enviarlos a los workers

        close(pipesWrite[pipeNumber][1]); // Cerrar el extremo de escritura en el broker
    } else {
        close(pipesRead[pipeNumber][1]); // Cerrar el extremo de escritura en el broker

        dup2(pipesRead[pipeNumber][0], STDIN_FILENO); // Duplicar el descriptor de lectura

        // Ejecutar el worker
        if (execv("./worker", (char* const*)argv) == -1) {
            perror("execv");
            exit(ERROR);
        }
    }
}
/* Parte de improvisacion */
void getPixels(RGBPixel *data, int Npixels, int i, RGBPixel *pixels) {
    for (int j = 0; j < Npixels; j++) {
        pixels[j] = data[i * Npixels + j];
    }
}
/*--------------------------*/



/* ----- Re-Creacion de create_sons ----- */

void create_sons(int Workers, int **pipesRead, int **pipesWrite, const char *argvW[], const char *argvLW[], RGBPixel *data, int Alto) {

    int PixelsForWorkers[2];
    pixels_per_worker(Alto, Workers, PixelsForWorkers);
    
    RGBPixel pixels[PixelsForWorkers[0]];
    for (int i = 0; i < Workers-1; i++){
        getPixels(data, PixelsForWorkers[0], i, pixels);
        CreateWorker(pixels, argvW, pipesRead, pipesWrite, i);
    }
    getPixels(data, PixelsForWorkers[1], Workers-1, pixels);
    CreateWorker(pixels, argvLW, pipesRead, pipesWrite, Workers-1);

}

/* --------------------------------------- */

/*
void create_sons(int W, int** pipes1, int** pipes2,const char *argvW[], const char *argvLW[]) {
    int i;
    printf("Creando workers\n");
    for (i = 0; i < W-1; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }else if (pid == 0) {
            // Cerrar los descriptores de lectura
            close(pipes1[i][0]);
            close(pipes2[i][0]);

            // Duplicar el descriptor de escritura
            dup2(pipes1[i][1], STDOUT_FILENO);
            dup2(pipes2[i][1], STDERR_FILENO);

            // Ejecutar el worker
            if (execv("./worker", (char* const*)argvW) == -1) {
                perror("execv");
                exit(EXIT_FAILURE);
            }
        } else {
            close(pipes2[i][0]); // Cerrar el extremo de lectura en el broker
            close(pipes1[i][1]); // Cerrar el extremo de escritura en el broker
        }
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if(pid == 0) {
        // Cerrar los descriptores de lectura
        close(pipes1[i][0]);
        close(pipes2[i][0]);

        // Duplicar el descriptor de escritura
        dup2(pipes1[i][1], STDOUT_FILENO);
        dup2(pipes2[i][1], STDERR_FILENO);

        // Ejecutar el worker
        if (execv("./worker", (char* const*)argvLW) == -1) {
            perror("execv");
            exit(EXIT_FAILURE);
        }else {printf("se ejecuto el worker\n");}
    }else if (pid > 0)
        {
            close(pipes2[i][0]); // Cerrar el extremo de lectura en el broker
            close(pipes1[i][1]); // Cerrar el extremo de escritura en el broker
        }
        else
        {
            printf("Error al crear worker");
            exit(1);
        }
}
*/

void free_pipes(int** pipes, int w) {
    for (int i = 0; i < w; i++) {
        free(pipes[i]);
    }
    free(pipes);
}