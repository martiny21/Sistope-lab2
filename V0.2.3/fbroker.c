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
/*
Descripción: Se procesa una imagen calculando la cantidad de pixeles que
le corresponden a cada worker, para que estos procesen los pixeles y
retornar la nueva imagen
Entrada: imagen tipo bmp, cantidad de workers, argv
Salida: Imagen bmp procesado
*/
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

/*
Entrada: arreglo de pixeles, cantidad de pixeles, argumentos para el worker, iterador, arreglo de pixeles nuevo
Salida: no hay
Descripcion: Crea un worker, con sus pipes, y le envia los pixeles que le corresponden
*/
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

/*
Entrada: pipe del broker al worker, pipe del worker al broker, cantidad de pixeles, arreglo de pixeles,
arreglo de pixeles nuevo, argumentos para el worker
Salida: no hay
Descripcion: Se duplican los descriptores de archivo y se ejecuta el worker
*/
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

/*
Entrada: pipe del broker al worker, pipe del worker al broker, cantidad de pixeles, arreglo de pixeles,
arreglo de pixeles nuevo, argumentos para el worker
Salida: no hay
Descripcion: Se envian de a 100 pixeles al worker y luego se reciben los pixeles ya procesados
*/
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

/*
Entrada: sub-arreglo de pixeles, arreglo de pixeles, cantidad de pixeles, pixeles listos
Salida: no hay
Descripcion: Se copian los pixeles de un arreglo a otro
*/
void putPixels(RGBPixel *SubData, RGBPixel *data, int numberPixels, int PixelsReady){
    memcpy(data + PixelsReady, SubData, numberPixels * sizeof(RGBPixel));
}


/*
void sendImages(BMPImage images[], int *pipe, int numImages) {
    for(int i = 0; i < numImages; i++) {
        //Enviar ancho y alto de una imagen
        write(STDOUT_FILENO, &images[i].width, sizeof(int));
        write(STDOUT_FILENO, &images[i].height, sizeof(int));

        int TotalPixels = images[i].width * images[i].height;
        int j = 0;

        //Enviar de a 100 pixeles
        while(j < TotalPixels) {
            int PixelsToSend = 100;
            if(j + 100 > TotalPixels) {
                PixelsToSend = TotalPixels - j;
            }
            write(STDOUT_FILENO, &images[i].data[j], PixelsToSend * sizeof(RGBPixel));
            j+= 100;
        }
    }
    //close(pipe[1]);
}


void sendImages2(BMPImage images[], int numImages){
    for (int i = 0; i < numImages; i++) {
        //BMPImage* img = &images[i];

        ssize_t bytes = write(STDOUT_FILENO, &images[i], sizeof(images[i]));

    }
}
*/

/*


void SendImages(BMPImage *Images, int *fd, int *fd2, int N) {
    for (int i = 0; i < N; i++) {
        BMPImage img = Images[i];
        int width = img.width;
        int height = img.height;

        // Enviar ancho y alto de la imagen
        write(fd[WRITE], &width, sizeof(int));
        write(fd[WRITE], &height, sizeof(int));

        int TotalPixels = width * height;
        int PixelsSent = 0;

        // Enviar de a 100 píxeles
        while (PixelsSent < TotalPixels)
        {
            size_t numPixels = 100;
            if (TotalPixels - PixelsSent < 100)
            {
                numPixels = TotalPixels - PixelsSent;
            }

            // Enviar cantidad de píxeles a enviar
            write(STDOUT_FILENO, &numPixels, sizeof(size_t));

            // Enviar los píxeles
            write(STDOUT_FILENO, img.data + PixelsSent, numPixels * sizeof(RGBPixel));

            PixelsSent += numPixels;

            // Esperar
            int done;
            read(fd2[READ], &done, sizeof(int));
        }

        // Enviar un 0 para indicar que se terminó de enviar la imagen
        size_t zero = 0;
        write(STDOUT_FILENO, &zero, sizeof(size_t));
    }
}
*/
void SendImages(BMPImage *Images, int *fd, int *fd2, int N)
{
    for (int i = 0; i < N; i++)
    {
        BMPImage img = Images[i];
        int width = img.width;
        int height = img.height;

        // Enviar el ancho y el alto de la imagen
        write(fd[WRITE], &width, sizeof(int));
        write(fd[WRITE], &height, sizeof(int));

        int TotalPixels = width * height;
        int PixelsSent = 0;

        // Enviar de a 100 píxeles
        while (1)
        {
            size_t numPixels = 100;
            if (TotalPixels - PixelsSent < 100)
            {
                numPixels = TotalPixels - PixelsSent;
            }

            // Enviar la cantidad de píxeles
            write(STDOUT_FILENO, &numPixels, sizeof(size_t));

            // Enviar los píxeles
            write(STDOUT_FILENO, img.data + PixelsSent, numPixels * sizeof(RGBPixel));

            PixelsSent += numPixels;

            // Esperar
            int done;
            read(fd2[READ], &done, sizeof(int));
        }

    }
}