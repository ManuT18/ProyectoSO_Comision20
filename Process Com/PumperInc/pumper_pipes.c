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
 *  - Dos procesos para los empleados que preparan papas fritas (P),
 *  - Un proceso para el empleado despachador (D), que va a tener dos hilos: uno para atender los pedidos, y otro para entregarlos.
 *  - Un pipe para la comunicación entre los clientes comunes y el despachador.
 *  - Un pipe para la comunicación entre los clientes VIP y el despachador.
 *  - Tres pipes para la comunicación entre el despachador y los cocineros.
 *  - Tres pipes para la comunicación entre los cocineros y el despachador.
 *  - Tres pipes para la comunicación entre el despachador y los clientes.
 *
 * Un cliente llega a donde el despachador, le indica su pedido y espera a que el despachador le indique que su pedido está listo.
 * El despachador va a tener dos hilos: uno es para atender los pedidos que vengan de un cliente común o VIP, y otro hilo para entregarlos. Es importante que el despachador tenga ambos hilos dado que si un cliente llega y hace un pedido, el despachador debe atenderlo, indicarle a los cocineros que preparen el pedido, y luego seguir atendiendo otros clientes, mientras que en el otro hilo estará esperando a que los cocineros le entreguen el pedido.
 * El que un cliente sea común o VIP se modelará con que los VIPS hagan su pedido en un pipe distinto al de los comunes. De esta manera, el despachador deberá primero chequear el pipe VIP antes que el pipe común. De resto, los códigos son los mismos
 *
 * El código para los pedidos será el siguiente:
 *      - 1: solo hamburguesa
 *      - 2: solo menú vegano
 *      - 3: solo papas fritas
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
#include <fcntl.h>

void *despachar_pedidos();

int pedido;

int pipe_CN_D[2]; // pipe para la comunicación entre Cliente Normal (CN) y Despachador (D)
int pipe_CV_D[2]; // pipe para la comunicación entre Cliente VIP (CV) y Despachador (D)
int pipe_D_H[2]; // pipe para la comunicación entre Despachador (D) y Hamburguesero (H)
int pipe_D_V[2]; // pipe para la comunicación entre Despachador (D) y el cocinero del Menú Vegano (V)
int pipe_D_P[2]; // pipe para la comunicación entre Despachador (D) y los PapaFriteros (P)
int pipe_devolucion[2]; // pipe para la comunicación entre los cocineros y Despachador (D)
int pipe_D_C_H[2]; // pipe para la comunicación entre Despachador (D) y los clientes para hamburguesas
int pipe_D_C_V[2]; // pipe para la comunicación entre Despachador (D) y los clientes para menú vegano
int pipe_D_C_P[2]; // pipe para la comunicación entre Despachador (D) y los clientes para papas fritas

int main() {
    pipe(pipe_CN_D);
    pipe(pipe_CV_D);
    pipe(pipe_D_H);
    pipe(pipe_D_V);
    pipe(pipe_D_P);
    pipe(pipe_D_C_H);
    pipe(pipe_D_C_V);
    pipe(pipe_D_C_P);
    pipe(pipe_devolucion);

    fcntl(pipe_CN_D[0], F_SETFL, O_NONBLOCK);
    fcntl(pipe_CV_D[0], F_SETFL, O_NONBLOCK);

    // proceso del empleado que prepara hamburguesas: lee de pipe_D_H[0] y escribe en pipe_H_D[1]
    pid_t pid_H;
    pid_H = fork();
    if (pid_H == 0) {
        close(pipe_CN_D[0]);
        close(pipe_CN_D[1]);
        close(pipe_CV_D[0]);
        close(pipe_CV_D[1]);
        close(pipe_D_H[1]);
        close(pipe_D_V[0]);
        close(pipe_D_V[1]);
        close(pipe_D_P[0]);
        close(pipe_D_P[1]);
        close(pipe_D_C_H[0]);
        close(pipe_D_C_H[1]);
        close(pipe_D_C_V[0]);
        close(pipe_D_C_V[1]);
        close(pipe_D_C_P[0]);
        close(pipe_D_C_P[1]);
        close(pipe_devolucion[0]);

        for (int i = 0; i < REPETITIONS; i++) {
            read(pipe_D_H[0], &pedido, sizeof(int));
            sleep(1); // tiempo que tarda en preparar la hamburguesa
            printf("Hamburguesa: Pedido listo\n");
            write(pipe_devolucion[1], &pedido, sizeof(int));
        }
        close(pipe_D_H[0]);
        close(pipe_devolucion[1]);
        exit(0);
    }

    // proceso del empleado que prepara menú vegano: lee de pipe_D_V[0] y escribe en pipe_V_D[1]
    pid_t pid_V;
    pid_V = fork();
    if (pid_V == 0) {
        close(pipe_CN_D[0]);
        close(pipe_CN_D[1]);
        close(pipe_CV_D[0]);
        close(pipe_CV_D[1]);
        close(pipe_D_H[0]);
        close(pipe_D_H[1]);
        close(pipe_D_V[1]);
        close(pipe_D_P[0]);
        close(pipe_D_P[1]);
        close(pipe_D_C_H[0]);
        close(pipe_D_C_H[1]);
        close(pipe_D_C_V[0]);
        close(pipe_D_C_V[1]);
        close(pipe_D_C_P[0]);
        close(pipe_D_C_P[1]);
        close(pipe_devolucion[0]);

        for (int i = 0; i < REPETITIONS; i++) {
            read(pipe_D_V[0], &pedido, sizeof(int));
            sleep(1); // tiempo que tarda en preparar el menú vegano
            printf("Menu Vegano: Pedido listo\n");
            write(pipe_devolucion[1], &pedido, sizeof(int));
        }
        close(pipe_D_V[0]);
        close(pipe_devolucion[1]);
        exit(0);
    }

    // procesos de los dos empleados que preparan papas fritas: lee de pipe_D_P[0] y escribe en pipe_P_D[1]
    pid_t pid_P;
    for (int i = 0; i < 2; i++) {
        pid_P = fork();
        if (pid_P == 0) {
            close(pipe_CN_D[0]);
            close(pipe_CN_D[1]);
            close(pipe_CV_D[0]);
            close(pipe_CV_D[1]);
            close(pipe_D_H[0]);
            close(pipe_D_H[1]);
            close(pipe_D_V[0]);
            close(pipe_D_V[1]);
            close(pipe_D_P[1]);
            close(pipe_D_C_H[0]);
            close(pipe_D_C_H[1]);
            close(pipe_D_C_V[0]);
            close(pipe_D_C_V[1]);
            close(pipe_D_C_P[0]);
            close(pipe_D_C_P[1]);
            close(pipe_devolucion[0]);

            for (int i = 0; i < REPETITIONS; i++) {
                read(pipe_D_P[0], &pedido, sizeof(int));
                sleep(1); // tiempo que tarda en preparar las papas fritas
                printf("Papas Fritas: Pedido listo\n");
                write(pipe_devolucion[1], &pedido, sizeof(int));
            }
            close(pipe_D_P[0]);
            close(pipe_devolucion[1]);
            exit(0);
        }
    }

    // proceso despachador:
    // con un hilo (el principal) lee en forma no bloqueante de pipe_CN_D[0] y pipe_CV_D[0], y escribe en pipe_D_H[1], pipe_D_V[1], pipe_D_P[1] dependiendo su pedido.
    // con otro hilo (creado) espera por los pedidos de los cocineros en pipe_H_D[0], pipe_V_D[0], pipe_P_D[0] y escribe en pipe_D_C_H[1], pipe_D_C_V[1], pipe_D_C_P[1] para entregar el pedido a los clientes que estén esperando allí.
    pid_t pid_D;
    pid_D = fork();
    if (pid_D == 0) {
        close(pipe_D_H[0]);
        close(pipe_D_V[0]);
        close(pipe_D_P[0]);
        close(pipe_D_C_H[0]);
        close(pipe_D_C_V[0]);
        close(pipe_D_C_P[0]);

        pthread_t hilo_despachar_pedidos;
        pthread_create(&hilo_despachar_pedidos, NULL, despachar_pedidos, NULL);

        while (1) {
            while (read(pipe_CV_D[0], &pedido, sizeof(int)) > 0) {
                sleep(1);
                // leer de los pipes de los clientes (normal y vip) en forma no bloqueante y verificar si hay pedidos para informar
                switch (pedido) {
                    case 1:
                        printf("Despachador: Pedido de hamburguesa\n");
                        write(pipe_D_H[1], &pedido, sizeof(int));
                        break;
                    case 2:
                        printf("Despachador: Pedido de menú vegano\n");
                        write(pipe_D_V[1], &pedido, sizeof(int));
                        break;
                    case 3:
                        printf("Despachador: Pedido de papas fritas\n");
                        write(pipe_D_P[1], &pedido, sizeof(int));
                        break;
                }
            }

            if (read(pipe_CN_D[0], &pedido, sizeof(int)) > 0) {
                sleep(1);
                switch (pedido) {
                    case 1:
                        printf("Despachador: Pedido de hamburguesa\n");
                        write(pipe_D_H[1], &pedido, sizeof(int));
                        break;
                    case 2:
                        printf("Despachador: Pedido de menú vegano\n");
                        write(pipe_D_V[1], &pedido, sizeof(int));
                        break;
                    case 3:
                        printf("Despachador: Pedido de papas fritas\n");
                        write(pipe_D_P[1], &pedido, sizeof(int));
                        break;
                }
            }
        }

        pthread_join(hilo_despachar_pedidos, NULL);
        close(pipe_CN_D[0]);
        close(pipe_CN_D[1]);
        close(pipe_CV_D[0]);
        close(pipe_CV_D[1]);
        close(pipe_D_H[1]);
        close(pipe_D_V[1]);
        close(pipe_D_P[1]);
        close(pipe_D_C_H[1]);
        close(pipe_D_C_V[1]);
        close(pipe_D_C_P[1]);
        close(pipe_devolucion[0]);
        close(pipe_devolucion[1]);
        exit(0);
    }

    // proceso cliente normal: escribe en pipe_CN_D[1] y lee de pipe_D_CN_H[0], pipe_D_CN_V[0], pipe_D_CN_P[0] dependiendo su pedido.
    pid_t pid_CN;
    for (int i = 0; i < NUM_CLIENTES; i++) {
        pid_CN = fork();
        if (pid_CN == 0) {
            close(pipe_CN_D[0]);
            close(pipe_CV_D[0]);
            close(pipe_CV_D[1]);
            close(pipe_D_H[0]);
            close(pipe_D_H[1]);
            close(pipe_D_V[0]);
            close(pipe_D_V[1]);
            close(pipe_D_P[0]);
            close(pipe_D_P[1]);
            close(pipe_D_C_H[1]);
            close(pipe_D_C_V[1]);
            close(pipe_D_C_P[1]);
            close(pipe_devolucion[0]);
            close(pipe_devolucion[1]);

            for (int i = 0; i < 4; i++) {
                sleep(1);
                srand(time(NULL)/(i+1) + getpid()*i);
                pedido = rand() % 3 + 1;
                write(pipe_CN_D[1], &pedido, sizeof(int));
                switch (pedido) {
                    case 1:
                        printf("Cliente Normal: Pedido de hamburguesa\n");
                        read(pipe_D_C_H[0], &pedido, sizeof(int));
                        break;
                    case 2:
                        printf("Cliente Normal: Pedido de menú vegano\n");
                        read(pipe_D_C_V[0], &pedido, sizeof(int));
                        break;
                    case 3:
                        printf("Cliente Normal: Pedido de papas fritas\n");
                        read(pipe_D_C_P[0], &pedido, sizeof(int));
                        break;
                }
            }
            close(pipe_CN_D[1]);
            close(pipe_D_C_H[0]);
            close(pipe_D_C_V[0]);
            close(pipe_D_C_P[0]);
            exit(0);
        }
    }

    // proceso cliente VIP: escribe en pipe_CV_D[1] y lee de pipe_D_CV_H[0], pipe_D_CV_V[0], pipe_D_CV_P[0] dependiendo su pedido.
    pid_t pid_CV;
    for (int i = 0; i < NUM_CLIENTES; i++) {
        pid_CV = fork();
        if (pid_CV == 0) {
            close(pipe_CN_D[0]);
            close(pipe_CN_D[1]);
            close(pipe_CV_D[0]);
            close(pipe_D_H[0]);
            close(pipe_D_H[1]);
            close(pipe_D_V[0]);
            close(pipe_D_V[1]);
            close(pipe_D_P[0]);
            close(pipe_D_P[1]);
            close(pipe_D_C_H[1]);
            close(pipe_D_C_V[1]);
            close(pipe_D_C_P[1]);
            close(pipe_devolucion[0]);
            close(pipe_devolucion[1]);

            for (int i = 0; i < 4; i++) {
                sleep(1);
                srand(time(NULL)*i + getpid()/(i+1));
                pedido = rand() % 3 + 1;
                write(pipe_CV_D[1], &pedido, sizeof(int));
                switch (pedido) {
                    case 1:
                        printf("Cliente VIP: Pedido de hamburguesa\n");
                        read(pipe_D_C_H[0], &pedido, sizeof(int));
                        break;
                    case 2:
                        printf("Cliente VIP: Pedido de menú vegano\n");
                        read(pipe_D_C_V[0], &pedido, sizeof(int));
                        break;
                    case 3:
                        printf("Cliente VIP: Pedido de papas fritas\n");
                        read(pipe_D_C_P[0], &pedido, sizeof(int));
                        break;
                }
            }
            close(pipe_CV_D[1]);
            close(pipe_D_C_H[0]);
            close(pipe_D_C_V[0]);
            close(pipe_D_C_P[0]);
            exit(0);
        }
    }

    close(pipe_CN_D[0]);
    close(pipe_CN_D[1]);
    close(pipe_CV_D[0]);
    close(pipe_CV_D[1]);
    close(pipe_D_H[0]);
    close(pipe_D_H[1]);
    close(pipe_D_V[0]);
    close(pipe_D_V[1]);
    close(pipe_D_P[0]);
    close(pipe_D_P[1]);
    close(pipe_D_C_H[0]);
    close(pipe_D_C_H[1]);
    close(pipe_D_C_V[0]);
    close(pipe_D_C_V[1]);
    close(pipe_D_C_P[0]);
    close(pipe_D_C_P[1]);
    close(pipe_devolucion[0]);
    close(pipe_devolucion[1]);

    waitpid(pid_H, NULL, 0);
    waitpid(pid_V, NULL, 0);
    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }
    waitpid(pid_D, NULL, 0);
    for (int i = 0; i < NUM_CLIENTES; i++) {
        wait(NULL);
        wait(NULL);
    }

    return 0;
}

void *despachar_pedidos() {
    int pedido = 0;
    for (int i = 0; i < REPETITIONS; i++) {
        // leer del pipe de los cocineros y escribir en los pipes de los clientes
        read(pipe_devolucion[0], &pedido, sizeof(int));
        switch (pedido) {
            case 1:
                write(pipe_D_C_H[1], &pedido, sizeof(int));
                break;
            case 2:
                write(pipe_D_C_V[1], &pedido, sizeof(int));
                break;
            case 3:
                write(pipe_D_C_P[1], &pedido, sizeof(int));
                break;
        }
    }

    return NULL;
}