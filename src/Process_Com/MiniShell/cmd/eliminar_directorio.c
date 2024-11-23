#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "minishell: Se espera un argumento para \"rmdir\"\n");
        return -1;
    }

    if (rmdir(args[1]) != 0) {
        fprintf(stderr, "minishell: No se pudo eliminar el directorio\n");
    }

    return 0;
}