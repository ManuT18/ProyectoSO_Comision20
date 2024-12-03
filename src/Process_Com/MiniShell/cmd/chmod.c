#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

char* permisos_a_modo_octal(char *permisos);

// Función para cambiar los permisos de un archivo
int main (int argc, char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "minishell: Se esperan argumentos para \"chmod\"\n");
        return -1;
    }

    // el primer argumento debe tener una longitud de 9 caracteres para que sea un permiso posiblemente valido
    if (strlen(args[1]) != 9) {
        fprintf(stderr, "minishell: Los permisos deben tener 9 caracteres\n");
        return -1;
    }

    char *mode_str = permisos_a_modo_octal(args[1]);
    if (mode_str == NULL) {
        return -1;
    }
    mode_t mode = strtol(mode_str, 0, 8);
    char *file = args[2];
    printf("mode: %d\n", mode);

    if (chmod(file, mode) != 0) {
        fprintf(stderr, "minishell: Error al cambiar los permisos del archivo\n");
    }

    return 0;
}

// convierte un string de permisos a un string en octal
char* permisos_a_modo_octal(char *permisos) {
    static char octal[5];
    int valor = 0;

    for (int i = 0; i < 3; i++) {
        valor = 0;
        if (permisos[i*3] != 'r' && permisos[i*3] != '-') {
            fprintf(stderr, "minishell: Permiso inválido: %c\n", permisos[i*3]);
            return NULL;
        }
        if (permisos[i*3+1] != 'w' && permisos[i*3+1] != '-') {
            fprintf(stderr, "minishell: Permiso inválido: %c\n", permisos[i*3+1]);
            return NULL;
        }
        if (permisos[i*3+2] != 'x' && permisos[i*3+2] != '-') {
            fprintf(stderr, "minishell: Permiso inválido: %c\n", permisos[i*3+2]);
            return NULL;
        }

        if (permisos[i*3] == 'r') {
            valor += 4;
        }
        if (permisos[i*3+1] == 'w') {
            valor += 2;
        }
        if (permisos[i*3+2] == 'x') {
            valor += 1;
        }
        octal[i+1] = '0' + valor; // índice corrido para dejar el primer carácter como '0'
    }
    octal[4] = '\0';
    octal[0] = '0';

    return octal;
}