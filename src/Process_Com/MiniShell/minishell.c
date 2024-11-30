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
#include <sys/wait.h>
#include <fcntl.h>
#include <fts.h>

int main() {
    char comando[256];
    char *args[3];
    int active = 1;

    while (active) {
        // obtener y guardar la ubicación actual
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
            continue; // para evitar poner un else gigante
        }

        comando[strcspn(comando, "\n")] = '\0'; // para que no tome el enter al final del comando

        args[0] = strtok(comando, " "); // nombre del comando
        args[1] = strtok(NULL, " "); // argumento 1
        args[2] = strtok(NULL, " "); // argumento 2

        char path[256];
        snprintf(path, sizeof(path), "bin/Process_Com/MiniShell/cmd/%s", args[0]);

        pid_t pid = vfork();
        if (pid == 0) {
            // buscar el binario dentro de "/cmd"
            if (execv(path, args) == -1) {
                fprintf(stderr, "minishell: Comando no encontrado\n");
                exit(-1);
            }
        }
    
        waitpid(pid, NULL, 0);
    }

    return 0;
}