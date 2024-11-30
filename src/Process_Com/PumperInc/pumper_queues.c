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
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <signal.h>

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
#define NUM_CLIENTES_COMUNES 13
#define NUM_CLIENTES_VIP 2
#define MSG_SIZE sizeof(struct msg_buffer) - sizeof(long)

void* despachar_pedidos();

int main() {
    int queue = msgget(KEY, IPC_CREAT | 0666);
    struct msg_buffer msg;

    // proceso del empleado que prepara hamburguesas: lee de la cola mensajes de tipo 3 y escribe en la cola mensaje de tipo 6 con el pedido "1"
    pid_t pid_H;
    pid_H = fork();
    if (pid_H == 0) {
        while (1) {
            msgrcv(queue, &msg, MSG_SIZE, 3, 0);
            msg.type = 6;
            msg.pedido = 1;
            printf("Hamburguesa: Pedido listo\n");
            msgsnd(queue, &msg, MSG_SIZE, 0);
        }
        exit(0);
    }

    // proceso del empleado que prepara menú vegano: lee de la cola mensajes de tipo 4 y escribe en la cola mensaje de tipo 6 con el pedido "2"
    pid_t pid_V;
    pid_V = fork();
    if (pid_V == 0) {
        int queue = msgget(KEY, 0666);
        while (1) {
            msgrcv(queue, &msg, MSG_SIZE, 4, 0);
            msg.type = 6;
            msg.pedido = 2;
            printf("Menú Vegano: Pedido listo\n");
            msgsnd(queue, &msg, MSG_SIZE, 0);
        }
        exit(0);
    }

    // proceso de los dos empleados que preparan papas fritas: leen de la cola mensajes de tipo 5 y escriben en la cola mensaje de tipo 6 con el pedido "3"
    pid_t pid_P;
    pid_t pid_P2;
    for (int i = 0; i < 2; i++) {
        pid_P = fork();
        i == 2 ? pid_P2 = pid_P : 0;
        if (pid_P == 0) {
            int queue = msgget(KEY, 0666);
            while (1) {
                msgrcv(queue, &msg, MSG_SIZE, 5, 0);
                msg.type = 6;
                msg.pedido = 3;
                printf("Papas Fritas: Pedido listo\n");
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

        // la lectura accede al mensaje cuyo tipo sea <= |-2| y a la vez sea el menor de todos.
        // es decir, si hay mensajes de tipo 1 y 2, accede primero a los de tipo 1 y luego a los de tipo 2.
        // si solo hay mensajes de tipo 2, los accederá de igual forma ya que cumplen la condición.
        while (msgrcv(queue, &msg, MSG_SIZE, -2, 0) != -1) {
            if (msg.type == 1) {
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

            if (msg.type == 2) {
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
            int queue = msgget(KEY, 0666);
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
            printf("Cliente VIP: Marchándome contento\n");
            exit(0);
        }
    }

    // proceso cliente común: escribe en la cola mensaje de tipo 2 y lee de la cola mensaje de tipo 7, 8 y 9 dependiendo su pedido.
    pid_t pid_CC;
    for (int i = 0; i < NUM_CLIENTES_COMUNES; i++) {
        pid_CC = fork();
        if (pid_CC == 0) {
            int queue = msgget(KEY, 0666);
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
            printf("Cliente común: Marchándome contento\n");
            exit(0);
        }
    }
    
    for (int i = 0; i < NUM_CLIENTES_VIP; i++) {
        wait(NULL);
    }
    for (int i = 0; i < NUM_CLIENTES_COMUNES; i++) {
        wait(NULL);
    }

    // borrar la cola de mensajes ahora que no hay más clientes que escriban en ella.
    msgctl(queue, IPC_RMID, NULL);

    // forzar la interrupción del programa una vez que los clientes se hayan marchado, matando a todos los hijos y despues al padre.
    // el único propósito de esto es para que el programa no se quede bloqueado por culpa de los bucles infinitos de los empleados, y de esa forma, traben la ejecución del makefile.
    kill(pid_H, SIGTERM);
    kill(pid_V, SIGTERM);
    kill(pid_P, SIGTERM);
    kill(pid_P2, SIGTERM);
    kill(pid_D, SIGTERM);

    waitpid(pid_H, NULL, 0);
    waitpid(pid_V, NULL, 0);
    waitpid(pid_P, NULL, 0);
    waitpid(pid_P2, NULL, 0);
    waitpid(pid_D, NULL, 0);

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