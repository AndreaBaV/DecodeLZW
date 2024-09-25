#ifndef DICCIONARIO_H
#define DICCIONARIO_H

#define DICT_SIZE 4096
#define MAX_SEQ_LENGTH 256

typedef struct Diccionario {
    unsigned char patron[MAX_SEQ_LENGTH];
    int longitud;
    int codigo;
    struct Diccionario *siguiente;
} Diccionario;

unsigned int funcionHash(const unsigned char *patron, int longitud);
void inicializacion(Diccionario *tablaHash);
void registro(Diccionario *tablaHash, const unsigned char *patron, int longitud, int codigo);
Diccionario* busqueda(Diccionario *tablaHash, int codigo);

#endif
