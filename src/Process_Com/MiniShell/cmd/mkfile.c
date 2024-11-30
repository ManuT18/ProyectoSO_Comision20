#include <stdio.h>
#include <stdlib.h>

// Función para crear un archivo con permisos 0666 (rw-rw-rw-)
int main (int argc, char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "minishell: Se espera un argumento para \"mkfile\"\n");
        return -1; 
    }

    // si el archivo ya existe, lanzar error
    if (fopen(args[1], "r") != NULL) {
        fprintf(stderr, "minishell: El archivo ya existe\n");
        return -1;
    }

    FILE *file = fopen(args[1], "w");
    if (file == NULL) {
        fprintf(stderr, "minishell: No se pudo crear el archivo\n");
    } else {
        fclose(file);
    }

    return 0;
}