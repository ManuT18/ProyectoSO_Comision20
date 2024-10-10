/**
 * Construir un shell que acepte un conjunto limitado de comandos de Unix.
 * Tiene que considerar como mínimo 6 comandos.
 * Explique las opciones de diseño que consideró al momento de implementarlo.
 * No puede invocar los comandos mediante la función system, y para la implementación de los mismos debe utilizar llamadas al sistema (system-calls) o funciones de librerías.
 * 
 * Los minimos comandos que debe tener son los siguientes:
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

void mostrar_funcionando();
void mostrar_ayuda(char *args[]);
void crear_directorio(char *dir);
void eliminar_directorio(char *dir);
void cambiar_directorio(char *directorio);
void crear_archivo(char *archivo);
void listar_directorio(char *dir);
void mostrar_contenido_archivo(char *archivo);
void cambiar_permisos_archivo(char *permisos, char *archivo);
void mostrar_about();
void mostrar_version();
void limpiar_pantalla(char *args[]);

typedef struct {
    char *command;
    void (*function)(char **args);
} Comando;

Comando commands[] = {
    {"test", mostrar_funcionando},
    {"help", mostrar_ayuda},
    {"mkdir", crear_directorio},
    {"rmdir", eliminar_directorio},
    {"cd", cambiar_directorio},
    {"touch", crear_archivo},
    {"ls", listar_directorio},
    {"cat", mostrar_contenido_archivo},
    {"chmod", cambiar_permisos_archivo},
    {"about", mostrar_about},
    {"version", mostrar_version},
    {"clear", limpiar_pantalla},
    {"exit", exit},
    {NULL, NULL}
};

// Función para mostrar la ayuda con los comandos disponibles
void mostrar_ayuda(char *args[]) {
    printf("Comandos disponibles:\n");
    printf("help - Mostrar este mensaje de ayuda\n");
    printf("mkdir <directorio> - Crear un directorio\n");
    printf("rmdir <directorio> - Eliminar un directorio\n");
    printf("touch <archivo> - Crear un archivo\n");
    printf("ls <directorio> - Listar el contenido de un directorio\n");
    printf("cd <directorio> - Cambiar el directorio actual\n");
    printf("cat <archivo> - Mostrar el contenido de un archivo\n");
    printf("chmod <permisos> <archivo> - Cambiar los permisos de un archivo\n");
    printf("exit - Salir del shell\n");
    printf("version - Mostrar la versión del shell\n");
    printf("about - Mostrar información sobre el shell\n");
    printf("clear - Limpiar la pantalla\n");
    fflush(stdout);
}

// implementación de "cd", cambiar directorio al que se pasa como argumento
void cambiar_directorio(char *directorio) {
    if (chdir(directorio) == -1) {
        perror("chdir");
    }
}

void limpiar_pantalla(char *args[]) {
    // https://stackoverflow.com/questions/37774983/clearing-the-screen-by-printing-a-character
    printf("\033[H\033[J"); // código de escape ANSI para limpiar la pantalla
    fflush(stdout);
}

void mostrar_funcionando() {
    printf("Funcionando!\n");
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
    int active = 1;

    while (active) {
        // dar la ilusión de un shell
        // obtener y guardar en una variable la ubicación actual
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd");
        }
        // cortar el path de cwd para que sea relativo
        printf("minishell>%s$ ", cwd);
        if (fgets(comando, sizeof(comando), stdin) == NULL) {
            break; // Salir del bucle si fgets falla
        }
        comando[strcspn(comando, "\n")] = '\0'; // no toma el enter

        args[0] = strtok(comando, " ");
        args[1] = strtok(NULL, " ");
        args[2] = strtok(NULL, " ");

        int i;
        int found = 0;
        for (i = 0; commands[i].command != NULL; i++) {
            if (strcmp(args[0], commands[i].command) == 0) {
                commands[i].function(args);
                found = 1;
                break;
            }
        }

        if (!found) {
            printf("Comando desconocido: %s\n", args[0]);
            fflush(stdout);
        }
    }

    return 0;
}