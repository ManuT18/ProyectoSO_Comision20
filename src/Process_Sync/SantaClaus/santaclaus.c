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

#define REPETITIONS 1
#define CANT_RENOS 9
#define CANT_ELFOS 30

sem_t sem_santa;        // para que Santa se despierte. Tiene un comportamiento preferiblemente binario (ver la función actividad_santa para entender por qué no es exactamente binario).
sem_t sem_santa_volvio; // para indicar que Santa volvió de entregar los regalos. Es binario
sem_t sem_puerta_elfos; // para que los elfos nuevos esperen atrás de una puerta a que Santa termine de ayudar a los elfos viejos (simula una puerta que se cierra cuando hay 3 elfos adentro). Es binario
sem_t sem_renos;        // indica la cantidad total de renos 
sem_t sem_elfos_ayuda;        // indica la cantidad de elfos máxima que pueden pedir ayuda a Santa

pthread_t threads_renos[CANT_RENOS];
pthread_t threads_elfos[CANT_ELFOS];
pthread_t thread_santa;

void *actividad_santa() {
    for (int i = 0; i < REPETITIONS; i++) {
        sem_wait(&sem_santa);
        printf("\nSanta Claus: Despierta\n");

        // si están los 9 renos esperando...
        if (sem_trywait(&sem_renos) == -1) {
            printf("\nSanta Claus: Preparando trineo");
            fflush(stdout);
            sleep(0.5);

            printf("\nSanta Claus: Trineo listo");
            fflush(stdout);
            sleep(0.5);
            printf("\nSanta Claus: Entregando regalos\n\nSanta volverá en un momento\n");
            fflush(stdout);
            
            for (int i = 5; i > 0; i--) {
                printf(". ");
                fflush(stdout);
                sleep(0.5);
            }

            printf("\n\nSanta Claus: Volvió\n\n");
            sleep(0.5);

            // si aparecieron elfos mientras estaba viajando, hicieron un signal de más en sem_santa. Debo consumirlo, total los voy a atender cuando vuelva
            if (sem_trywait(&sem_elfos_ayuda) == -1) {
                sem_wait(&sem_santa);
            } else {
                sem_post(&sem_elfos_ayuda);
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
        if (sem_trywait((&sem_elfos_ayuda)) == -1) {
            printf("\nSanta Claus: Ayudando a los elfos\n");
            fflush(stdout);
            sleep(0.5);

            for (int i = 0; i < 3; i++) {
                sem_post(&sem_elfos_ayuda);
                printf("\nSanta Claus: Ayudando al %i° elfo \n", i + 1);
                fflush(stdout);
                sleep(0.5);
            }

            printf("\nSanta Claus: Terminé de ayudar a los elfos\n");
            fflush(stdout);
            sleep(0.5);

            sem_post(&sem_puerta_elfos);
        } else {
            sem_post(&sem_elfos_ayuda);
        }

        printf("\nSanta Claus: Volviendo a dormir\n\n");
        fflush(stdout);
        sleep(0.5);
    }

    printf("\nSanta Claus se ha jubilado, ya no trabajará\n\n");
    // avisar a los renos y a los elfos que dejen de existir
    /*
    for (int i = 0; i < CANT_RENOS; i++) {
        pthread_cancel(threads_renos[i]);
    }
    for (int i = 0; i < CANT_ELFOS; i++) {
        pthread_cancel(threads_elfos[i]);
    }
    */

    return NULL;
}

void *actividad_reno(void *id_reno) {
    sem_wait(&sem_renos);
    printf("\nReno %i volvió de vacaciones\n", (int)id_reno);
    fflush(stdout);
    sleep(0.5);

    if (sem_trywait(&sem_renos) == -1) {
        printf("\nYa somos 9 renos, vamos a acudir a Santa\n");
        fflush(stdout);
        sem_post(&sem_santa);
    } else {
        sem_post(&sem_renos);
    }

    sem_wait(&sem_santa_volvio);

    return NULL;
}

void *actividad_elfo(void *id_elfo) {

    sem_wait(&sem_elfos_ayuda);
    printf("\nElfo %i tiene problema\n", (int)id_elfo);
    fflush(stdout);
    sleep(0.5);

    if (sem_trywait(&sem_elfos_ayuda) == -1) {
        printf("\nYa somos 3 elfos, vamos a acudir a Santa\n");
        fflush(stdout);
        sem_post(&sem_santa);
    } else {
        sem_post(&sem_elfos_ayuda);
    }

    return NULL;
}

int main(int argc, char const *argv[]) {
    sem_init(&sem_santa, 0, 0);
    sem_init(&sem_santa_volvio, 0, 1);
    sem_init(&sem_renos, 0, 9);
    sem_init(&sem_elfos_ayuda, 0, 3);
    sem_init(&sem_puerta_elfos, 0, 1);

    for (int i = 0; i < CANT_RENOS; i++) {
        pthread_create(&threads_renos[i], NULL, actividad_reno, (i+1));
    }
    for (int i = 0; i < CANT_ELFOS; i++) {
        pthread_create(&threads_elfos[i], NULL, actividad_elfo, (i+1));
    }
    pthread_create(&thread_santa, NULL, actividad_santa, NULL);

    for (int i = 0; i < CANT_RENOS; i++) {
        pthread_join(threads_renos[i], NULL);
    }
    for (int i = 0; i < CANT_ELFOS; i++) {
        pthread_join(threads_elfos[i], NULL);
    }
    pthread_join(thread_santa, NULL);

    sem_destroy(&sem_renos);
    sem_destroy(&sem_elfos_ayuda);
    sem_destroy(&sem_santa);
    sem_destroy(&sem_santa_volvio);
    sem_destroy(&sem_puerta_elfos);

    printf("\n\nFin del programa, gracias por su atencion!\nAtte.: Comision 20 :)\n");

    return 0;
}
