/**
 * PumperInc.
 * Una cadena de comida rápida de la ciudad ofrece distintos tipos de combos a distintos tipos de clientes. 
 * Para la preparación de combos dispone de personal especializado. 
 * Un empleado prepara hamburguesas simples, otro prepara el menú apto vegano y para las papas fritas, que es lo más solicitado, se cuenta con dos empleados. 
 * Otro empleado recibe los pedidos y despacha enseguida lo solicitado por cada cliente. 
 * Cada cliente espera ser atendido en una cola, aunque si hay mucha gente puede decidir marcharse y volver más tarde.
 * Cuando el cliente tiene una credencial de cliente VIP, este tiene prioridad sobre los demás, teniéndose que despachar su pedido lo antes posible. 
 * El cliente aguarda a que su pedido esté listo, y una vez que lo recibe, se marcha contento.
 * 
 * (a) Describa las políticas seleccionadas para el modelo e implementación del problema utilizando procesos y pipes para la comunicación entre los participantes. Los procesos pueden utilizar hilos internamente para resolver algunas cuestiones.
 * 
 * (b) Resuelve el mismo problema pero utilizando colas de mensajes, detalle las políticas seleccionadas para el modelo, diseño e implementación. ¿Tiene alguna ventaja esta implementación?
 */


/**
 * Modelo:
 *  - Un proceso para el empleado que prepara hamburguesas simples (H),
 *  - Un proceso para el empleado que prepara el menú apto vegano (V),
 *  - Un proceso para los empleados que preparan papas fritas (P),
 *  - Un proceso para el empleado despachador (D)
 *  - Pipes para la comunicación entre los clientes y el despachador.
 *  - Pipes para la comunicación entre el despachador y los empleados.
 *  - Pipes para la comunicación entre los empleados y el despachador.
 *  - Pipes para la comunicación entre el despachador y los clientes.
 */

#define REPETITIONS 100

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Primero vamos a resolver el problema considerando solo a los clientes simples (no VIPs).
int main() {

    int confirmation;

    int pipe_C_D[2]; // pipe para la comunicación entre Cliente (C) y Despachador (D)
    int pipe_D_H[2]; // pipe para la comunicación entre Despachador (D) y Hamburguesero (H)
    int pipe_D_V[2]; // pipe para la comunicación entre Despachador (D) y Vegano (V)
    int pipe_D_P[2]; // pipe para la comunicación entre Despachador (D) y los PapaFriteros (P)
    int pipe_H_D[2]; // pipe para la comunicación entre Hamburguesero (H) y Despachador (D)
    int pipe_V_D[2]; // pipe para la comunicación entre Vegano (V) y Despachador (D)
    int pipe_P_D[2]; // pipe para la comunicación entre PapaFritero y Despachador (D)
    int pipe_D_C[2]; // pipe para la comunicación entre Despachador (D) y Cliente (C)

    pipe(pipe_C_D);
    pipe(pipe_D_H);
    pipe(pipe_D_V);
    pipe(pipe_D_P);
    pipe(pipe_H_D);
    pipe(pipe_V_D);
    pipe(pipe_P_D);
    pipe(pipe_D_C);

    pid_t pid_H = fork();
    if (pid_H == 0) {
        // Proceso Hamburguesero
        close(pipe_H_D[0]);
        close(pipe_D_H[1]);

        // Esperar pedido del Despachador
        read(pipe_D_H[0], &confirmation, sizeof(int));
        // Preparar hamburguesa
        printf("Hamburguesero: Preparando hamburguesa\n");
        fflush(stdout);
        // Enviar hamburguesa al Despachador
        write(pipe_H_D[1], &confirmation, sizeof(int));

        exit(0);
    }

    pid_t pid_V = fork();
    if (pid_V == 0) {
        // Proceso Vegano
        close(pipe_V_D[0]);
        close(pipe_D_V[1]);

        // Esperar pedido del Despachador
        read(pipe_D_V[0], &confirmation, sizeof(int));
        // Preparar menú vegano
        printf("Vegano: Preparando menú vegano\n");
        fflush(stdout);
        // Enviar menú vegano al Despachador
        write(pipe_V_D[1], &confirmation, sizeof(int));

        exit(0);
    }

    pid_t pid_P = fork();
    if (pid_P == 0) {
        // Proceso PapaFritero
        close(pipe_P_D[0]);
        close(pipe_D_P[1]);

        // Esperar pedido del Despachador
        read(pipe_D_P[0], &confirmation, sizeof(int));
        // Preparar papas fritas
        printf("PapaFritero: Preparando papas fritas\n");
        fflush(stdout);
        // Enviar papas fritas al Despachador
        write(pipe_P_D[1], &confirmation, sizeof(int));

        exit(0);
    }

    pid_t pid_D = fork();
    if (pid_D == 0) {
        // Proceso Despachador
        close(pipe_D_H[0]);
        close(pipe_D_V[0]);
        close(pipe_D_P[0]);
        close(pipe_H_D[1]);
        close(pipe_V_D[1]);
        close(pipe_P_D[1]);

        // Recibir pedido del Cliente
        read(pipe_C_D[0], &confirmation, sizeof(int));
        printf("Despachador: Recibiendo pedido\n");
        fflush(stdout);
        // Enviar pedido al Hamburguesero
        write(pipe_D_H[1], &confirmation, sizeof(int));
        // Enviar pedido al Vegano
        write(pipe_D_V[1], &confirmation, sizeof(int));
        // Enviar pedido a los PapaFriteros
        write(pipe_D_P[1], &confirmation, sizeof(int));
        // Esperar hamburguesa del Hamburguesero
        read(pipe_H_D[0], &confirmation, sizeof(int));
        // Esperar menú vegano del Vegano
        read(pipe_V_D[0], &confirmation, sizeof(int));
        // Esperar papas fritas de los PapaFriteros
        read(pipe_P_D[0], &confirmation, sizeof(int));
        // Entregar pedido al Cliente
        printf("Despachador: Entregando pedido\n");
        fflush(stdout);
        write(pipe_D_C[1], &confirmation, sizeof(int));

        exit(0);
    }

    pid_t pid_C = fork();
    if (pid_C == 0) {
        // Proceso Cliente
        close(pipe_C_D[0]);
        close(pipe_D_C[1]);

        // Enviar pedido al Despachador
        printf("Cliente: Realizando pedido\n");
        fflush(stdout);
        write(pipe_C_D[1], &confirmation, sizeof(int));
        // Esperar pedido del Despachador
        read(pipe_D_C[0], &confirmation, sizeof(int));
        printf("Cliente: Marchándome con el pedido\n");
        fflush(stdout);

        exit(0);
    }

    waitpid(pid_H, NULL, 0);
    waitpid(pid_V, NULL, 0);
    waitpid(pid_P, NULL, 0);
    waitpid(pid_D, NULL, 0);
    waitpid(pid_C, NULL, 0);

    return 0;
}