#include "diccionario.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 funcionHash - asigna el espacio correspondiente a alguna secuencia dentro de
              la estructura de datos.


                Parámetros: patron (ab, abab, una vez)
                            longitud (número de bytes del patrón: 1-256)

                Algoritmo implementado: FNV-1a
                    1. Semilla del algoritmo: 2166136261
                    2. XOR entre el primer byte del patrón y el valor de hash
                    Reasignar el valor de hash
                    Repetir con el siguiente byte hasta que se termine el patrón
                    3. Multiplicación por 17 (número primo)
                    4. División entre el tamaño del diccionario (asignar el bucket)

                Fuente de información: 1.3 ReadMe
 */


unsigned int funcionHash(const unsigned char *patron, int longitud) {
    unsigned int hash = 2166136261;

    for (int i = 0; i < longitud; i++){
        hash ^= patron[i];
    }

    hash = (hash * 17) % DICT_SIZE;

    return hash;
}



/*
 inicializacion - reserva e inicializa el espacio de memoria para el diccionario

                  Parámetros: tablaHash (arreglo del tamaño del diccionario: 4096
                                         el tipo de dato que contiene son Diccionarios)

                  Notas: calloc reserva la memoria e inicializa con el valor 0
                  Fuentes de información: 1.4 ReadMe


 */

void inicializacion(Diccionario *tablaHash) {
    *tablaHash = (Diccionario **) calloc(DICT_SIZE, sizeof(Diccionario*))
}

/*
 registro - 
*/

void registro(Diccionario *tablaHash, const unsigned char *patron, int longitud, int codigo) {
    unsigned int numeroBucket = funcionHash(patron, longitud);
    Diccionario *nuevo = (Diccionario *)malloc(sizeof(Diccionario));
    memcpy(nuevo->patron, patron, longitud);
    nuevo->longitud= longitud;
    nuevo->codigo= codigo;

    if(tablaHash[numeroBucket]==NULL){
      tablaHash[numeroBucket] = nuevo;
      nuevo->siguiente=NULL;
    }
    else {
     Diccionario *actual = tablaHash[numeroBucket];
     while (actual->siguiente != NULL){
      actual = 
     }
    }
}

Diccionario* busqueda(Diccionario *tablaHash, int codigo) {
    //pendiente
}
