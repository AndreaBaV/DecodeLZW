//Este archivo ignora los primeros 4 bits si son 0

#include <stdio.h>
#include <stdlib.h>

unsigned short read12BitBinary(FILE *inputFile, unsigned short *bitBuffer, int *bitCount) {
    unsigned short value = 0;

    while (*bitCount < 12) {
        unsigned char byteRead;
        if (fread(&byteRead, sizeof(unsigned char), 1, inputFile) == 1) {
            *bitBuffer = (*bitBuffer << 8) | byteRead;  // Añadir el byte al buffer
            *bitCount += 8;                             // Incrementar el contador de bits
        } else {
            // Si no hay más bytes que leer, retornamos 0
            return 0xFFFF;  // Indicamos fin de archivo o error
        }
    }

    // Extraer los 12 bits más significativos del buffer
    value = (*bitBuffer >> (*bitCount - 12)) & 0xFFF;
    *bitCount -= 12;  // Reducimos el contador de bits

    // Limpiar 
    *bitBuffer &= (1 << *bitCount) - 1;

    return value;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <archivo_entrada.lzw> <archivo_salida>\n", argv[0]);
        return 1;
    }

    FILE *inputFile = fopen(argv[1], "rb");
    FILE *outputFile = fopen(argv[2], "w");

    if (inputFile == NULL || outputFile == NULL) {
        fprintf(stderr, "Error al abrir los archivos.\n");
        return 1;
    }

    unsigned short bitBuffer = 0;  // Buffer para acumular bits
    int bitCount = 0;              // Contador de bits en el buffer

    // Leemos y decodificamos los valores de 12 bits hasta el final del archivo
    unsigned short value;
    while ((value = read12BitBinary(inputFile, &bitBuffer, &bitCount)) != 0xFFFF) {
        unsigned char ch = (unsigned char)(value & 0xFF);  // Solo los 8 bits inferiores
        fputc(ch, outputFile);                             // Carácter original al archivo de salida
    }

    fclose(inputFile);
    fclose(outputFile);

    printf("Archivo descomprimido correctamente: %s\n", argv[2]);
    return 0;
}
