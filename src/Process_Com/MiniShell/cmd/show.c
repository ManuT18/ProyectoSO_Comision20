#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "minishell: Se espera un argumento para \"show\"\n");
        return -1;
    }

    FILE *file = fopen(args[1], "r");
    if (file == NULL) {
        fprintf(stderr, "minishell: Error al abrir el archivo\n");
        return -1;
    }
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }
    fclose(file);

    return 0;
}