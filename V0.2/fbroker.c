


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
/*
Idea nueva para el broker
dado a que pipe tiene un limite de 64kb, se puede hacer un arreglo de pixeles y enviarlo a cada worker
por lo tanto se va a trabajar con un worker a la vez, se va a recibir informacion de un worker y luego empieza otro



*/
/* ---- RE-RE-Implementacion ---- */
/*
void CreateWorker(RGBPixel *pixels, char *argv[], int **pipes1, int **pipes2, int pipeNumber, int NumberPixels) {
    pid_t pid = fork();
    if (pid == -1) {
        printf("Error al crear worker\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Proceso hijo
        fprintf(stderr, "Proceso hijo creado con PID: %d\n", getpid());

        close(pipes1[pipeNumber][WRITE]); // Cerrar el extremo de escritura en el broker-to-worker
        close(pipes2[pipeNumber][READ]); // Cerrar el extremo de lectura en el worker-to-broker

        // Duplicar el descriptor de lectura
        dup2(pipes1[pipeNumber][READ], STDIN_FILENO); // Duplicar el descriptor de lectura
        dup2(pipes2[pipeNumber][WRITE], STDOUT_FILENO); // Duplicar el descriptor de escritura

        // Ejecutar el worker
        if (execv("./worker", argv) == -1) {
            printf("Error al ejecutar worker\n");
            exit(EXIT_FAILURE);
        }

        exit(EXIT_FAILURE);

    } else { // Proceso padre
        size_t dataSize = NumberPixels * sizeof(RGBPixel);
        
        close(pipes1[pipeNumber][READ]); // Cerrar el extremo de lectura en el broker-to-worker
        close(pipes2[pipeNumber][WRITE]); // Cerrar el extremo de escritura en el worker-to-broker

        if (write(pipes1[pipeNumber][1], pixels, dataSize) != dataSize) {
            printf("Error al escribir en el pipe\n");
            exit(EXIT_FAILURE);
        }
        close(pipes1[pipeNumber][1]); // Cerrar el extremo de escritura en el broker-to-worker
    }
}
*/

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

void create_sons(int W, int **pipes1, int **pipes2, char *argvWorker[], char *argvLWorker[], RGBPixel *data, int alto, int ancho) {
    int pixelsPerWorker[2];
    pixels_per_worker(alto, ancho,W, pixelsPerWorker);

    for (int i = 0; i < W; i++) {
        if (i == W - 1) {
            CreateWorker(data + i * pixelsPerWorker[1], argvLWorker, pipes1, pipes2, i, pixelsPerWorker[1]);
        } else {
            CreateWorker(data + i * pixelsPerWorker[0], argvWorker, pipes1, pipes2, i, pixelsPerWorker[0]);
        }
    }
}

/* --------------------------------------- */


void free_pipes(int** pipes, int w) {
    for (int i = 0; i < w; i++) {
        free(pipes[i]);
    }
    free(pipes);
}


/*
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
*/
/**
void freeImages(BMPImage *images, int count) {
    for (int i = 0; i < count; i++) {
        free(images[i].data); // Liberar datos de la imagen si fueron asignados dinámicamente
    }
    free(images); // Liberar el arreglo de imágenes
}
*/
BMPImage *formatImage(RGBPixel *data, BMPImage *image){
    BMPImage *newImage = malloc(sizeof(BMPImage));
    newImage->width = image->width;
    newImage->height = image->height;
    newImage->data = malloc(image->width * image->height * sizeof(RGBPixel));
    if (newImage->data == NULL){
        fprintf(stderr, "Error al asignar memoria\n");
        exit(ERROR);
    }
    for(int i = 0; i < image->width * image->height; i++){
        newImage->data[i] = data[i];
    }
    return newImage;
}

BMPImage * addImage(BMPImage *Images, int *count, BMPImage newImage){
    BMPImage *temp = (BMPImage*)realloc(Images, sizeof(BMPImage) * (*count + 1));
    if (temp == NULL){
        fprintf(stderr,"Error al reasignar memoria\n");
        free(Images);
        return NULL;
    }
    Images = temp;
    Images[*count + 1] = newImage;
    *count += 1;
    return Images;
}

void *AddImage(BMPImage *Images[], int count, BMPImage *newImage){
    Images[count] = newImage;
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

/*Es posible que haya que revisar el caso de que el lastPixels sea mas pequeño que Npixels*/
/*
void readAllPixels(int **pipes, int W, RGBPixel **pixelsArray , int totalPixels, int Npixels, int lastPixels){
    RGBPixel pixelsArrayRead[Npixels];
    if (W == 1){
        if(read(pipes[0][READ], pixelsArrayRead, totalPixels * sizeof(RGBPixel)) != totalPixels * sizeof(RGBPixel)){
            fprintf(stderr, "Error al leer los pixeles\n");
            exit(ERROR);
        }
        memcpy(pixelsArray[0], pixelsArrayRead, totalPixels * sizeof(RGBPixel));//Esto esta dudoso
    }


    for (int i = 0; i < W-1; i++){

        size_t  dataSize = Npixels * sizeof(RGBPixel);
        if (read(pipes[i][READ], pixelsArrayRead, dataSize) != dataSize){
            fprintf(stderr, "Error al leer los pixeles\n");
            exit(ERROR);
        }
        memcpy(pixelsArray[i], pixelsArrayRead, dataSize);
    }
    size_t lastDataSize = lastPixels * sizeof(RGBPixel);

    if (read(pipes[W-1][READ], pixelsArrayRead, lastDataSize) != lastDataSize){
        fprintf(stderr, "ERROR: Last worker\n\n");
        fprintf(stderr, "Error al leer los pixeles\n");
        exit(ERROR);
    }
    memcpy(pixelsArray[W-1], pixelsArrayRead, lastDataSize);
}
*/

/*En teoria esto esta bien, pero no funciona quizas es el worker?
void readAllPixels(int **pipes1, int W, RGBPixel **pixelsArray, int totalPixels, int pixelsPerWorker, int lastWorkerPixels) {
    
    for (int i = 0; i < W; i++) {
        int numPixels;
        if (i == W - 1){
            numPixels = lastWorkerPixels;
        }else{
            numPixels = pixelsPerWorker;
        }
        RGBPixel pixelsArrayRead[numPixels];
        size_t dataSize = numPixels * sizeof(RGBPixel);

        if (read(pipes1[i][0], pixelsArrayRead, dataSize) != dataSize) {
            printf("Error al leer del pipe\n");
            exit(EXIT_FAILURE);
        }
        close(pipes1[i][0]); // Cerrar el extremo de lectura después de leer
        memcpy(pixelsArray[i], pixelsArrayRead, dataSize);
    }
}*/

void readAllPixels(int **pipes, int W, RGBPixel **pixelsArray, int totalPixels, int Npixels, int lastPixels) {
    RGBPixel *pixelsArrayRead = malloc(Npixels * sizeof(RGBPixel));
    if (pixelsArrayRead == NULL) {
        fprintf(stderr, "Error al asignar memoria\n");
        exit(ERROR);
    }
    if (W == 1)
    {
        if (read(pipes[0][READ], pixelsArrayRead, totalPixels * sizeof(RGBPixel)) != totalPixels * sizeof(RGBPixel))
        {
            fprintf(stderr, "Broker: Error al leer los píxeles\n");
            free(pixelsArrayRead);
            exit(ERROR);
        }
        memcpy(pixelsArray[0], pixelsArrayRead, totalPixels * sizeof(RGBPixel));
        //Imprimir pixels
        printPixels(pixelsArray[0], totalPixels);
        return;
    }
    int k = 0;
    for (int j = 0; j < W; j++) {

        if(j == W - 1){
            if (read(pipes[j][READ], pixelsArrayRead, lastPixels * sizeof(RGBPixel)) != lastPixels * sizeof(RGBPixel)) {
                fprintf(stderr, "Broker: Error al leer los píxeles\n");
                free(pixelsArrayRead);
                exit(ERROR);
            }
            memcpy(pixelsArray[j], pixelsArrayRead, lastPixels * sizeof(RGBPixel));
            //Imprimir pixels
            //printPixels(pixelsArray[j], lastPixels);
        }else{
            if (read(pipes[j][READ], pixelsArrayRead, Npixels * sizeof(RGBPixel)) != Npixels * sizeof(RGBPixel)) {
                fprintf(stderr, "Broker: Error al leer los píxeles\n");
                free(pixelsArrayRead);
                exit(ERROR);
            }
            memcpy(pixelsArray[j], pixelsArrayRead, Npixels * sizeof(RGBPixel));
            //fprintf(stderr, "Worker %d\n", j);
            //fprintf(stderr, "Npixels: %d\n", Npixels);
            //printPixels(pixelsArray[j], Npixels);
            
        }
        //fprintf(stderr, "k:%d\n",k);
        k++;
    }
    //free(pixelsArrayRead);
}


void AllPixelsToOne(RGBPixel ** pixelsArray, int W, RGBPixel *NewData ,int Npixels, int lastPixels, int totalPixels){
    /*fprintf(stderr, "Pixeles entrantes:");
    fprintf(stderr, "Npixels: %d\n", Npixels);
    fprintf(stderr, "W: %d\n", W);
    for (int j = 0; j < W; j++){
        if(j == W - 1){
            printPixels(pixelsArray[j], lastPixels);
        }else{
            printPixels(pixelsArray[j], Npixels);
        }
        fprintf(stderr, "Worker %d\n", j);
    }
    */
    if(W == 1){
        memcpy(NewData, pixelsArray[0], totalPixels * sizeof(RGBPixel));
        return;
    }

    for (int i = 0; i < W; i++){
        if(i == W - 1){
            memcpy(NewData + i * Npixels, pixelsArray[i], lastPixels * sizeof(RGBPixel));
        }else{
            memcpy(NewData + i * Npixels, pixelsArray[i], Npixels * sizeof(RGBPixel));
        }
    }

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

BMPImage *processImage(BMPImage *image, int W, char *argv[]){
    int pixelsPerWorker[2];
    pixels_per_worker(image->height, image->width, W, pixelsPerWorker);

    int totalPixels = image->height * image->width;
    RGBPixel *NewData = (RGBPixel*)malloc(sizeof(RGBPixel) * image->height * image->width);
    if (NewData == NULL){
        fprintf(stderr, "Error al asignar memoria\n");
        exit(ERROR);
    }
    processPixels(image->data, totalPixels, W, pixelsPerWorker, NewData, argv);

    BMPImage newImage;
    newImage.width = image->width;
    newImage.height = image->height;
    newImage.data = NewData;

    return &newImage;
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
        }else{
            RGBPixel *SubData = (RGBPixel*)malloc(sizeof(RGBPixel) * pixelsPerWorker[0]);
            CreateWorker(data,pixelsPerWorker[0], argv, i, SubData);
            memcpy(NewData + i * pixelsPerWorker[0], SubData, pixelsPerWorker[0] * sizeof(RGBPixel));
        }
    }
}

void CreateWorker(RGBPixel *data, int NumberPixels, char *argv[], int iterator, RGBPixel *NewData){
    int brokerToWorker[2]; // Pipe del broker al worker
    int workerToBroker[2]; // Pipe del worker al broker

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
        workerProcess(brokerToWorker[0], workerToBroker[1]);
    } else {
        // Proceso padre: el broker
        close(brokerToWorker[0]); // Cerrar el extremo de lectura en brokerToWorker
        close(workerToBroker[1]); // Cerrar el extremo de escritura en workerToBroker
        brokerProcess(brokerToWorker[1], workerToBroker[0]);

        // Esperar al proceso hijo
        wait(NULL);
    }

    return 0;
}


