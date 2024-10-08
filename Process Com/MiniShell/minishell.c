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