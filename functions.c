#include "functions.h"

/*
Entrada: Nombre del archivo a leer (archivo bpm)
Salida: imagen tipo BMPImage
Descripcion: Se lee un archivo tipo bmp, se define el ancho, el alto y los pixeles de la imagen
y se guardan estos datos en las respectivas variables para luego retornar la variable image de tipo BMPImage
*/
BMPImage* read_bmp(const char* filename) {
    FILE* file = fopen(filename, "rb"); //rb = read binary
    if (!file) {
        return NULL;
    }

    BMPHeader header;
    fread(&header, sizeof(BMPHeader), 1, file);
    if (header.type != 0x4D42) { // 42 = 66 en decimal = B en ASCII y 4D = 77 en decimal = M en ASCII
        fprintf(stderr, "Error: El archivo no es un BMP válido.\n");
        fclose(file);
        return NULL;
    }

    BMPInfoHeader info_header;
    fread(&info_header, sizeof(BMPInfoHeader), 1, file); //fread(puntero a la estructura, tamaño de la estructura, cantidad de estructuras, archivo)

    BMPImage* image = (BMPImage*)malloc(sizeof(BMPImage));
    image->width = info_header.width;
    image->height = info_header.height;
    image->data = (RGBPixel*)malloc(sizeof(RGBPixel) * info_header.width * info_header.height);

    fseek(file, header.offset, SEEK_SET); // fseek(archivo, desplazamiento, origen desde donde se desplaza SEEK_SET = inicio del archivo, SEEK_CUR = posición actual del archivo, SEEK_END = final del archivo)

    //se hace padding para que la imagen tenga un tamaño múltiplo de 4, esto se hace para que la imagen sea más rápida de leer
    int padding = (4 - (info_header.width * sizeof(RGBPixel)) % 4) % 4; // primero se pasan a bytes los píxeles de la imagen y se calcula el residuo de la división entre 4, si el residuo es 0 no hay padding, si el residuo es 1, 2 o 3 se calcula el padding
    for (int y = info_header.height - 1; y >= 0; y--) {
        for (int x = 0; x < info_header.width; x++) {
            RGBPixel pixel;
            fread(&pixel, sizeof(RGBPixel), 1, file);
            image->data[y * info_header.width + x] = pixel;
        }
        fseek(file, padding, SEEK_CUR);
    }

    fclose(file);
    return image;
}

/*
Entrada: imagen tipo BMPImage
Salida: void
Descripcion: Se libera memoria de una imagen
*/
void free_bmp(BMPImage* image) {
    if (image) {
        free(image->data);
        free(image);
    }
}

/*
Entrada: Nombre de archivo, imagen tipo BMPImage
Salida: Imagen creada en carpeta
Descripcion: Se utiliza la informacion de una imagen para escribirla en la carpeta
donde se encuentra el programa, el nombre de la imagen corresponde a la variable filename
*/
void write_bmp(const char* filename, BMPImage* image) {
    FILE* file = fopen(filename, "wb"); //wb = write binary
    if (!file) {
        fprintf(stderr, "Error: No se pudo abrir el archivo.\n");
        return;
    }

    BMPHeader header;
    header.type = 0x4D42;
    header.size = sizeof(BMPHeader) + sizeof(BMPInfoHeader) + image->width * image->height * sizeof(RGBPixel);
    header.offset = sizeof(BMPHeader) + sizeof(BMPInfoHeader);

    BMPInfoHeader info_header;
    info_header.size = sizeof(BMPInfoHeader);
    info_header.width = image->width;
    info_header.height = image->height;
    info_header.planes = 1;
    info_header.bit_count = 24; // está fijado en 24 en este ejemplo pero puede ser 1, 4, 8, 16, 24 o 32
    info_header.size_image = image->width * image->height * sizeof(RGBPixel);

    fwrite(&header, sizeof(BMPHeader), 1, file);
    fwrite(&info_header, sizeof(BMPInfoHeader), 1, file);

    int padding = (4 - (image->width * sizeof(RGBPixel)) % 4) % 4;
    for (int y = image->height - 1; y >= 0; y--) {
        for (int x = 0; x < image->width; x++) {
            RGBPixel pixel = image->data[y * image->width + x];
            fwrite(&pixel, sizeof(RGBPixel), 1, file);
        }

        RGBPixel padding_pixel = {0};
        fwrite(&padding_pixel, sizeof(RGBPixel), padding, file);
    }

    fclose(file);
}



/*
Entrada: imagen tipo BMPImage y un umbral (factor) de clasificacion
Salida: entero, 1 si es una imagen calificada como nearly black, 0 en caso contrario
Descripcion: se clasifica una imagen usando un umbral (o factor) como referencia, si el pixel tiene
un valor  mayor a este, se aumenta cont1, es decir, se haya pixel blanco, por el contrario, se asigna 0, indicando
un pixel negro
*/
int nearly_black(BMPImage* image, float factor){
    //cont 1 para contar pixeles blancos, cont2 para contar pixeles negros
    int cont1 = 0, cont2 = 0;
    int umbral = (int)fabs(255 * factor);

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            RGBPixel pixel = image->data[y * image->width + x];

            //Verificar si el pixel pasa el umbral
            if ((pixel.r > umbral && pixel.g > umbral) && pixel.b > umbral) {
                cont1++;

            } else{
                cont2++;
            }
        }
    }

    if (cont1 < cont2) {
        return 1; //Imagen es nearly black
    }

    return 0; //Imagen no es nearly black
}

BMPImage * receiveImages(int pipe[2], int numberImages){
    BMPImage * Images = (BMPImage*)malloc(sizeof(BMPImage)*numberImages);
    size_t dataSize = sizeof(BMPImage) * numberImages;

    read(pipe[READ],Images, dataSize);
}

int CountImages(char* prefix){
    char resultado[200]; // Se mantiene la declaración de resultado
    char signo = '_';
    char bmp[20] = ".bmp";
    resultado[0] = '\0';
    int counter = 0;
    int loop = 1;
    int bandera = -1;

    while(bandera == -1){
        // Concatenar prefijo (N) y símbolo _
        snprintf(resultado, sizeof(resultado), "%s%c", prefix, signo);

        // Concatenar el número de loop con resultado
        snprintf(resultado + strlen(resultado), sizeof(resultado) - strlen(resultado), "%d", loop);

        // Concatenar la extension del archivo
        snprintf(resultado + strlen(resultado), sizeof(resultado) - strlen(resultado), "%s", bmp);

        FILE* file = fopen(resultado, "rb");
        if(file == NULL){
            bandera = 1;
        } else {
            counter++;
            loop++;
        }
    }

    return counter;
}