#include <stdint.h>

typedef struct {
    uint32_t size; // Tamaño de la información de la imagen, tiene 4 bytes y es un número que indica el tamaño de la información de la imagen en bytes
    int32_t width; // Ancho de la imagen, tiene 4 bytes y es un número que indica el ancho de la imagen en píxeles
    int32_t height; // Alto de la imagen, tiene 4 bytes y es un número que indica el alto de la imagen en píxeles
    uint16_t planes; // Planos, tiene 2 bytes y es un número que indica la cantidad de planos de color, debe valer 1
    uint16_t bit_count; // Bits por píxel, tiene 2 bytes y es un número que indica la cantidad de bits por píxel, debe valer 1, 4, 8, 16, 24, or 32
    uint32_t compression; // Compresión, tiene 4 bytes y es un número que indica el tipo de compresión, puede ser 0 (sin compresión), 1 (RLE 8-bit/píxel), 2 (RLE 4-bit/píxel), 3 (bitfields), 4 (JPEG), 5 (PNG)
    uint32_t size_image; // Tamaño de la imagen, tiene 4 bytes y es un número que indica el tamaño de la imagen en bytes
    int32_t x_pixels_per_meter; // Píxeles por metro en el eje X, tiene 4 bytes y es un número que indica la cantidad de píxeles por metro en el eje X
    int32_t y_pixels_per_meter; // Píxeles por metro en el eje Y, tiene 4 bytes y es un número que indica la cantidad de píxeles por metro en el eje Y
    uint32_t colors_used; // Colores usados, tiene 4 bytes y es un número que indica la cantidad de colores usados
    uint32_t colors_important; // Colores importantes, tiene 4 bytes y es un número que indica la cantidad de colores importantes
} BMPInfoHeader;