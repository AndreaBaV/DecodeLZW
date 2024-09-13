// Ejemplo 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DICT_SIZE 4096        // Tamaño del diccionario (12 bits)
#define MAX_SEQ_LENGTH 256   // Longitud máxima de una secuencia

// Estructura para representar una entrada en el diccionario
typedef struct Diccionario {
    char secuencia[MAX_SEQ_LENGTH];  // Clave (secuencia de caracteres)
    int codigo;                      // Valor (código correspondiente)
    struct Diccionario *siguiente;   // Para manejar colisiones con encadenamiento
} Diccionario;

// Función hash simple para calcular el índice del diccionario
unsigned int funcion_hash(const char *secuencia) {
    unsigned int hash = 0;
    while (*secuencia) {
        hash = (hash << 5) + *secuencia++;  // Hash por desplazamiento y suma
    }
    return hash % DICT_SIZE;
}

// Inicializa el diccionario
void inicializar_diccionario(Diccionario *tabla_hash[]) {
    for (int i = 0; i < DICT_SIZE; i++) {
        tabla_hash[i] = NULL;
    }
}

// Inserta una secuencia en el diccionario
void insertar(Diccionario *tabla_hash[], const char *secuencia, int codigo) {
    unsigned int index = funcion_hash(secuencia);
    Diccionario *nueva_entrada = (Diccionario *)malloc(sizeof(Diccionario));
    if (!nueva_entrada) {
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
    }
    strcpy(nueva_entrada->secuencia, secuencia);
    nueva_entrada->codigo = codigo;
    nueva_entrada->siguiente = tabla_hash[index];  // Encadenar en caso de colisión
    tabla_hash[index] = nueva_entrada;
}

// Busca una secuencia en el diccionario
int buscar(Diccionario *tabla_hash[], const char *secuencia) {
    unsigned int index = funcion_hash(secuencia);
    Diccionario *entrada = tabla_hash[index];
    while (entrada != NULL) {
        if (strcmp(entrada->secuencia, secuencia) == 0) {
            return entrada->codigo;  // Secuencia encontrada
        }
        entrada = entrada->siguiente;
    }
    return -1;  // No encontrado
}

// Ejemplo de uso
int main() {
    Diccionario *tabla_hash[DICT_SIZE];
    inicializar_diccionario(tabla_hash);

    // Insertar secuencias iniciales
    insertar(tabla_hash, "A", 1);
    insertar(tabla_hash, "B", 2);
    insertar(tabla_hash, "AB", 256);

    // Buscar secuencias
    int codigo = buscar(tabla_hash, "B");
    if (codigo != -1) {
        printf("Secuencia 'B' encontrada con código: %d\n", codigo);
    } else {
        printf("Secuencia 'B' no encontrada\n");
    }

    codigo = buscar(tabla_hash, "AB");
    if (codigo != -1) {
        printf("Secuencia 'AB' encontrada con código: %d\n", codigo);
    } else {
        printf("Secuencia 'AB' no encontrada\n");
    }

    // Liberar memoria (omitido en este ejemplo, pero se debe hacer en una implementación completa)

    return 0;
}
