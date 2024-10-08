/**
 * Construir un shell que acepte un conjunto limitado de comandos de Unix.
 * Tiene que considerar como mínimo 6 comandos.
 * Explique las opciones de diseño que consideró al momento de implementarlo.
 * No puede invocar los comandos mediante la función system, y para la implementación de los mismos debe utilizar llamadas al sistema (system-calls) o funciones de librerías.
 * 
 * Los minimos comandos que debe tener son los siguientes:
 * 
 * (a) Mostrar una ayuda con los comandos disponibles.
 * (b) Crear un directorio
 * (c) Eliminar un directorio
 * (d) Crear un archivo
 * (e) Listar el contenido de un directorio
 * (f) Mostrar el contenido de un archivo
 * (g) Modificar los permisos de un archivo. Los permisos son de lectura, escritura y ejecución.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fts.h>

// Función para mostrar la ayuda con los comandos disponibles
void mostrar_ayuda() {
    printf("Comandos disponibles:\n");
    printf("ayuda - Mostrar este mensaje de ayuda\n");
    printf("mkdir <directorio> - Crear un directorio\n");
    printf("rmdir <directorio> - Eliminar un directorio\n");
    printf("touch <archivo> - Crear un archivo\n");
    printf("ls <directorio> - Listar el contenido de un directorio\n");
    printf("cat <archivo> - Mostrar el contenido de un archivo\n");
    printf("chmod <permisos> <archivo> - Cambiar los permisos de un archivo\n");
    printf("salir - Salir del shell\n");
    printf("version - Mostrar la versión del shell\n");
    fflush(stdout);
}

void mostrar_about() {
    printf("Minishell realizado por la comisión 20 de Sistemas Operativos.\n");
    fflush(stdout);
}

void mostrar_version() {
    printf("Minishell v1.0\n");
    fflush(stdout);
}

// Función para crear un directorio con permisos 0755 (rwxr-xr-x)
void crear_directorio(char *dir) {
    if (mkdir(dir, 0755) == -1) {
        perror("mkdir");
    }
}

// Función para eliminar un directorio
void eliminar_directorio(char *dir) {
    if (rmdir(dir) == -1) {
        perror("rmdir");
    }
}

// Función para crear un archivo con permisos 0644 (rw-r--r--)
void crear_archivo(char *archivo) {
    int fd = open(archivo, O_CREAT | O_WRONLY, 0644); 
    if (fd == -1) {
        perror("open");
    } else {
        close(fd); // Cerrar el archivo si se creó correctamente
    }
}

// Función para listar el contenido de un directorio
void listar_directorio(char *dir) {
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
        perror("fts_open");
        return;
    }

    FTSENT *node; // la estructura FTSENT contiene información sobre un nodo del sistema de archivos
    while ((node = fts_read(ftsp)) != NULL) { // leer cada nodo del sistema de archivos
        if (node->fts_info & FTS_F) { // verificar si el nodo es un archivo (regular file)
            printf("%s\n", node->fts_name); // imprimir el nombre del archivo
        }
    }

    if (fts_close(ftsp) < 0) {
        perror("fts_close");
    }
}

// Función para mostrar el contenido de un archivo
void mostrar_contenido_archivo(char *archivo) {
    char buffer[1024];
    FILE *file = fopen(archivo, "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }
    size_t bytes_leidos;
    while ((bytes_leidos = fread(buffer, 1, sizeof(buffer) - 1, file)) > 0) {
        buffer[bytes_leidos] = '\0'; // null terminated
        printf("%s", buffer);
    }
    if (ferror(file)) {
        perror("fread");
    }
    fclose(file);
}

// Función para cambiar los permisos de un archivo
void cambiar_permisos_archivo(char *permisos, char *archivo) {
    mode_t modo = strtol(permisos, NULL, 8); // convertir los permisos de string a octal
    if (chmod(archivo, modo) == -1) {
        perror("chmod");
    }
}

int main() {
    char comando[256];
    char *args[3];

    while (1) {
        // dar la ilusión de un shell
        printf("minishell> ");
        if (fgets(comando, sizeof(comando), stdin) == NULL) {
            break; // Salir del bucle si fgets falla
        }
        comando[strcspn(comando, "\n")] = '\0'; // Eliminar el salto de línea

        args[0] = strtok(comando, " ");
        args[1] = strtok(NULL, " ");
        args[2] = strtok(NULL, " ");

        if (args[0] == NULL) {
            continue; // reintentar si no se ingresó algún comando
        } else if (strcmp(args[0], "test") == 0) {
            printf("Funcionando!\n");
            fflush(stdout);
        } else if (strcmp(args[0], "ayuda") == 0) {
            mostrar_ayuda();
        } else if (strcmp(args[0], "mkdir") == 0 && args[1] != NULL) {
            crear_directorio(args[1]);
        } else if (strcmp(args[0], "rmdir") == 0 && args[1] != NULL) {
            eliminar_directorio(args[1]);
        } else if (strcmp(args[0], "touch") == 0 && args[1] != NULL) {
            crear_archivo(args[1]);
        } else if (strcmp(args[0], "ls") == 0 && args[1] != NULL) {
            listar_directorio(args[1]);
        } else if (strcmp(args[0], "cat") == 0 && args[1] != NULL) {
            mostrar_contenido_archivo(args[1]);
        } else if (strcmp(args[0], "chmod") == 0 && args[1] != NULL && args[2] != NULL) {
            cambiar_permisos_archivo(args[1], args[2]);
        } else if (strcmp(args[0], "salir") == 0) {
            break;
        } else if (strcmp(args[0], "about") == 0) {
            mostrar_about();
        }
        else if (strcmp(args[0], "version") == 0) {
            mostrar_version();
        } else {
            printf("Comando desconocido: %s\n", args[0]);
        }
    }

    return 0;
}