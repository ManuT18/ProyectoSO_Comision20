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

#define REPETITIONS 100

/**
 * Santa Claus duerme en su tienda en el Polo Norte y sólo puede ser despertado por:
 *  - los nueve renos, una vez que regresan de sus vacaciones en el Pacífico Sur,
 *  - algunos de los elfos que están teniendo dificultades para hacer juguetes.
 * Para permitir que Santa descanse lo mejor posible, los elfos solo pueden despertarlo cuando tres de ellos tienen problemas.
 * Cuando estos tres elfos están resolviendo sus problemas, cualquier otro elfo que quiera visitar a Santa debe esperar a que estos elfos terminen.
 * En el momento que el último reno arriba al Polo Norte, este debe ir a buscar a Santa mientras los demás esperan en una cálida cabaña antes de ser enganchados al trineo.
 * Si Santa se despierta y encuentra a tres elfos esperando en la puerta de su tienda, junto con el último reno que ha regresado de los trópicos, atiende primero a los renos, ya que es más importante preparar su trineo.
 * 
 * (a) Resolver este problema utilizando hilos y semáforos para su sincronización.
 * (b) Explique brevemente el modelo implementado.
 */

sem_t sem_renos;
sem_t sem_elfos;
sem_t sem_santa;
sem_t sem_santaVolvio;
sem_t sem_puerta_elfos; // Para que los elfos esperen a que Santa termine de ayudar a los elfos (simula una puerta). Es binario
pthread_mutex_t mutAyuda;
pthread_t threads[3];

void *thread_santa() {
    int cant_renos;
    int cant_elfos;

    for (int i = 0; i < REPETITIONS; i++) {
        sem_wait(&sem_santa);
        printf("\nSanta Claus: Despierta\n");
        sleep(2);

        sem_getvalue(&sem_renos, &cant_renos);

        if (cant_renos == 9) {
            pthread_mutex_lock(&mutAyuda);
            
            printf("\nSanta Claus: Preparando trineo");
            fflush(stdout);
            sleep(2);
            for (int i = 0; i < 9; i++) {
                sem_wait(&sem_renos);
            }

            printf("\nSanta Claus: Trineo listo");
            fflush(stdout);
            printf("\nSanta Claus: Entregando regalos\nSanta volverá en 5 segundos\n");
            fflush(stdout);
            for (int i = 5; i > 0; i--) {
                printf(". ");
                fflush(stdout);
                sleep(1);
            }
            printf("\n");
            printf("Santa Claus: Volvió\n");
            printf("\n");
            fflush(stdout);

            sem_post(&sem_santaVolvio);

            pthread_mutex_unlock(&mutAyuda);
        }

        sem_getvalue(&sem_elfos, &cant_elfos);

        if (cant_elfos >= 3 && cant_renos == 9) {
            sem_wait(&sem_santa); // consumir el signal que se hizo de más porque llegaron elfos en mitad del viaje
        }

        if (cant_elfos >= 3) {
            pthread_mutex_lock(&mutAyuda);
            printf("\nSanta Claus: Ayudando a los elfos\n");
            sleep(2);
            
            for (int i = 0; i < 3; i++) {
                sem_wait(&sem_elfos);
                printf("Santa Claus: Ayudando al %i° elfo \n", i + 1);
                sleep(1);
            }
            sem_post(&sem_puerta_elfos);
            pthread_mutex_unlock(&mutAyuda);
        }

        printf("\nSanta Claus: Volviendo a dormir\n\n");
        fflush(stdout);
    }

    return NULL;
}

void *thread_reno() {
    for (int i = 0; i < REPETITIONS; i++) {
        sem_wait(&sem_santaVolvio);
        sleep(10);

        for (int i = 0; i < 9; i++) {
            sem_post(&sem_renos);
            printf("Llega reno %i\n", i + 1);
            fflush(stdout);
            sleep(2);
        }

        sem_post(&sem_santa);
    }

    return NULL;
}

void *thread_elfo() {
    int cant_elfos;

    for (int i = 0; i < REPETITIONS; i++) {
        sem_wait(&sem_puerta_elfos);

        for (int j = 0; j < 3; j++) {
            sleep(3);
            sem_post(&sem_elfos);

            sem_getvalue(&sem_elfos, &cant_elfos);

            printf("\nElfo %i tiene problema\n\n", j + 1);
            fflush(stdout);
        }

        printf("Ya somos 3 elfos, vamos a acudir a Santa\n");
        fflush(stdout);
        sem_post(&sem_santa);
    }

    return NULL;
}

int main(int argc, char const *argv[]) {
    sem_init(&sem_renos, 0, 0);
    sem_init(&sem_elfos, 0, 0);
    sem_init(&sem_santa, 0, 0);
    sem_init(&sem_santaVolvio, 0, 1);
    sem_init(&sem_puerta_elfos, 0, 1);
    pthread_mutex_init(&mutAyuda, NULL);

    pthread_create(&threads[0], NULL, thread_reno, NULL);
    pthread_create(&threads[1], NULL, thread_elfo, NULL);
    pthread_create(&threads[2], NULL, thread_santa, NULL);

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&sem_renos);
    sem_destroy(&sem_elfos);
    sem_destroy(&sem_santa);
    sem_destroy(&sem_santaVolvio);
    pthread_mutex_destroy(&mutAyuda);

    return 0;
}
