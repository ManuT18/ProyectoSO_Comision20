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

void mostrar_ayuda(char **args);
void mostrar_version(char **args);
void limpiar_pantalla(char **args);
void salir(char **args);

typedef struct {
    char *command;
    void (*function)(char **args);
} Comando;

Comando commands[] = {
    {"help", mostrar_ayuda},
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
    printf("version - Mostrar la versión del shell\n");
    printf("help - Mostrar este mensaje de ayuda\n");
    printf("mkdir <directorio> - Crear un directorio\n");
    printf("rmdir <directorio> - Eliminar un directorio\n");
    printf("mkfile <archivo> - Crear un archivo\n");
    printf("list <directorio> - Listar el contenido de un directorio\n");
    printf("chdir <directorio> - Cambiar el directorio actual\n");
    printf("show <archivo> - Mostrar el contenido de un archivo\n");
    printf("chmod <permisos> <archivo> - Cambiar los permisos de un archivo, especificados en formato string\n");
    printf("clear - Limpiar la pantalla\n");
    printf("about - Mostrar información sobre el shell\n");
    printf("exit - Salir del shell\n");
    fflush(stdout);
}

void limpiar_pantalla(char **args) {
    // https://stackoverflow.com/questions/37774983/clearing-the-screen-by-printing-a-character
    printf("\033[H\033[J"); // código de escape ANSI para limpiar la pantalla
    fflush(stdout);
}

void mostrar_version(char **args) {
    printf("Minishell v1.0\n");
}

int main() {
    char comando[256];
    char *args[3];
    int active = 1;

    // dar la ilusión de un shell
    while (active) {
        // obtener y guardar en una variable la ubicación actual
        char cwd[1024];

        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            fprintf(stderr, "minishell: No se pudo obtener el directorio actual\n");
            strcpy(cwd, "");
        }

        printf("minishell>%s$ ", cwd);
        if (fgets(comando, sizeof(comando), stdin) == NULL) {
            active = 0;
        }

        // si solo apreta enter, no hacer nada
        if (comando[0] == '\n') {
            continue;
        }

        comando[strcspn(comando, "\n")] = '\0'; // para que no tome el enter

        args[0] = strtok(comando, " ");
        args[1] = strtok(NULL, " ");
        args[2] = strtok(NULL, " ");

        pid_t pid = fork();
        if (pid == 0) {
            // buscar el binario dentro de la carpeta del archivo
            execl(args[0], args[0], args[1], args[2], NULL);
        }

        waitpid(pid);
    }

    return 0;
}