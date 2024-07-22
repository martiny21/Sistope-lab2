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
Descripcion: Función que envia los datos necesarios para procesar
una imagen a los workers
Entrada: Arreglo de pipes, cantidad de workers, arreglo de pixeles, 
Salida: N/A
*/
void send_data(int** pipes, int W, RGBPixel* data, int Npixels, int lastPixels) {
    /*
    Esto no esta terminado, falta dividir los datos y enviarlos a los workers
    */
    
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
        
        
        iter++;
        //Puede que este de mas
        iter2++;
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
    close(pipes[W][0]);
    
    
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
        wait(NULL);
    }
}

void receive_data(int** pipes, int W, RGBPixel* data, int Npixels, int lastPixels) {

    RGBPixel* dataReceived = (RGBPixel*)malloc(Npixels * sizeof(RGBPixel));
    int sizeUsed = 0;
    int i = 0, j = 0;
    while(i < W-1) {

        //cerrar el descriptor de escritura
        close(pipes[i][1]);
        read(pipes[i][0], dataReceived, Npixels * sizeof(RGBPixel));
        memcpy(data + sizeUsed*i, dataReceived, Npixels * sizeof(RGBPixel));
        
        i++;
    }
    
    if(lastPixels == Npixels) {
        close(pipes[i][1]);
        read(pipes[i][0], dataReceived, Npixels * sizeof(RGBPixel));
        memcpy(data + sizeUsed*i, dataReceived, Npixels * sizeof(RGBPixel));
    } else {
        close(pipes[i][1]);
        read(pipes[i][0], dataReceived, lastPixels * sizeof(RGBPixel));
        memcpy(data + sizeUsed*i, dataReceived, lastPixels * sizeof(RGBPixel));
    }

    free(dataReceived);
}