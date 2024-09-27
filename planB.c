#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DICT_SIZE 4095
#define MAX_SEQ_LENGTH 256

typedef struct Arreglo {
    unsigned char secuencia[MAX_SEQ_LENGTH];
    int longitud;
    unsigned int codigo;
    int error;
} Arreglo;

/*
 inicializar_diccionario - inicializa el diccionario con valores vacíos. 
                            Se establece el código y el indicador de error.
 
                            Parámetros: diccionario (estructura de datos)
*/
void inicializar_diccionario(Arreglo diccionario[]) {
    for (int i = 0; i <= DICT_SIZE + 1; i++) {
        diccionario[i].longitud = 0;
        diccionario[i].codigo = 0;
        diccionario[i].error = 7;
    }
}

/*
 agregar_entrada - agrega una secuencia nueva al diccionario con un código asociado.
                   Parámetros: diccionario (estructura de datos), secuencia (cadena de bytes),
                               longitud (longitud de la secuencia), código (índice en el diccionario), 
                               error (indicador de error)
*/
void agregar_entrada(Arreglo diccionario[], const unsigned char *secuencia, int longitud, unsigned int codigo, int error) {
    if (codigo <= DICT_SIZE) {
        memcpy(diccionario[codigo].secuencia, secuencia, longitud);
        diccionario[codigo].longitud = longitud;
        diccionario[codigo].codigo = codigo;
        diccionario[codigo].error = error;
    }
}

/*
 buscar_codigo - busca una secuencia en el diccionario usando el código como clave.
                 Si se encuentra, retorna el código de la entrada en el diccionario.
                 Parámetros: diccionario (estructura de datos), código (clave de búsqueda)
                 Retorno: código de la entrada si existe, -1 si no se encuentra.
*/
int buscar_codigo(Arreglo diccionario[], int codigo) {
    if (codigo >= 0 && codigo <= DICT_SIZE) {
        if (diccionario[codigo].codigo == codigo) {
            return codigo;  // Devuelve el código encontrado.
        }
    }
    return -1;  // Código no encontrado.
}

/*
 leer_codigo - lee un código comprimido del archivo de entrada usando un buffer de bits.
               El código es de 12 bits. También se gestiona un código especial (4095) para reiniciar el diccionario.
               Parámetros: entrada (archivo comprimido), buffer (almacenamiento temporal de bits), 
                           bits_restantes (número de bits pendientes en el buffer)
*/
int leer_codigo(FILE *entrada, int *buffer, int *bits_restantes) {
    int codigo = 0;
    int error;

    while (*bits_restantes < 12) {
        int byte = fgetc(entrada);
        if (byte == EOF) return -1;

        *buffer = (*buffer << 8) | byte;
        *bits_restantes += 8;
    }

    codigo = (*buffer >> (*bits_restantes - 12)) & 0xFFF;

    if (codigo == 4095) {
        error = -2;
        return error;
    }

    *bits_restantes -= 12;

    return codigo;
}

/*
 descomprimir_archivo - función principal que descomprime un archivo usando un algoritmo basado en diccionario.
                        Lee códigos comprimidos y los traduce a secuencias originales, escribiéndolas en el archivo de salida.
                        Parámetros: entrada_path (ruta al archivo comprimido), salida_path (ruta al archivo descomprimido)
*/
void descomprimir_archivo(const char *entrada_path, const char *salida_path) {
    FILE *entrada = fopen(entrada_path, "rb");
    FILE *salida = fopen(salida_path, "wb");

    Arreglo diccionario[DICT_SIZE + 1];
    inicializar_diccionario(diccionario);

    for (int i = 0; i < 256; i++) {
        unsigned char secuencia[1] = {(unsigned char)i};
        agregar_entrada(diccionario, secuencia, 1, i, 7);
    }

    int codigo_siguiente = 256;
    int anterior_codigo = -1;
    int buffer = 0;
    int bits_restantes = 0;
    int codigo;

    while ((codigo = leer_codigo(entrada, &buffer, &bits_restantes)) != -1) {
        if (codigo == -2) {
            inicializar_diccionario(diccionario);

            for (int i = 0; i < 256; i++) {
                unsigned char secuencia[1] = {(unsigned char)i};
                agregar_entrada(diccionario, secuencia, 1, i, 7);
            }

            codigo_siguiente = 256;
            anterior_codigo = -1;
            continue;
        }

        int entrada_codigo;
        unsigned char secuencia[MAX_SEQ_LENGTH];
        int longitud;

        if (codigo < 256) {
            secuencia[0] = (unsigned char)codigo;
            longitud = 1;
        } else {
            entrada_codigo = buscar_codigo(diccionario, codigo);
            if (entrada_codigo != -1) {
                memcpy(secuencia, diccionario[entrada_codigo].secuencia, diccionario[entrada_codigo].longitud);
                longitud = diccionario[entrada_codigo].longitud;
            } else if (codigo == codigo_siguiente && anterior_codigo != -1) {
                entrada_codigo = buscar_codigo(diccionario, anterior_codigo);
                if (entrada_codigo != -1) {
                    memcpy(secuencia, diccionario[entrada_codigo].secuencia, diccionario[entrada_codigo].longitud);
                    secuencia[diccionario[entrada_codigo].longitud] = diccionario[entrada_codigo].secuencia[0];
                    longitud = diccionario[entrada_codigo].longitud + 1;
                }
            }
        }

        fwrite(secuencia, 1, longitud, salida);

        if (anterior_codigo != -1) {
            entrada_codigo = buscar_codigo(diccionario, anterior_codigo);
            if (entrada_codigo != -1) {
                unsigned char nueva_secuencia[MAX_SEQ_LENGTH];
                memcpy(nueva_secuencia, diccionario[entrada_codigo].secuencia, diccionario[entrada_codigo].longitud);
                nueva_secuencia[diccionario[entrada_codigo].longitud] = secuencia[0];

                if (codigo_siguiente < DICT_SIZE) {
                    agregar_entrada(diccionario, nueva_secuencia, diccionario[entrada_codigo].longitud + 1, codigo_siguiente++, 7);
                }
            }
        }

        anterior_codigo = codigo;
    }

    fclose(entrada);
    fclose(salida);
}

int main(int argc, char *argv[]) {
    descomprimir_archivo(argv[1], argv[2]);
    return 0;
}
