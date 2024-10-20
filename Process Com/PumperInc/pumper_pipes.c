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
 *  - Un proceso para el empleado despachador (D).
 *  - Pipes para la comunicación entre los clientes y el despachador.
 *  - Pipes para la comunicación entre el despachador y los empleados.
 *  - Pipes para la comunicación entre los empleados y el despachador.
 *  - Pipes para la comunicación entre el despachador y los clientes.
 * 
 * Un cliente llega a donde el despachador, le indica su pedido y espera a que el despachador le indique que su pedido está listo.
 * El despachador va a tener dos hilos: uno es para indicarle a los cocineros qué deben cocinar y esperar por la realización de un pedido que venga de un cliente común, y el otro hilo es para lo mismo pero del cliente VIP. Es importante que el despachador utilice ambos hilos dado que, si llegara un cliente VIP después de uno común, su pedido debería ser atendido, en lugar de que espere. El proceso general del despachador (con su hilo master) va a atender a los clientes, y dependiendo cuál llegue, spawnea uno u otro hilo.
 * 
 * El código para los pedidos será el siguiente:
 *      - 1: solo hamburguesa
 *      - 2: solo menú vegano
 *      - 3: solo papas fritas
 *      - 4: hamburguesa y menú vegano
 *      - 5: hamburguesa y papas fritas
 *      - 6: menú vegano y papas fritas
 *      - 7: hamburguesa, menú vegano y papas fritas
 * A partir de 8 hasta 15, son los mismos pedidos (en el mismo orden) de un cliente VIP.
 * 
 */

#define REPETITIONS 100
#define NUM_CLIENTES 7

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>

void *pedidos_comunes();
void *pedidos_vip();

int pedido;

int pipe_C_D[2]; // pipe para la comunicación entre Cliente (C) y Despachador (D)
int pipe_D_H[2]; // pipe para la comunicación entre Despachador (D) y Hamburguesero (H)
int pipe_D_V[2]; // pipe para la comunicación entre Despachador (D) y Vegano (V)
int pipe_D_P[2]; // pipe para la comunicación entre Despachador (D) y los PapaFriteros (P)
int pipe_H_D[2]; // pipe para la comunicación entre Hamburguesero (H) y Despachador (D)
int pipe_V_D[2]; // pipe para la comunicación entre Vegano (V) y Despachador (D)
int pipe_P_D[2]; // pipe para la comunicación entre PapaFritero y Despachador (D)
int pipe_D_C[2]; // pipe para la comunicación entre Despachador (D) y Cliente (C)

int main() {

    pipe(pipe_C_D);
    pipe(pipe_D_H);
    pipe(pipe_D_V);
    pipe(pipe_D_P);
    pipe(pipe_H_D);
    pipe(pipe_V_D);
    pipe(pipe_P_D);
    pipe(pipe_D_C);

    pid_t pid_H = fork();
    if (pid_H == 0) { // Proceso Hamburguesero
        
        close(pipe_H_D[0]);
        close(pipe_D_H[1]);

        while (1) {
            // Esperar pedido del Despachador
            read(pipe_D_H[0], &pedido, sizeof(int));
            // Preparar hamburguesa
            printf("Hamburguesero: Preparando hamburguesa\n");
            fflush(stdout);
            // Enviar hamburguesa al Despachador
            write(pipe_H_D[1], &pedido, sizeof(int));
        }
    }

    pid_t pid_V = fork();
    if (pid_V == 0) { // Proceso Vegano
        
        close(pipe_V_D[0]);
        close(pipe_D_V[1]);

        while (1) {
            // Esperar pedido del Despachador
            read(pipe_D_V[0], &pedido, sizeof(int));
            // Preparar menú vegano
            printf("Vegano: Preparando menú vegano\n");
            fflush(stdout);
            // Enviar menú vegano al Despachador
            write(pipe_V_D[1], &pedido, sizeof(int));
        }
    }

    pid_t pid_P1 = fork();
    if (pid_P1 == 0) { // Proceso PapaFritero 1
        
        close(pipe_P_D[0]);
        close(pipe_D_P[1]);

        while (1) {
            // Esperar pedido del Despachador
            read(pipe_D_P[0], &pedido, sizeof(int));
            // Preparar papas fritas
            printf("PapaFritero: Preparando papas fritas\n");
            fflush(stdout);
            // Enviar papas fritas al Despachador
            write(pipe_P_D[1], &pedido, sizeof(int));
        }
    }

    pid_t pid_P2 = fork();
    if (pid_P2 == 0) { // Proceso PapaFritero 2
        
        close(pipe_P_D[0]);
        close(pipe_D_P[1]);

        while (1) {
            // Esperar pedido del Despachador
            read(pipe_D_P[0], &pedido, sizeof(int));
            // Preparar papas fritas
            printf("PapaFritero: Preparando papas fritas\n");
            fflush(stdout);
            // Enviar papas fritas al Despachador
            write(pipe_P_D[1], &pedido, sizeof(int));
        }
    }

    pid_t pid_D = fork();
    if (pid_D == 0) { // Proceso Despachador
        
        close(pipe_D_H[0]);
        close(pipe_D_V[0]);
        close(pipe_D_P[0]);
        close(pipe_H_D[1]);
        close(pipe_V_D[1]);
        close(pipe_P_D[1]);

        while (1) {
            read(pipe_C_D[0], &pedido, sizeof(int));
            printf("Despachador: Recibiendo pedido ");
            fflush(stdout);

            if (pedido >= 1 && pedido <= 8) {
                // Atender pedidos comunes
                printf("común\n");
                pedidos_comunes(pedido);
            } 
            if (pedido >= 9 && pedido <= 15) {
                // Atender pedidos VIP
                printf("VIP\n");
                pedidos_vip(pedido);
            }
        }
    }

    // Tenemos NUM_CLIENT clientes: NUM_CLIENT procesos
    pid_t pid_C;
    for (int i = 0; i < NUM_CLIENTES; i++) {
        pid_C = fork();
        if (pid_C == 0) { // Proceso Cliente
            
            close(pipe_C_D[0]);
            close(pipe_D_C[1]);

            srand(time(NULL) + getpid() + getppid());
            int solicitud = rand() % 7; // decidir aleatoriamente que pedido hacer
        
            if (solicitud <= 8) {
                printf("Cliente Común %d realizando pedido\n", i+1);
                fflush(stdout);
            } else {
                printf("Cliente VIP %d realizando pedido\n", i+1);
                fflush(stdout);
            }
            // Enviar pedido al Despachador
            write(pipe_C_D[1], &solicitud, sizeof(int));
            // Esperar pedido del Despachador
            read(pipe_D_C[0], &solicitud, sizeof(int));
            printf("Cliente %d: Marchándome con el pedido\n", i+1);
            fflush(stdout);
            if (i == NUM_CLIENTES - 1) {
                sleep(1);
                printf("Ya no hay mas clientes\n");
                fflush(stdout);
                // enviar señal de terminacion al padre
            }
            exit(0);
        }
    }

    waitpid(pid_H, NULL, 0);
    waitpid(pid_V, NULL, 0);
    waitpid(pid_P1, NULL, 0);
    waitpid(pid_P2, NULL, 0);
    waitpid(pid_D, NULL, 0);
    
    for (int i = 0; i < NUM_CLIENTES; i++) {
        waitpid(pid_C, NULL, 0);
    }

    return 0;
}

void *pedidos_comunes(int pedido) {
    // Atender pedidos comunes
    switch (pedido) {
        case 1:
            printf("Despachador: Pedido de hamburguesa\n");
            fflush(stdout);
            write(pipe_D_H[1], &pedido, sizeof(int));

            read(pipe_H_D[0], &pedido, sizeof(int));

            write(pipe_D_C[1], &pedido, sizeof(int));
            break;
        case 2:
            printf("Despachador: Pedido de menú vegano\n");
            fflush(stdout);
            write(pipe_D_V[1], &pedido, sizeof(int));

            read(pipe_V_D[0], &pedido, sizeof(int));

            write(pipe_D_C[1], &pedido, sizeof(int));
            break;
        case 3:
            printf("Despachador: Pedido de papas fritas\n");
            fflush(stdout);
            write(pipe_D_P[1], &pedido, sizeof(int));

            read(pipe_P_D[0], &pedido, sizeof(int));

            write(pipe_D_C[1], &pedido, sizeof(int));
            break;
        case 4:
            printf("Despachador: Pedido de hamburguesa y menú vegano\n");
            fflush(stdout);
            write(pipe_D_H[1], &pedido, sizeof(int));
            write(pipe_D_V[1], &pedido, sizeof(int));

            read(pipe_H_D[0], &pedido, sizeof(int));
            read(pipe_V_D[0], &pedido, sizeof(int));

            write(pipe_D_C[1], &pedido, sizeof(int));
            break;
        case 5:
            printf("Despachador: Pedido de hamburguesa y papas fritas\n");
            fflush(stdout);
            write(pipe_D_H[1], &pedido, sizeof(int));
            write(pipe_D_P[1], &pedido, sizeof(int));

            read(pipe_H_D[0], &pedido, sizeof(int));
            read(pipe_P_D[0], &pedido, sizeof(int));

            write(pipe_D_C[1], &pedido, sizeof(int));
            break;
        case 6:
            printf("Despachador: Pedido de menú vegano y papas fritas\n");
            fflush(stdout);
            write(pipe_D_V[1], &pedido, sizeof(int));
            write(pipe_D_P[1], &pedido, sizeof(int));

            read(pipe_V_D[0], &pedido, sizeof(int));
            read(pipe_P_D[0], &pedido, sizeof(int));

            write(pipe_D_C[1], &pedido, sizeof(int));
            break;
        case 7:
            printf("Despachador: Pedido de hamburguesa, menú vegano y papas fritas\n");
            write(pipe_D_H[1], &pedido, sizeof(int));
            write(pipe_D_V[1], &pedido, sizeof(int));
            write(pipe_D_P[1], &pedido, sizeof(int));

            read(pipe_H_D[0], &pedido, sizeof(int));
            read(pipe_V_D[0], &pedido, sizeof(int));
            read(pipe_P_D[0], &pedido, sizeof(int));

            write(pipe_D_C[1], &pedido, sizeof(int));
            break;
    }
    return NULL;
}

void *pedidos_vip(int pedido) {
    // Atender pedidos VIP

    switch(pedido) {
        case 9:
            printf("Despachador: Pedido VIP de hamburguesa\n");
            fflush(stdout);
            write(pipe_D_H[1], &pedido, sizeof(int));

            read(pipe_H_D[0], &pedido, sizeof(int));

            write(pipe_D_C[1], &pedido, sizeof(int));
            break;
        case 10:
            printf("Despachador: Pedido VIP de menú vegano\n");
            fflush(stdout);
            write(pipe_D_V[1], &pedido, sizeof(int));

            read(pipe_V_D[0], &pedido, sizeof(int));

            write(pipe_D_C[1], &pedido, sizeof(int));
            break;
        case 11:
            printf("Despachador: Pedido VIP de papas fritas\n");
            fflush(stdout);
            write(pipe_D_P[1], &pedido, sizeof(int));

            read(pipe_P_D[0], &pedido, sizeof(int));

            write(pipe_D_C[1], &pedido, sizeof(int));
            break;
        case 12:
            printf("Despachador: Pedido VIP de hamburguesa y menú vegano\n");
            fflush(stdout);
            write(pipe_D_H[1], &pedido, sizeof(int));
            write(pipe_D_V[1], &pedido, sizeof(int));

            read(pipe_H_D[0], &pedido, sizeof(int));
            read(pipe_V_D[0], &pedido, sizeof(int));

            write(pipe_D_C[1], &pedido, sizeof(int));
            break;
        case 13:
            printf("Despachador: Pedido VIP de hamburguesa y papas fritas\n");
            fflush(stdout);
            write(pipe_D_H[1], &pedido, sizeof(int));
            write(pipe_D_P[1], &pedido, sizeof(int));

            read(pipe_H_D[0], &pedido, sizeof(int));
            read(pipe_P_D[0], &pedido, sizeof(int));

            write(pipe_D_C[1], &pedido, sizeof(int));
            break;
        case 14:
            printf("Despachador: Pedido VIP de menú vegano y papas fritas\n");
            fflush(stdout);
            write(pipe_D_V[1], &pedido, sizeof(int));
            write(pipe_D_P[1], &pedido, sizeof(int));

            read(pipe_V_D[0], &pedido, sizeof(int));
            read(pipe_P_D[0], &pedido, sizeof(int));

            write(pipe_D_C[1], &pedido, sizeof(int));
            break;
        case 15:
            printf("Despachador: Pedido VIP de hamburguesa, menú vegano y papas fritas\n");
            fflush(stdout);
            write(pipe_D_H[1], &pedido, sizeof(int));
            write(pipe_D_V[1], &pedido, sizeof(int));
            write(pipe_D_P[1], &pedido, sizeof(int));

            read(pipe_H_D[0], &pedido, sizeof(int));
            read(pipe_V_D[0], &pedido, sizeof(int));
            read(pipe_P_D[0], &pedido, sizeof(int));

            write(pipe_D_C[1], &pedido, sizeof(int));
            break;
    }
    return NULL;
}

/*
Se bloquea:
Cliente Común 1 realizando pedido
Cliente VIP 2 realizando pedido
Cliente VIP 4 realizando pedido
Cliente VIP 3 realizando pedido
Despachador: Recibiendo pedido común
Despachador: Pedido de papas fritas
PapaFritero: Preparando papas fritas
Cliente Común 5 realizando pedido
Cliente Común 6 realizando pedido
Despachador: Recibiendo pedido VIP
Despachador: Pedido VIP de hamburguesa, menú vegano y papas fritas
Cliente 1: Marchándome con el pedido
Cliente VIP 7 realizando pedido
Hamburguesero: Preparando hamburguesa
PapaFritero: Preparando papas fritas
Vegano: Preparando menú vegano
Cliente 2: Marchándome con el pedido
Despachador: Recibiendo pedido VIP
Despachador: Pedido VIP de hamburguesa
Hamburguesero: Preparando hamburguesa
Cliente 4: Marchándome con el pedido
Despachador: Recibiendo pedido VIP
Despachador: Pedido VIP de hamburguesa
Hamburguesero: Preparando hamburguesa
Despachador: Recibiendo pedido común
Despachador: Recibiendo pedido común
Despachador: Pedido de hamburguesa y papas fritas
Hamburguesero: Preparando hamburguesa
Cliente 3: Marchándome con el pedido
PapaFritero: Preparando papas fritas
Cliente 5: Marchándome con el pedido
Despachador: Recibiendo pedido VIP
Despachador: Pedido VIP de menú vegano
Vegano: Preparando menú vegano
Cliente 6: Marchándome con el pedido
 */