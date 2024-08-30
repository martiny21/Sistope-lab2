#include <stdint.h>
#pragma pack(push, 1)

typedef struct {
    // BMP Header | Tipo de dato por bits | Descripción
    uint16_t type; // Tipo de dato, tiene 2 bytes y es un número que indica si el archivo es BMP a través de las siglas BM
    uint32_t size; // Tamaño del archivo, tiene 4 bytes y es un número que indica el tamaño del archivo en bytes
    uint16_t reserved1; // Reservado, tiene 2 bytes y es un número que no se utiliza
    uint16_t reserved2; // Reservado, tiene 2 bytes y es un número que no se utiliza
    uint32_t offset; // Offset, tiene 4 bytes y es un número que indica la posición en bytes donde comienza la información de la imagen
} BMPHeader;