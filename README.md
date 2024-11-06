# Proyecto de Sincronización y Comunicación de Procesos - Comisión 20 de la cátedra Sistemas Operativos - UNS

Este proyecto incluye varios ejercicios relacionados con la sincronización y comunicación entre procesos. Los ejercicios están escritos en **C** y abordan temas como la gestión de procesos, la sincronización utilizando semáforos, y la comunicación entre procesos mediante pipes, colas de mensajes, entre otros.

## Estructura del Proyecto

El proyecto se organiza en dos directorios principales:

- **`src/`**: Contiene los archivos fuente del proyecto.
  - **`Process_Com/`**: Implementaciones relacionadas con la comunicación de procesos.
    - **`Minishell/`**: Shell interactiva.
    - **`PumperInc/`**: Implementaciones de sistemas de colas y pipes.
  - **`Process_Sync/`**: Implementaciones relacionadas con la sincronización de procesos.
    - **`Motos/`**: Ejemplo de sincronización de varios procesos.
    - **`SantaClaus/`**: Simulación de tareas sincronizadas.
  
- **`bin/`**: Contiene los archivos binarios generados después de la compilación, con el mismo formato de carpetas que **`src`**.

## Requisitos

- **GCC** o cualquier compilador compatible con C.
- **make** para la automatización de la compilación y ejecución.
  
## Instalación y Uso

### Compilación de los Archivos Fuente

1. Cloná el repositorio en tu máquina local:

   ```bash
   git clone https://github.com/tu_usuario/tu_repositorio.git
   cd tu_repositorio
   ```

2. Usá `make` para compilar todos los archivos fuente:

   ```bash
   make all
   ```

   Esto generará los archivos binarios dentro de la carpeta `bin/`, correspondientes a cada módulo.

### Ejecución de los Programas

Para ejecutar los programas de manera individual, podés usar las siguientes instrucciones:

- **Ejecutar un programa específico:**

   ```bash
   make run_minishell       # Ejecutar el programa Minishell
   make run_pumper_queues   # Ejecutar el programa PumperInc - Queues
   make run_pumper_pipes    # Ejecutar el programa PumperInc - Pipes
   make run_motos           # Ejecutar el programa Motos
   make run_santaclaus      # Ejecutar el programa SantaClaus
   ```

### Limpiar los Archivos Generados

Para eliminar los binarios generados, usá el siguiente comando:

```bash
make clean
```

Esto eliminará todos los archivos dentro de `bin/`.

## Archivos y Funcionalidad

### `Minishell`

Este programa simula una shell básica que interactúa con el sistema operativo, permitiendo al usuario ejecutar comandos.

### `PumperInc`

Implementación de un sistema de comunicación entre procesos utilizando pipes y colas de mensajes.

### `Motos`

Ejemplo de sincronización de procesos utilizando semáforos. El caso de estudio es la producción de un taller de motos.

### `SantaClaus`

Ejemplo de sincronización de procesos utilizando semáforos. El caso de estudio es Santa Claus en su tienda del Polo Norte, repartiendo regalos y atendiendo elfos.

## Contribuciones

Las contribuciones son bienvenidas. Si tenés sugerencias o mejoras para el proyecto, por favor, abrí un **issue** o realizá un **pull request**.

## Licencia

Este proyecto está bajo la Licencia MIT - consultá el archivo [LICENSE](LICENSE.md) para más detalles.
