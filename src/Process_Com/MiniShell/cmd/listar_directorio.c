#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fts.h>

int main (int argc, char **args) {
    // si no se pasa ningun argumento, listar el directorio actual
    if (args[1] == NULL) {
        args[1] = ".";
    }

    char dir[256];
    strcpy(dir, args[1]);
    dir[sizeof(dir) - 1] = '\0'; // null terminated
    
    /*
    Usamos la librería FTS para recorrer el sistema de archivos de forma recursiva.
    Esta decisión se sustenta en que FTS es parte del estándar POSIX y proporciona una forma más sencilla de recorrer el sistema de archivos que otras alternativas como opendir/readdir/closedir o la librería dirent.
    https://man7.org/linux/man-pages/man3/fts.3.html
    */
    char *paths[] = { dir, NULL }; // Crear un array de paths con el directorio a listar (NULL terminated)
    // Abrir el sistema de archivos para recorrerlo con la estructura FTS que es un árbol de directorios
    // FTS_NOCHDIR: No cambiar de directorio al abrir un directorio
    // FTS_PHYSICAL: No continuar con los enlaces simbólicos (solo listarlos)
    FTS *ftsp = fts_open(paths, FTS_NOCHDIR | FTS_PHYSICAL, NULL); 
    
    if (ftsp == NULL) {
        fprintf(stderr, "minishell: Error al abrir el sistema de archivos\n");
        return -1;
    }

    FTSENT *node; // la estructura FTSENT contiene información sobre un nodo del sistema de archivos
    while ((node = fts_read(ftsp)) != NULL) { // leer cada nodo del sistema de archivos
        // listar los directorios y archivos recursivamente
        switch (node->fts_info) {
            case FTS_D:
                printf("%s/\n", node->fts_name);
                break;
            case FTS_F:
                printf("%s\n", node->fts_name);
                break;
            default:
                break;
        }
    }

    if (fts_close(ftsp) < 0) {
        fprintf(stderr, "minishell: Error al cerrar el sistema de archivos\n");
    }

    return 0;
}