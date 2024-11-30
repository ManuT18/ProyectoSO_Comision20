#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

// Función para crear un directorio con permisos 0666 (rw-rw-rw-)
int main (int argc, char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "minishell: Se espera un argumento para \"mkdir\"\n");
        return -1;
    }

    if (mkdir(args[1], 0666) != 0) {
        switch errno {
            case EEXIST:
                fprintf(stderr, "minishell: El directorio ya existe\n");
                return -1;
                break;
            case ENAMETOOLONG:
                fprintf(stderr, "minishell: El nombre del directorio es muy largo\n");
                return -1;
                break;
            case EPERM:
                fprintf(stderr, "minishell: No tiene permisos para crear el directorio en el directorio ubicado\n");
                return -1;
                break;
            default:
                fprintf(stderr, "minishell: Error al crear el directorio\n");
                break;
        }
    }
    
    return 0;
}