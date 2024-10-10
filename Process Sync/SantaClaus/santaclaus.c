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

sem_t sem_renos, sem_elfos, sem_santaVolvio;
pthread_mutex_t mutex_reno, mutex_elfo, mutex_santa;
pthread_t threads[3];

void *process_llegaReno()
{
    while (1)
    {
        sem_wait(&sem_santaVolvio);

        // Llegan renos
        for (int i = 0; i < 9; i++)
        {
            // Inicializa el generador de números aleatorios
            srand(time(NULL));

            // Genera un número aleatorio entre 0 y 2000 milisegundos
            int wait_time = rand() % 2001;

            // Espera el tiempo generado
            usleep(wait_time * 1000);

            sem_post(&sem_renos);

            printf("Llega reno %i\n", i + 1);
            fflush(stdout);
        }

        pthread_mutex_lock(&mutex_reno);
    }

    return NULL;
}

void *process_prepararTrineo()
{
    while (1)
    {
        if(pthread_mutex_trylock(&mutex_reno) == 0){
            printf("Santa Claus: Preparando trineo\n");
            fflush(stdout);
        }
        
    }

    return NULL;
}

void *process_elfoPideAyuda()
{
    while (1)
    {
        // Inicializa el generador de números aleatorios
        srand(time(NULL));

        // Genera un número aleatorio entre 5 y 7 segundos
        int wait_time = 5000 + (rand() % 2001);

        // Espera el tiempo generado
        usleep(wait_time * 1000);

        sem_post(&sem_elfos);

        int i = 1;
        printf("Elfo %i tiene problema\n", i);
        fflush(stdout);
        i++;
    }

    return NULL;
}

void *process_santaClaus()
{
    while (1)
    {
        // Espera a que lleguen los 9 renos, cuando llega el ultimo despierta a Santa Claus
        for (int i = 0; i < 9; i++)
        {
            sem_wait(&sem_renos);
        }

        // Espera de 1 segundo
        sleep(1);

        pthread_mutex_lock(&mutex_santa);
        printf("\nSanta Claus: Despierta\n");
        fflush(stdout);

        printf("\nSanta Claus se fue con sus renos, volverá en 5\n");
        fflush(stdout);
        sleep(1);
        for (int i = 4; i > 0; i--)
        {
            printf("                                             %i\n", i);
            fflush(stdout);
            sleep(1);
        }
        printf("\nSanta Claus volvió al Polo Norte!\n\n");
        fflush(stdout);
        sem_post(&sem_santaVolvio);
    }

    return NULL;
}

int main(int argc, char const *argv[])
{
    sem_init(&sem_renos, 0, 0);
    sem_init(&sem_elfos, 0, 0);
    sem_init(&sem_santaVolvio, 0, 1);
    pthread_mutex_init(&mutex_reno, NULL);
    pthread_mutex_init(&mutex_elfo, NULL);
    pthread_mutex_init(&mutex_santa, NULL);

    pthread_create(&threads[0], NULL, process_llegaReno, NULL);
    pthread_create(&threads[3], NULL, process_prepararTrineo, NULL);
    pthread_create(&threads[1], NULL, process_elfoPideAyuda, NULL);
    //pthread_create(&threads[2], NULL, process_ayudarElfos, NULL);
    pthread_create(&threads[2], NULL, process_santaClaus, NULL);

    for (int i = 0; i < 3; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("\n");
    printf("\n");
    fflush(stdout);

    return 0;
}