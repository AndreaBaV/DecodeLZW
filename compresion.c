#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DICT_SIZE 4096
#define MAX_SEQ_LENGTH 256

typedef struct Diccionario {
    char secuencia[MAX_SEQ_LENGTH];
    int codigo;
    struct Diccionario *siguiente;
} Diccionario;

unsigned int funcion_hash(const char *secuencia) {
    unsigned int hash = 0;
    while (*secuencia) {
        hash = (hash * 31 + *secuencia++) % DICT_SIZE;
    }
    return hash;
}

void inicializar_diccionario(Diccionario *tabla_hash[]) {
    for (int i = 0; i < DICT_SIZE; i++) {
        tabla_hash[i] = NULL;
    }
}

void agregar_entrada(Diccionario *tabla_hash[], const char *secuencia, int codigo) {
    unsigned int index = funcion_hash(secuencia);
    Diccionario *nuevo = (Diccionario *)malloc(sizeof(Diccionario));
    strncpy(nuevo->secuencia, secuencia, MAX_SEQ_LENGTH);
    nuevo->codigo = codigo;
    nuevo->siguiente = tabla_hash[index];
    tabla_hash[index] = nuevo;
}

int buscar_codigo(Diccionario *tabla_hash[], const char *secuencia) {
    unsigned int index = funcion_hash(secuencia);
    Diccionario *entrada = tabla_hash[index];
    while (entrada != NULL) {
        if (strcmp(entrada->secuencia, secuencia) == 0) {
            return entrada->codigo;
        }
        entrada = entrada->siguiente;
    }
    return -1;
}

// Escribe un código de 12 bits en el archivo usando un buffer de bits
void escribir_codigo(FILE *salida, unsigned int codigo, int *bit_buffer, int *bit_count) {
    // Agregar los 12 bits al buffer
    *bit_buffer |= (codigo << *bit_count);
    *bit_count += 12;

    // Escribir bytes completos desde el buffer
    while (*bit_count >= 8) {
        fputc(*bit_buffer & 0xFF, salida);  // Escribir el byte menos significativo
        *bit_buffer >>= 8;  // Desplazar el buffer
        *bit_count -= 8;  // Reducir el conteo de bits
    }
}

// Vacía los bits restantes en el buffer al archivo
void vaciar_buffer(FILE *salida, int *bit_buffer, int *bit_count) {
    if (*bit_count > 0) {  // Si quedan bits en el buffer
        fputc(*bit_buffer & 0xFF, salida);  // Escribir el último byte
    }
}

void comprimir_archivo(const char *entrada_path, const char *salida_path) {
    FILE *entrada = fopen(entrada_path, "r");
    FILE *salida = fopen(salida_path, "wb");
    if (!entrada || !salida) {
        perror("Error al abrir archivo");
        exit(EXIT_FAILURE);
    }

    Diccionario *tabla_hash[DICT_SIZE];
    inicializar_diccionario(tabla_hash);

    // Inicializar diccionario con caracteres individuales
    for (int i = 0; i < 256; i++) {
        char secuencia[2] = { i, '\0' };
        agregar_entrada(tabla_hash, secuencia, i);
    }

    int codigo_siguiente = 256;
    char buffer[MAX_SEQ_LENGTH];
    int longitud_buffer = 0;
    int c;

    // Variables para la gestión de bits
    int bit_buffer = 0;  // Buffer temporal para bits incompletos
    int bit_count = 0;   // Conteo de bits en el buffer

    // Compresión
    while ((c = fgetc(entrada)) != EOF) {
        buffer[longitud_buffer++] = c;
        buffer[longitud_buffer] = '\0';

        if (buscar_codigo(tabla_hash, buffer) == -1) {
            buffer[longitud_buffer - 1] = '\0';
            int codigo = buscar_codigo(tabla_hash, buffer);

            // Escribir el código de 12 bits
            escribir_codigo(salida, codigo, &bit_buffer, &bit_count);

            if (codigo_siguiente < DICT_SIZE) {
                buffer[longitud_buffer - 1] = c;
                buffer[longitud_buffer] = '\0';
                agregar_entrada(tabla_hash, buffer, codigo_siguiente++);
            }

            longitud_buffer = 1;
            buffer[0] = c;
        }
    }

    // Escribir el último código si queda algo en el buffer
    if (longitud_buffer > 0) {
        buffer[longitud_buffer] = '\0';
        int codigo = buscar_codigo(tabla_hash, buffer);
        escribir_codigo(salida, codigo, &bit_buffer, &bit_count);
    }

    // Vaciar el buffer de bits restante
    vaciar_buffer(salida, &bit_buffer, &bit_count);

    fclose(entrada);
    fclose(salida);
}

int main() {
    comprimir_archivo("entrada.txt", "comprimido.lzw");
    return 0;
}
