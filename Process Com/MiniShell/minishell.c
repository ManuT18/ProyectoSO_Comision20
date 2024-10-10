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

void mostrar_funcionando(char **args);
void mostrar_ayuda(char **args);
void crear_directorio(char **args);
void eliminar_directorio(char **args);
void cambiar_directorio(char **args);
void crear_archivo(char **args);
void listar_directorio(char **args);
void mostrar_contenido_archivo(char **args);
void cambiar_permisos_archivo(char **args);
void mostrar_about(char **args);
void mostrar_version(char **args);
void limpiar_pantalla(char **args);
void salir(char **args);

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
    {"exit", salir},
    {NULL, NULL}
};

void salir(char **args) {
    exit(0);
}

// Función para mostrar la ayuda con los comandos disponibles
void mostrar_ayuda(char **args) {
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
void cambiar_directorio(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Se espera un argumento para \"cd\"\n");
        return;
    }

    if (chdir(args[1]) != 0) {
        fprintf(stderr, "minishell: Error al cambiar de directorio\n");
    }
}

void limpiar_pantalla(char **args) {
    // https://stackoverflow.com/questions/37774983/clearing-the-screen-by-printing-a-character
    printf("\033[H\033[J"); // código de escape ANSI para limpiar la pantalla
    fflush(stdout);
}

void mostrar_funcionando(char **args) {
    printf("Funcionando!\n");
    fflush(stdout);
}

void mostrar_about(char **args) {
    printf("Minishell realizado por la comisión 20 de Sistemas Operativos.\n");
    fflush(stdout);
}

void mostrar_version(char **args) {
    printf("Minishell v1.0\n");
    fflush(stdout);
}

// Función para crear un directorio con permisos 0755 (rwxr-xr-x)
void crear_directorio(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Se espera un argumento para \"mkdir\"\n");
        return;
    }

    if (mkdir(args[1], 0777) != 0) {
        fprintf(stderr, "minishell: Error al crear el directorio\n");
    }
}

// Función para eliminar un directorio
void eliminar_directorio(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Se espera un argumento para \"rmdir\"\n");
        return;
    }

    if (rmdir(args[1]) != 0) {
        fprintf(stderr, "No se pudo eliminar el directorio\n");
    }
}

// Función para crear un archivo con permisos 0644 (rw-r--r--)
void crear_archivo(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Se espera un argumento para \"touch\"\n");
        return;
    }
    FILE *file = fopen(args[1], "w");
    if (file == NULL) {
        fprintf(stderr, "No se pudo abrir el archivo\n");
    } else {
        fclose(file);
    }
}

// Función para listar el contenido de un directorio
void listar_directorio(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Se espera un argumento para \"ls\"\n");
        return;
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
        return;
    }

    FTSENT *node; // la estructura FTSENT contiene información sobre un nodo del sistema de archivos
    while ((node = fts_read(ftsp)) != NULL) { // leer cada nodo del sistema de archivos
        if (node->fts_info & FTS_F) { // verificar si el nodo es un archivo (regular file)
            printf("%s\n", node->fts_name); // imprimir el nombre del archivo
        }
    }

    if (fts_close(ftsp) < 0) {
        fprintf(stderr, "minishell: Error al cerrar el sistema de archivos\n");
    }
}

// Función para mostrar el contenido de un archivo
void mostrar_contenido_archivo(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Se espera un argumento para \"cat\"\n");
        return;
    }

    FILE *file = fopen(args[1], "r");
    if (file == NULL) {
        fprintf(stderr, "minishell: Error al abrir el archivo\n");
        return;
    }
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }
    fclose(file);
}

// Función para cambiar los permisos de un archivo
void cambiar_permisos_archivo(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "Se esperan argumentos para \"chmod\"\n");
        return;
    }

    mode_t mode = strtol(args[1], NULL, 8);
    if (chmod(args[2], mode) != 0) {
        fprintf(stderr, "minishell: Error al cambiar los permisos del archivo\n");
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
            fprintf(stderr, "No se pudo obtener el directorio actual\n");
        }

        printf("minishell>%s$ ", cwd);
        if (fgets(comando, sizeof(comando), stdin) == NULL) {
            active = 0;
        }

        comando[strcspn(comando, "\n")] = '\0'; // no tome el enter

        args[0] = strtok(comando, " ");
        args[1] = strtok(NULL, " ");
        args[2] = strtok(NULL, " ");

        int found = 0;
        int i = 0;

        while (!found && commands[i].command != NULL) {
            if (strcmp(args[0], commands[i].command) == 0) {
                commands[i].function(args);
                found = 1;
            }
            i++;
        }

        if (!found) {
            printf("Comando desconocido: %s\n", args[0]);
            fflush(stdout);
        }
    }

    return 0;
}