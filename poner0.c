//Este archivo es para generar casos de prueba del -ignorar0.c-
//Agrega 0000 al principio del cósigo ASCII del caracter 
// Ej: 000011011100

#include <stdio.h>
#include <stdlib.h>

void write12BitBinary(FILE *outputFile, unsigned short *bitBuffer, int *bitCount, unsigned short value) {
    *bitBuffer = (*bitBuffer << 12) | (value & 0xFFF);  // Agregar los 12 bits del valor al buffer
    *bitCount += 12;

    // Mientras tengamos al menos 8 bits en el buffer, escribimos un byte al archivo
    while (*bitCount >= 8) {
        unsigned char byteToWrite = (*bitBuffer >> (*bitCount - 8)) & 0xFF;  // Obtener los 8 bits más significativos
        fwrite(&byteToWrite, sizeof(unsigned char), 1, outputFile);          // Escribir 1 byte al archivo
        *bitCount -= 8;  // Reducir el contador de bits
    }

    // Limpiar los bits que ya se han escrito
    *bitBuffer &= (1 << *bitCount) - 1;  // Mantener solo los bits restantes
}

void flushRemainingBits(FILE *outputFile, unsigned short bitBuffer, int bitCount) {
    // Si quedan bits no escritos en el buffer, los escribimos como un byte final
    if (bitCount > 0) {
        unsigned char byteToWrite = (bitBuffer << (8 - bitCount)) & 0xFF;  // Alinear los bits restantes a 8 bits
        fwrite(&byteToWrite, sizeof(unsigned char), 1, outputFile);        // Escribir el byte restante
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <archivo_entrada> <archivo_salida.lzw>\n", argv[0]);
        return 1;
    }

    FILE *inputFile = fopen(argv[1], "r");
    FILE *outputFile = fopen(argv[2], "wb");

    if (inputFile == NULL || outputFile == NULL) {
        fprintf(stderr, "Error al abrir los archivos.\n");
        return 1;
    }

    unsigned short bitBuffer = 0;  // Buffer para acumular bits
    int bitCount = 0;              // Contador de bits en el buffer

    char ch;
    while ((ch = fgetc(inputFile)) != EOF) {
        write12BitBinary(outputFile, &bitBuffer, &bitCount, (unsigned short)(unsigned char)ch);
    }

    flushRemainingBits(outputFile, bitBuffer, bitCount);

    fclose(inputFile);
    fclose(outputFile);

    printf("Archivo binario generado correctamente: %s\n", argv[2]);
    return 0;
}  
