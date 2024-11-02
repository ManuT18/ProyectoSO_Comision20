/**
 * PumperInc.
 * Una cadena de comida rápida de la ciudad ofrece distintos tipos de combos a distintos tipos de clientes. 
 * Para la preparación de combos dispone de personal especializado. 
 * Un empleado prepara hamburguesas simples, otro prepara el menú apto vegano y para las papas fritas, que es lo más solicitado, se cuenta con dos empleados. 
 * Otro empleado recibe dos pedidos y despacha enseguida lo solicitado por cada cliente. 
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
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <pthread.h>

struct msg_buffer {
    long type;
    int pedido;
};

// el tipo 1 es para los pedidos de clientes VIP
// el tipo 2 es para los pedidos de clientes comunes
// el tipo 3 es para el despachador indicar a los cocineros de hamburguesas
// el tipo 4 es para el despachador indicar a los cocineros de menú vegano
// el tipo 5 es para el despachador indicar a los cocineros de papas fritas
// el tipo 6 es para que los cocineros le indiquen al despachador que el pedido está listo, y mediante el pedido, el despachador sabe cuál es el pedido
// el tipo 7 es para que el despachador le indique al cliente que su pedido de hamburguesas está listo
// el tipo 8 es para que el despachador le indique al cliente que su pedido de menú vegano está listo
// el tipo 9 es para que el despachador le indique al cliente que su pedido de papas fritas está listo

#define KEY 1234
#define ITERATIONS 100
#define NUM_CLIENTES_COMUNES 5
#define NUM_CLIENTES_VIP 4
#define MSG_SIZE sizeof(struct msg_buffer) - sizeof(long)

void* despachar_pedidos();

int main() {
    int queue = msgget(KEY, IPC_CREAT | 0666);
    struct msg_buffer msg;

    // proceso del empleado que prepara hamburguesas: lee de la cola mensajes de tipo 3 y escribe en la cola mensaje de tipo 6 con el pedido "1"
    pid_t pid_H;
    pid_H = fork();
    if (pid_H == 0) {
        for (int i = 0; i < ITERATIONS; i++) {
            msgrcv(queue, &msg, MSG_SIZE, 3, 0);
            sleep(1); // tiempo que tarda en preparar la hamburguesa
            msg.type = 6;
            msg.pedido = 1;
            msgsnd(queue, &msg, MSG_SIZE, 0);
        }
        exit(0);
    }

    // proceso del empleado que prepara menú vegano: lee de la cola mensajes de tipo 4 y escribe en la cola mensaje de tipo 6 con el pedido "2"
    pid_t pid_V;
    pid_V = fork();
    if (pid_V == 0) {
        int queue = msgget(KEY, 0666);
        for (int i = 0; i < ITERATIONS; i++) {
            msgrcv(queue, &msg, MSG_SIZE, 4, 0);
            sleep(1); // tiempo que tarda en preparar el menú vegano
            msg.type = 6;
            msg.pedido = 2;
            msgsnd(queue, &msg, MSG_SIZE, 0);
        }
        exit(0);
    }

    // proceso de los dos empleados que preparan papas fritas: leen de la cola mensajes de tipo 5 y escriben en la cola mensaje de tipo 6 con el pedido "3"
    pid_t pid_P;
    for (int i = 0; i < 2; i++) {
        pid_P = fork();
        if (pid_P == 0) {
            int queue = msgget(KEY, 0666);
            for (int i = 0; i < ITERATIONS; i++) {
                msgrcv(queue, &msg, MSG_SIZE, 5, 0);
                sleep(1); // tiempo que tarda en preparar las papas fritas
                msg.type = 6;
                msg.pedido = 3;
                msgsnd(queue, &msg, MSG_SIZE, 0);
            }
            exit(0);
        }
    }

    // proceso despachador: tiene dos hilos: uno que lee de la cola mensajes de tipo 1 y 2 y escribe en la cola mensajes de tipo 3, 4 y 5, y otro que lee de la cola mensajes de tipo 6 y escribe en la cola mensajes de tipo 7, 8 y 9
    pid_t pid_D;
    pid_D = fork();
    if (pid_D == 0) {
        int queue = msgget(KEY, 0666);
        pthread_t hilo_despachar_pedidos;
        pthread_create(&hilo_despachar_pedidos, NULL, despachar_pedidos, NULL);

        while (1) {
            // primero chequeo todos los pedidos de los clientes VIP
            while (msgrcv(queue, &msg, MSG_SIZE, 1, IPC_NOWAIT) != -1) {
                printf("Despachador: Pedido VIP de tipo %d\n", msg.pedido);
                switch (msg.pedido) {
                    case 1:
                        msg.type = 3;
                        break;
                    case 2:
                        msg.type = 4;
                        break;
                    case 3:
                        msg.type = 5;
                        break;
                }
                msgsnd(queue, &msg, MSG_SIZE, 0);
            }

            if (msgrcv(queue, &msg, MSG_SIZE, 2, IPC_NOWAIT) != -1) {
                printf("Despachador: Pedido común de tipo %d\n", msg.pedido);
                switch (msg.pedido) {
                    case 1:
                        msg.type = 3;
                        break;
                    case 2:
                        msg.type = 4;
                        break;
                    case 3:
                        msg.type = 5;
                        break;
                }
                msgsnd(queue, &msg, MSG_SIZE, 0);
            }
        }

        pthread_join(hilo_despachar_pedidos, NULL);
        exit(0);
    }

    // proceso cliente VIP: escribe en la cola mensaje de tipo 1 y lee de la cola mensaje de tipo 7, 8 y 9 dependiendo su pedido.
    pid_t pid_CV;
    for (int i = 0; i < NUM_CLIENTES_VIP; i++) {
        pid_CV = fork();
        if (pid_CV == 0) {
            sleep(3);
            int queue = msgget(KEY, 0666);
            for (int i = 0; i < ITERATIONS; i++) {
                sleep(1);
                srand(time(NULL)*i + getpid()/(i+1));
                msg.pedido = rand() % 3 + 1;
                msg.type = 1;
                printf("Cliente VIP: Pedido %d\n", msg.pedido);
                msgsnd(queue, &msg, MSG_SIZE, 0);
                switch (msg.pedido) {
                    case 1:
                        msgrcv(queue, &msg, MSG_SIZE, 7, 0);
                        break;
                    case 2:
                        msgrcv(queue, &msg, MSG_SIZE, 8, 0);
                        break;
                    case 3:
                        msgrcv(queue, &msg, MSG_SIZE, 9, 0);
                        break;
                }
            }
            exit(0);
        }
    }

    // proceso cliente común: escribe en la cola mensaje de tipo 2 y lee de la cola mensaje de tipo 7, 8 y 9 dependiendo su pedido.
    pid_t pid_CC;
    for (int i = 0; i < NUM_CLIENTES_COMUNES; i++) {
        pid_CC = fork();
        if (pid_CC == 0) {
            int queue = msgget(KEY, 0666);
            for (int i = 0; i < ITERATIONS; i++) {
                sleep(1);
                srand(time(NULL)/(i+1) + getpid()*i);
                msg.pedido = rand() % 3 + 1;
                msg.type = 2;
                printf("Cliente común: Pedido %d\n", msg.pedido);
                msgsnd(queue, &msg, MSG_SIZE, 0);
                switch (msg.pedido) {
                    case 1:
                        msgrcv(queue, &msg, MSG_SIZE, 7, 0);
                        break;
                    case 2:
                        msgrcv(queue, &msg, MSG_SIZE, 8, 0);
                        break;
                    case 3:
                        msgrcv(queue, &msg, MSG_SIZE, 9, 0);
                        break;
                }
            }
            exit(0);
        }
    }

    waitpid(pid_H, NULL, 0);
    waitpid(pid_V, NULL, 0);
    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }
    waitpid(pid_D, NULL, 0);
    for (int i = 0; i < NUM_CLIENTES_VIP; i++) {
        wait(NULL);
    }
    for (int i = 0; i < NUM_CLIENTES_COMUNES; i++) {
        wait(NULL);
    }

    return 0;
}

void *despachar_pedidos() {
    int queue = msgget(KEY, 0666);
    struct msg_buffer msg;

    while (1) {
        msgrcv(queue, &msg, MSG_SIZE, 6, 0);
        switch (msg.pedido) {
            case 1:
                msg.type = 7;
                break;
            case 2:
                msg.type = 8;
                break;
            case 3:
                msg.type = 9;
                break;
        }
        printf("Despachador: Pedido listo de tipo %d\n", msg.pedido);
        msgsnd(queue, &msg, MSG_SIZE, 0);
    }

    return NULL;
}