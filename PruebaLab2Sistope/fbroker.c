#include "fbroker.h"

/*
Descripcion: Funcion que calcula la cantidad de pixeles para cada worker basado en en M % W
    - Pixels[0] = Cantidad de pixeles que le corresponden a cada worker a excepcion del ultimo
    - Pixels[1] = Cantidad de pixeles que le corresponden al ultimo worker
Entrada: Alto de la imagen, cantidad de workers, arreglo de numeros de pixeles
Salida: N/A
*/
void pixels_per_worker(int alto, int ancho, int W, int *pixels){ 
    int Workers = ancho / W; //Cantidad de pixeles que corresponden a cada worker
    int UltimoWorker = ancho % W;  // Columnas restantes que deben ser asignadas al último worker

    pixels[0] = Workers * alto;  // Cantidad de píxeles para todos los workers menos el último
    pixels[1] = (Workers + UltimoWorker) * alto;  // Píxeles para el último worker que recibe las columnas restantes
}



/*
Entrada: arreglo de pixeles, cantidad de pixeles, iterador, arreglo de pixeles nuevo
Salida: no hay
Descripcion: funcion que traspasa ciertos pixeles a un arreglo
*/
void getPixels(RGBPixel *data, int Npixels, int i, RGBPixel *pixels) {
    for (int j = 0; j < Npixels; j++) {
        pixels[j] = data[i * Npixels + j];
    }
}

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
        exit(EXIT_FAILURE);
    }
    processPixels(image->data, totalPixels, W, pixelsPerWorker, NewData, argv);

    BMPImage newImage;
    newImage.width = image->width;
    newImage.height = image->height;
    newImage.data = NewData;

    return newImage;
}

/*
Entrada: arreglo de pixeles, cantidad de pixeles, cantidad de workers, arreglo de cantidad de pixeles
por worker, arreglo de pixeles nuevo, argumentos para el worker
Salida: no hay
Descripcion: Cada worker recibe la cantidad de pixeles que le corresponde y los procesa
*/
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
        exit(EXIT_FAILURE);
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
        free(SubData);
        exit(EXIT_SUCCESS);
    } else {
        // Proceso padre: el broker
        close(brokerToWorker[0]); // Cerrar el extremo de lectura en brokerToWorker
        close(workerToBroker[1]); // Cerrar el extremo de escritura en workerToBroker
        brokerProcess(brokerToWorker, workerToBroker, NumberPixels, SubData, NewData);

        // Esperar al proceso hijo
        wait(NULL);
        free(SubData);
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
void brokerProcess(int *BrokerToWorker, int *WorkerToBroker, int numberPixels, RGBPixel *data, RGBPixel *NewData){
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
Entrada: nombre de la imagen, cantidad de workers, cantidad de imagenes
Salida: caracter con string concatenado
Descripcion: Se mueven las imagenes a un directorio
*/
char* ConcatenateStringWithInt(const char* base, int i) {
    // Calcular el tamaño necesario para el string final
    // + 5 para ".bmp" + 10 para el entero + 1 para el terminador nulo '\0'
    size_t length = strlen(base) + 5 + 10 + 1;
    char* result = (char*)malloc(length * sizeof(char));
    
    if (result == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria\n");
        exit(EXIT_FAILURE);
    }

    // Crear el string final concatenado
    snprintf(result, length, "%s%d.bmp", base, i);
    
    return result;
}

/*
Entrada: nombre de la imagen, nombre del directorio
Salida: no hay
Descripcion: Se mueve la imagen a un directorio especifico, el nombre del directorio
es el especificado en la linea de comandos
*/
void moveFileToDirectory(const char* filename, const char* directory) {
    // Crear un string para la nueva ruta del archivo
    size_t newPathLength = strlen(directory) + strlen(filename) + 2; // +1 para '/' y +1 para '\0'
    char* newPath = (char*)malloc(newPathLength);
    
    if (newPath == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria para la nueva ruta\n");
        exit(EXIT_FAILURE);
    }
    
    // Crear la nueva ruta en formato "directorio/archivo"
    snprintf(newPath, newPathLength, "%s/%s", directory, filename);
    
    // Mover el archivo con rename
    if (rename(filename, newPath) == 0) {} else {
        // Mostrar el error en caso de fallo
        fprintf(stderr,"Error al mover el archivo");
    }

    // Liberar memoria asignada para newPath
    free(newPath);
}