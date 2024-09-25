#include "diccionario.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int funcion_hash(const unsigned char *secuencia, int longitud) {
    // ...
}

void inicializar_diccionario(Diccionario *tabla_hash[]) {
    // ...
}

void agregar_entrada(Diccionario *tabla_hash[], const unsigned char *secuencia, int longitud, int codigo) {
    // ...
}

Diccionario* buscar_entrada(Diccionario *tabla_hash[], int codigo) {
    // ...
}
