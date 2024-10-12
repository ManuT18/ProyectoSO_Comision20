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

sem_t sem_renos, sem_elfos, sem_santa, sem_santaVolvio;
pthread_mutex_t mutAyuda;
pthread_t threads[3];

void *process_santa()
{
    for (int i = 0; i < 100; i++)
    {
        sem_wait(&sem_santa);
        printf("\nSanta Claus: Despierta\n");
        sleep(2);

        int cantrenos;
        sem_getvalue(&sem_renos, &cantrenos);
        int cantelfos;
        sem_getvalue(&sem_elfos, &cantelfos);

        if (cantrenos >= 9)
        {
            pthread_mutex_lock(&mutAyuda);

            printf("\nSanta Claus: Preparando trineo");
            sleep(2);
            for (int i = 0; i < 9; i++)
            {
                sem_wait(&sem_renos);
            }

            printf("\nSanta Claus: Trineo listo");
            fflush(stdout);
            printf("\nSanta Claus: Entregando regalos\nSanta volverá en 5 segundos\n");
            fflush(stdout);
            for (int i = 5; i > 0; i--)
            {
                printf(". ");
                fflush(stdout);
                sleep(1);
            }
            printf("\n\n");
            fflush(stdout);

            sem_post(&sem_santaVolvio);

            pthread_mutex_unlock(&mutAyuda);
        }
        else if (cantelfos >= 3)
        {
            pthread_mutex_lock(&mutAyuda);

            printf("\nSanta Claus: Ayudando a los elfos\n");
            sleep(2);
            for (int i = 0; i < 3; i++)
            {
                sem_wait(&sem_elfos);
                printf("Santa Claus: Ayudando al %i° elfo \n", i + 1);
                sleep(1);
            }

            pthread_mutex_unlock(&mutAyuda);
        }

        printf("\nSanta Claus: Volviendo a dormir\n\n");
        fflush(stdout);
    }
}

void *process_reno()
{
    for (int i = 0; i < 100; i++)
    {
        sem_wait(&sem_santaVolvio);
        for (int i = 0; i < 9; i++)
        {
            sem_post(&sem_renos);
            printf("Llega reno %i\n", i + 1);
            fflush(stdout);
            sleep(2);
        }

        sem_post(&sem_santa);
    }

    return NULL;
}

void *process_elfo()
{
    for (int i = 0; i < 100; i++)
    {
        sleep(10);
        sem_post(&sem_elfos);
        printf("\nElfo %i tiene problema\n\n", i + 1);
        fflush(stdout);

        int cantelfos;
        sem_getvalue(&sem_elfos, &cantelfos);
        if (cantelfos >= 3)
        {
            sem_post(&sem_santa);
        }
    }

    return NULL;
}

int main(int argc, char const *argv[])
{
    sem_init(&sem_renos, 0, 0);
    sem_init(&sem_elfos, 0, 0);
    sem_init(&sem_santa, 0, 0);
    sem_init(&sem_santaVolvio, 0, 1);
    pthread_mutex_init(&mutAyuda, NULL);

    pthread_create(&threads[0], NULL, process_reno, NULL);
    pthread_create(&threads[1], NULL, process_elfo, NULL);
    pthread_create(&threads[2], NULL, process_santa, NULL);

    for (int i = 0; i < 3; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}