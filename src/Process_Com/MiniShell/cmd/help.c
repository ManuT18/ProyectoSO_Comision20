#include <stdio.h>

// Función para mostrar la ayuda con los comandos disponibles
int main(int argc, char **args) {
    printf("** Comandos disponibles **\n");
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
    fflush(stdout);

    return 0;
}