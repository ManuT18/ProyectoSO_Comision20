#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>

#define REPETITIONS 3

sem_t sem_santa;        // para que Santa se despierte. Tiene un comportamiento preferiblemente binario (ver la función thread_santa para entender por qué no es exactamente binario).
sem_t sem_santa_volvio; // para indicar que Santa volvió de entregar los regalos. Es binario
sem_t sem_puerta_elfos; // para que los elfos nuevos esperen atrás de una puerta a que Santa termine de ayudar a los elfos viejos (simula una puerta que se cierra cuando hay 3 elfos adentro). Es binario
sem_t sem_renos;        // indica la cantidad de renos
sem_t sem_elfos;        // indica la cantidad de elfos

pthread_t threads[3];

void *thread_santa() {
    for (int i = 0; i < REPETITIONS; i++) {
        sem_wait(&sem_santa);
        printf("\nSanta Claus: Despierta\n");

        // si están los 9 renos esperando...
        if (sem_trywait(&sem_renos) == -1) {
            printf("\nSanta Claus: Preparando trineo");
            fflush(stdout);

            printf("\nSanta Claus: Trineo listo");
            printf("\nSanta Claus: Entregando regalos\n\nSanta volverá en un momento\n");

            for (int i = 5; i > 0; i--) {
                printf(". ");
                fflush(stdout);
            }

            printf("\n\nSanta Claus: Volvió\n\n");

            // si aparecieron elfos mientras estaba viajando, hicieron un signal de más en sem_santa. Debo consumirlo, total los voy a atender cuando vuelva
            if (sem_trywait(&sem_elfos) == -1) {
                sem_wait(&sem_santa);
            } else {
                sem_post(&sem_elfos);
            }

            // libero a los nueve renos
            for (int i = 0; i < 9; i++) {
                sem_post(&sem_renos);
            }

            sem_post(&sem_santa_volvio);
        } else {
            sem_post(&sem_renos);
        }

        // si hay 3 elfos esperando...
        if (sem_trywait((&sem_elfos)) == -1) {
            printf("\nSanta Claus: Ayudando a los elfos\n");

            for (int i = 0; i < 3; i++) {
                sem_post(&sem_elfos);
                printf("\nSanta Claus: Ayudando al %i° elfo \n", i + 1);
            }

            printf("\nSanta Claus: Terminé de ayudar a los elfos\n");

            sem_post(&sem_puerta_elfos);
        } else {
            sem_post(&sem_elfos);
        }

        printf("\nSanta Claus: Volviendo a dormir\n\n");
        fflush(stdout);
    }

    printf("\nSanta Claus se ha jubilado, ya no trabajará\n\n");
    // avisar a los renos y a los elfos que dejen de existir
    pthread_cancel(threads[0]);
    pthread_cancel(threads[1]);

    return NULL;
}

void *thread_reno() {
    for (int i = 0; i < REPETITIONS; i++) {
        sem_wait(&sem_santa_volvio);

        for (int i = 0; i < 9; i++) {
            sem_wait(&sem_renos);
            printf("\nLlega reno %i\n", i + 1);
            fflush(stdout);
        }

        sem_post(&sem_santa);
    }

    return NULL;
}

void *thread_elfo() {
    for (int i = 0; i < REPETITIONS; i++) {
        sem_wait(&sem_puerta_elfos);

        for (int j = 0; j < 3; j++) {
            sem_wait(&sem_elfos);

            printf("\nElfo %i tiene problema\n", j + 1);
        }

        printf("\nYa somos 3 elfos, vamos a acudir a Santa\n");
        fflush(stdout);
        sem_post(&sem_santa);
    }

    return NULL;
}

int main(int argc, char const *argv[]) {
    sem_init(&sem_santa, 0, 0);
    sem_init(&sem_santa_volvio, 0, 1);
    sem_init(&sem_renos, 0, 9);
    sem_init(&sem_elfos, 0, 3);
    sem_init(&sem_puerta_elfos, 0, 1);

    pthread_create(&threads[0], NULL, thread_reno, NULL);
    pthread_create(&threads[1], NULL, thread_elfo, NULL);
    pthread_create(&threads[2], NULL, thread_santa, NULL);

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&sem_renos);
    sem_destroy(&sem_elfos);
    sem_destroy(&sem_santa);
    sem_destroy(&sem_santa_volvio);
    sem_destroy(&sem_puerta_elfos);

    printf("\n\nFin del programa, gracias por su atencion!\nAtte.: Comision 20 :)\n");

    return 0;
}

/**
 * Elfo 1 tiene problema

Llega reno 1

Llega reno 2

Llega reno 3

Llega reno 4

Llega reno 5

Llega reno 6

Llega reno 7

Llega reno 8

Llega reno 9

Elfo 2 tiene problema

Elfo 3 tiene problema

Santa Claus: Despierta

Santa Claus: Preparando trineo
Santa Claus: Trineo listo
Santa Claus: Entregando regalos

Santa volverá en un momento
. . . . . 

Santa Claus: Volvió


Ya somos 3 elfos, vamos a acudir a Santa

Santa Claus: Ayudando a los elfos

Santa Claus: Ayudando al 1° elfo 

Santa Claus: Ayudando al 2° elfo 

Santa Claus: Ayudando al 3° elfo 

Santa Claus: Terminé de ayudar a los elfos

Santa Claus: Volviendo a dormir


Llega reno 1

Llega reno 2

Llega reno 3

Llega reno 4

Llega reno 5

Llega reno 6

Llega reno 7

Llega reno 8

Llega reno 9

Elfo 1 tiene problema

Elfo 2 tiene problema

Elfo 3 tiene problema

Ya somos 3 elfos, vamos a acudir a Santa

Santa Claus: Despierta

Santa Claus: Preparando trineo
Santa Claus: Trineo listo
Santa Claus: Entregando regalos

Santa volverá en un momento
. . . . . 

Santa Claus: Volvió


Santa Claus: Ayudando a los elfos

Santa Claus: Ayudando al 1° elfo 

Santa Claus: Ayudando al 2° elfo 

Santa Claus: Ayudando al 3° elfo 

Santa Claus: Terminé de ayudar a los elfos

Elfo 1 tiene problema

Elfo 2 tiene problema

Elfo 3 tiene problema

Ya somos 3 elfos, vamos a acudir a Santa

Llega reno 1

Llega reno 2

Llega reno 3

Llega reno 4

Llega reno 5

Llega reno 6

Llega reno 7

Llega reno 8

Llega reno 9

Santa Claus: Volviendo a dormir


Santa Claus: Despierta

Santa Claus: Preparando trineo
Santa Claus: Trineo listo
Santa Claus: Entregando regalos

Santa volverá en un momento
. . . . . 

Santa Claus: Volvió


Santa Claus: Ayudando a los elfos

Santa Claus: Ayudando al 1° elfo 

Santa Claus: Ayudando al 2° elfo 

Santa Claus: Ayudando al 3° elfo 

Santa Claus: Terminé de ayudar a los elfos

Santa Claus: Volviendo a dormir


 */