#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

// Función para crear un directorio con permisos 0666 (rw-rw-rw-)
int main (int argc, char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "minishell: Se espera un argumento para \"mkdir\"\n");
        return;
    }

    if (mkdir(args[1], 0666) != 0) {
        fprintf(stderr, "minishell: Error al crear el directorio\n");
    }
    
    return 0;
}