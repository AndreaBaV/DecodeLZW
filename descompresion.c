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

const char* buscar_secuencia(Diccionario *tabla_hash[], int codigo) {
    for (int i = 0; i < DICT_SIZE; i++) {
        Diccionario *entrada = tabla_hash[i];
        while (entrada != NULL) {
            if (entrada->codigo == codigo) {
                return entrada->secuencia;
            }
            entrada = entrada->siguiente;
        }
    }
    return NULL;
}

void descomprimir_archivo(const char *entrada_path, const char *salida_path) {
    FILE *entrada = fopen(entrada_path, "rb");
    FILE *salida = fopen(salida_path, "w");
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
    int anterior_codigo = -1;
    int codigo;
    while (fread(&codigo, sizeof(int), 1, entrada) == 1) {
        const char *secuencia;
        if (codigo < 256) {
            secuencia = (char[]){(char)codigo, '\0'};
        } else {
            secuencia = buscar_secuencia(tabla_hash, codigo);
        }

        if (secuencia) {
            fprintf(salida, "%s", secuencia);
            if (anterior_codigo != -1) {
                const char *anterior_secuencia = buscar_secuencia(tabla_hash, anterior_codigo);
                if (anterior_secuencia) {
                    char nueva_secuencia[MAX_SEQ_LENGTH];
                    snprintf(nueva_secuencia, sizeof(nueva_secuencia), "%s%c", anterior_secuencia, secuencia[0]);
                    if (codigo_siguiente < DICT_SIZE) {
                        agregar_entrada(tabla_hash, nueva_secuencia, codigo_siguiente++);
                    }
                }
            }
        }
        anterior_codigo = codigo;
    }

    fclose(entrada);
    fclose(salida);
}

int main() {
    descomprimir_archivo("comprimido.lzw", "descomprimido.txt");
    return 0;
}
