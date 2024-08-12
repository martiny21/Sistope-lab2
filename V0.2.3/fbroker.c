#include "fbroker.h"
    
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



/*
Descripcion: Funcion que calcula la cantidad de pixeles para cada worker basado en en M % W
    - Pixels[0] = Cantidad de pixeles que le corresponden a cada worker a excepcion del ultimo
    - Pixels[1] = Cantidad de pixeles que le corresponden al ultimo worker
Entrada: Alto de la imagen, cantidad de workers, arreglo de numeros de pixeles
Salida: N/A
*/
void pixels_per_worker(int alto, int ancho, int W, int *pixels){ 
    int Workers = ancho / W; //Cantidad de pixeles que corresponden a cada worker
    //Cantidad de pixeles que le corresponden al ultimo worker (Se encarga de las columnas restantes)
    int fragmentoWorkers = Workers * alto;
    int UltimoWorker = (alto * ancho) - fragmentoWorkers;
    
    pixels[0] = fragmentoWorkers;
    pixels[1] = UltimoWorker;
}

/* Idea crear una funcion para que cree un worker y solamente llamarlo n veces
Entradas para la funcion: arreglo de los pixeles, los argumentos que necesita el worker
Salidas para la funcion: N/A*/
/* ----- Idea funcion para un worker -----

/* Parte de improvisacion */
/*
    Descripcion: funcion que traspasa ciertos pixeles a un arreglo
*/
void getPixels(RGBPixel *data, int Npixels, int i, RGBPixel *pixels) {
    for (int j = 0; j < Npixels; j++) {
        pixels[j] = data[i * Npixels + j];
    }
}
/*--------------------------*/


/* --------------------------------------- */


void free_pipes(int** pipes, int w) {
    for (int i = 0; i < w; i++) {
        free(pipes[i]);
    }
    free(pipes);
}


void sendImages(BMPImage *Images, int count){
    size_t dataSize = sizeof(BMPImage) * count;
    write(STDOUT_FILENO, Images, dataSize);
    close(STDOUT_FILENO);
    
}

void freeImages(BMPImage *images, int count) {
    for (int i = 0; i < count; i++) {
        free(images[i].data); // Liberar datos de la imagen si fueron asignados dinámicamente
    }
    free(images); // Liberar el arreglo de imágenes
}

void printPixels(RGBPixel *pixels, int N){
    int i;
    for (i = 0; i < N; i++){
        fprintf(stderr,"R: %d, G: %d, B: %d\n", pixels[i].r, pixels[i].g, pixels[i].b);
    }
    fprintf(stderr, "total: %d\n", i+1);
}

void freePixelsArray(RGBPixel ** pixelsArray, int W){
    for (int i = 0; i < W; i++){
        free(pixelsArray[i]);
    }
    free(pixelsArray);
}

/* Todo Bonito */

BMPImage processImage(BMPImage *image, int W, char *argv[]){
    int pixelsPerWorker[2];
    pixels_per_worker(image->height, image->width, W, pixelsPerWorker);

    int totalPixels = image->height * image->width;
    RGBPixel *NewData = (RGBPixel*)malloc(sizeof(RGBPixel) * totalPixels);
    if (NewData == NULL){
        fprintf(stderr, "Error al asignar memoria\n");
        exit(ERROR);
    }
    processPixels(image->data, totalPixels, W, pixelsPerWorker, NewData, argv);

    BMPImage newImage;
    newImage.width = image->width;
    newImage.height = image->height;
    newImage.data = NewData;

    return newImage;
}

void processPixels(RGBPixel *data, int totalPixels, int W, int *pixelsPerWorker, RGBPixel *NewData, char *argv[]){
    if(W == 1){
        CreateWorker(data, totalPixels, argv, 0, NewData);
        return;
    }

    for(int i = 0; i < W; i++){
        if (i == W - 1){
            RGBPixel *SubData = (RGBPixel*)malloc(sizeof(RGBPixel) * pixelsPerWorker[1]);
            CreateWorker(data,pixelsPerWorker[1], argv, i, SubData);
            memcpy(NewData + i * pixelsPerWorker[1], SubData, pixelsPerWorker[1] * sizeof(RGBPixel));
            free(SubData);
        }else{
            RGBPixel *SubData = (RGBPixel*)malloc(sizeof(RGBPixel) * pixelsPerWorker[0]);
            CreateWorker(data,pixelsPerWorker[0], argv, i, SubData);
            memcpy(NewData + i * pixelsPerWorker[0], SubData, pixelsPerWorker[0] * sizeof(RGBPixel));
            free(SubData);
        }
    }
    
}

void CreateWorker(RGBPixel *data, int NumberPixels, char *argv[], int iterator, RGBPixel *NewData){
    int brokerToWorker[2]; // Pipe del broker al worker
    int workerToBroker[2]; // Pipe del worker al broker

    char *argvWorker[] = {"./worker", argv[2], argv[3], argv[4], NULL};


    RGBPixel *SubData = (RGBPixel*)malloc(sizeof(RGBPixel) * NumberPixels);
    if (SubData == NULL){
        fprintf(stderr, "Error al asignar memoria\n");
        exit(ERROR);
    }
    getPixels(data, NumberPixels, iterator, SubData);


    // Crear pipes
    if (pipe(brokerToWorker) == -1 || pipe(workerToBroker) == -1) {
        perror("Error al crear pipes");
        exit(EXIT_FAILURE);
    }


    pid_t pid = fork();
    if (pid == -1) {
        perror("Error al hacer fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Proceso hijo: el worker
        close(brokerToWorker[1]); // Cerrar el extremo de escritura en brokerToWorker
        close(workerToBroker[0]); // Cerrar el extremo de lectura en workerToBroker
        workerProcess(brokerToWorker, workerToBroker, NumberPixels, SubData, NewData, argvWorker);
    } else {
        // Proceso padre: el broker
        close(brokerToWorker[0]); // Cerrar el extremo de lectura en brokerToWorker
        close(workerToBroker[1]); // Cerrar el extremo de escritura en workerToBroker
        brokerProcess(brokerToWorker, workerToBroker, NumberPixels, SubData, NewData, argvWorker);

        // Esperar al proceso hijo
        wait(NULL);
    }

    return;
}

void workerProcess(int *BrokerToWorker, int *WorkerToBroker, int numberPixels ,RGBPixel *data, RGBPixel *NewData, char *argv[]){


    // Duplicar el descriptor de lectura
    dup2(BrokerToWorker[READ], STDIN_FILENO); // Duplicar el descriptor de lectura
    dup2(WorkerToBroker[WRITE], STDOUT_FILENO); // Duplicar el descriptor de escritura

    // Ejecutar el worker
    if (execv("./worker", argv) == -1) {
        fprintf(stderr,"Error al ejecutar worker\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_FAILURE);
}


void brokerProcess(int *BrokerToWorker, int *WorkerToBroker, int numberPixels, RGBPixel *data, RGBPixel *NewData, char *argv[]){


    int PixelsSent = 0;

    size_t numberSent = 100;
    if(numberPixels < 100){
        numberSent = numberPixels;
    }
    RGBPixel *subData = (RGBPixel*)malloc(sizeof(RGBPixel) * numberSent);
    if (subData == NULL){
        fprintf(stderr, "Error al asignar memoria\n");
        exit(EXIT_FAILURE);
    }
    getPixels(data, numberSent, 0, subData);

    RGBPixel Buffer[numberSent];
    /*
    int iterator = 1;
    //Crear ciclo que vaya enviando de a 1000 pixeles
    write(BrokerToWorker[WRITE], &numberSent, sizeof(size_t));

    write(BrokerToWorker[WRITE], subData, numberSent * sizeof(RGBPixel));

    read(WorkerToBroker[READ], Buffer, numberSent * sizeof(RGBPixel));

    numberSent = 0;
    write(BrokerToWorker[WRITE], &numberSent, sizeof(size_t));

    exit(EXIT_FAILURE);
    /*
    while(PixelsSent < numberPixels){
        
        /*----
        if(PixelsSent + numberSent > numberPixels){
            numberSent = numberPixels -PixelsSent;
        }

        getPixels(data, numberSent,PixelsSent, subData);
        /*----
        
        if (PixelsSent + numberSent < numberPixels){
            if(write(BrokerToWorker[WRITE], &numberSent, sizeof(size_t)) == -1){
                fprintf(stderr,"Broker: Error al escribir en el pipe\n");
                exit(EXIT_FAILURE);
            }

            if(write(BrokerToWorker[WRITE], subData, numberSent * sizeof(RGBPixel)) != numberSent * sizeof(RGBPixel)){
                fprintf(stderr,"Broker: Error al escribir en el pipe\n");
                free(subData);
                exit(EXIT_FAILURE);
            }

            if(read(WorkerToBroker[READ], Buffer, numberSent * sizeof(RGBPixel)) != numberSent * sizeof(RGBPixel)){
                fprintf(stderr,"Broker: Error al leer del pipe\n");
                free(subData);
                exit(EXIT_FAILURE);
            }

            putPixels(Buffer, NewData, numberSent, PixelsSent);
            
            La gracia de esta funcion es que pase los pixeles que se modificaron al arreglo mas grande.
            En mayor detalle los pixeles de subData se van a pasar a NewData, pero en la posicion que le corresponde
            sabiendo cuantos pixeles se van a agregar y cuantos ya se han agregado
            


            PixelsSent += numberSent;
        }else{    
            int lastPixels = numberPixels - PixelsSent;
            if(lastPixels <= 0){
                size_t size = 0;
                write(BrokerToWorker[WRITE], &size, sizeof(size_t));
                break;
            }
            RGBPixel LastBuffer[lastPixels];
            getPixels(data, lastPixels, iterator, subData);
            if(write(BrokerToWorker[WRITE], subData, lastPixels * sizeof(RGBPixel)) != lastPixels * sizeof(RGBPixel)){
                fprintf(stderr,"Error al escribir en el pipe\n");
                exit(EXIT_FAILURE);
            }

            if(read(WorkerToBroker[READ], LastBuffer, lastPixels * sizeof(RGBPixel)) != lastPixels * sizeof(RGBPixel)){
                fprintf(stderr,"Error al leer del pipe\n");
                exit(EXIT_FAILURE);
            }
            putPixels(subData, NewData, lastPixels, PixelsSent);


            PixelsSent += lastPixels;
        }
        if(PixelsSent < numberPixels){
            if(numberPixels - PixelsSent > 100){
                getPixels(data, numberSent, iterator, subData); //OJO
            }
        }
        iterator++;
        
    }
    free(subData);*/

    
    //reIdeacion
    size_t PixelsWaiting = numberPixels;
    int iterator = 1;
    while(PixelsWaiting >= 100){
        write(BrokerToWorker[WRITE], &numberSent, sizeof(size_t));
        write(BrokerToWorker[WRITE], subData, numberSent * sizeof(RGBPixel));
        read(WorkerToBroker[READ], Buffer, numberSent * sizeof(RGBPixel));
        putPixels(Buffer, NewData, numberSent, PixelsSent);
        
        PixelsWaiting -= numberSent;
        PixelsSent += (int)numberSent;
        if(PixelsWaiting < 100){
            break;
        }
        getPixels(data, numberSent, iterator, subData);
        iterator++;
    }
    
    write(BrokerToWorker[WRITE], &PixelsWaiting, sizeof(size_t));
    write(BrokerToWorker[WRITE], subData, PixelsWaiting * sizeof(RGBPixel));
    read(WorkerToBroker[READ], Buffer, PixelsWaiting * sizeof(RGBPixel));
    putPixels(Buffer, NewData, PixelsWaiting, PixelsSent);

    numberSent = 0;
    write(BrokerToWorker[WRITE], &numberSent, sizeof(size_t));

    free(subData);
    close(BrokerToWorker[WRITE]); // Cerrar el extremo de escritura en brokerToWorker
    close(WorkerToBroker[0]); // Cerrar el extremo de lectura en workerToBroker
}

void putPixels(RGBPixel *SubData, RGBPixel *data, int numberPixels, int PixelsReady){
    memcpy(data + PixelsReady, SubData, numberPixels * sizeof(RGBPixel));
}

