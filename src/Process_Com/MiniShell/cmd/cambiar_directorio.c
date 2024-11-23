#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// implementación de "cd", cambiar directorio al que se pasa como argumento
int main (int argc, char **args) {
    if (args[1] == NULL) {
        // cambiar al directorio HOME
        if (chdir(getenv("HOME")) != 0) {
            fprintf(stderr, "minishell: Error al cambiar de directorio\n");
            return -1;
        }
    }

    if (chdir(args[1]) != 0) {
        fprintf(stderr, "minishell: Error al cambiar de directorio\n");
    }

    return 0;
}