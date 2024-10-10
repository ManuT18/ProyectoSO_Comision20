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

sem_t sem_ruedas, sem_chasis, sem_motor, sem_pintura, sem_extras;
pthread_t threads[6];

//
#define ITERATIONS 7

void *process_ruedas()
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        sem_wait(&sem_ruedas);
        printf("Rueda \n");
        fflush(stdout);
        sem_post(&sem_chasis);

        printf("Rueda \n");
        fflush(stdout);
        sem_post(&sem_chasis);
    }
    return NULL;
}

void *process_chasis()
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        sem_wait(&sem_chasis);
        sem_wait(&sem_chasis);
        printf("Chasis\n");
        fflush(stdout);
        sem_post(&sem_motor);
    }
    return NULL;
}

void *process_motor()
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        sem_wait(&sem_motor);
        printf("Motor\n");
        fflush(stdout);
        sem_post(&sem_pintura);
    }
    return NULL;
}

void *process_pinturaverde()
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        sem_wait(&sem_pintura);
        printf("Pintura verde\n--------------------\n");
        fflush(stdout);
        sem_post(&sem_extras);
    }
    return NULL;
}

void *process_pinturaroja()
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        sem_wait(&sem_pintura);
        printf("Pintura roja\n--------------------\n");
        fflush(stdout);
        sem_post(&sem_extras);
    }
    return NULL;
}

void *process_extras()
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        sem_wait(&sem_extras);
        sem_post(&sem_ruedas);
        sem_wait(&sem_extras);
        printf("Extras añadidos\n--------------------\n");
        fflush(stdout);
        sem_post(&sem_ruedas);
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    sem_init(&sem_ruedas, 0, 1);
    sem_init(&sem_chasis, 0, 0);
    sem_init(&sem_motor, 0, 0);
    sem_init(&sem_pintura, 0, 0);
    sem_init(&sem_extras, 0, 0);

    pthread_create(&threads[0], NULL, process_ruedas, NULL);
    pthread_create(&threads[1], NULL, process_chasis, NULL);
    pthread_create(&threads[2], NULL, process_motor, NULL);
    pthread_create(&threads[3], NULL, process_pinturaverde, NULL);
    pthread_create(&threads[4], NULL, process_pinturaroja, NULL);
    pthread_create(&threads[5], NULL, process_extras, NULL);

    for (int i = 0; i < 6; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("\n");
    printf("\n");
    fflush(stdout);

    sem_destroy(&sem_ruedas);
    sem_destroy(&sem_chasis);
    sem_destroy(&sem_motor);
    sem_destroy(&sem_pintura);
    sem_destroy(&sem_extras);

    return 0;
}

/**
 * Se bloquea:
 * 
 * Rueda 
Rueda 
Chasis
Motor
Pintura verde
--------------------
Rueda 
Rueda 
Chasis
Motor
Pintura verde
--------------------
Extras añadidos
--------------------
Rueda 
Rueda 
Chasis
Motor
Pintura roja
--------------------
Rueda 
Rueda 
Chasis
Motor
Pintura verde
--------------------
Extras añadidos
--------------------
Rueda 
Rueda 
Chasis
Motor
Pintura roja
--------------------
Rueda 
Rueda 
Chasis
Motor
Pintura verde
--------------------
Extras añadidos
--------------------
Rueda 
Rueda 
Chasis
Motor
Pintura roja
--------------------
 */