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

#define REPETITIONS 60
#define CANT_RENOS 9
#define CANT_ELFOS 13

sem_t sem_santa; // para que Santa se despierte. Tiene un comportamiento preferiblemente binario (ver la función actividad_santa para entender por qué no es exactamente binario).
sem_t sem_santa_volvio; // para indicar que Santa volvió de entregar los regalos. Es binario
sem_t sem_puerta_elfos; // para que los elfos nuevos esperen atrás de una puerta a que Santa termine de ayudar a los elfos viejos (simula una puerta que se cierra cuando hay 3 elfos adentro). Es binario
sem_t sem_maxima_renos; // indica la cantidad máxima de renos 
sem_t sem_elfos_ayuda; // indica la cantidad de elfos máxima que pueden pedir ayuda a Santa
sem_t sem_elfos_grupo; // indica la cantidad corriente de elfos
sem_t sem_renos; // indica la cantidad corriente de renos
sem_t sem_renos_pueden_entrar; // para indicar cuándo pueden volver los nueve renos

pthread_t threads_renos[CANT_RENOS];
pthread_t threads_elfos[CANT_ELFOS];
pthread_t thread_santa;
pthread_mutex_t mutex_renos;
pthread_mutex_t mutex_elfos;

void *actividad_santa() {
    for (int i = 0; i < REPETITIONS; i++) {
        sem_wait(&sem_santa);
        printf("\nSanta Claus: Despierta\n");

        // si están los 9 renos esperando...
        if (sem_trywait(&sem_maxima_renos) == -1) {
            printf("Santa Claus: Preparando trineo\n");

            printf("Santa Claus: Trineo listo\n");
            printf("Santa Claus: Entregando regalos\n");
            printf("Santa Claus: Volverá en un momento\n");
            
            for (int i = 5; i > 0; i--) {
                printf(". ");
                fflush(stdout);
            }

            printf("\n\nSanta Claus: Volvió\n\n");

            // si aparecieron elfos mientras estaba viajando, hicieron un signal de más en sem_santa. Debo consumirlo, total los voy a atender cuando vuelva
            if (sem_trywait(&sem_elfos_ayuda) == -1) {
                sem_wait(&sem_santa);
            } else {
                sem_post(&sem_elfos_ayuda);
            }

            // libero a los nueve renos
            for (int i = 0; i < 9; i++) {
                sem_wait(&sem_renos);
                sem_post(&sem_maxima_renos);
            }
            sem_post(&sem_renos_pueden_entrar);

            sem_post(&sem_santa_volvio);
        } else {
            sem_post(&sem_maxima_renos);
        }

        // si hay 3 elfos esperando...
        if (sem_trywait((&sem_elfos_ayuda)) == -1) {
            printf("\nSanta Claus: Ayudando a los elfos\n");

            for (int i = 0; i < 3; i++) {
                sem_post(&sem_elfos_ayuda);
                printf("\nSanta Claus: Ayudando al %i° elfo \n", i + 1);
            }

            printf("\nSanta Claus: Terminé de ayudar a los elfos\n");

            sem_post(&sem_puerta_elfos);
        } else {
            sem_post(&sem_elfos_ayuda);
        }

        printf("\nSanta Claus: Volviendo a dormir\n\n");
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

void *actividad_reno(void *args) {
    int id_reno = (int)(size_t)args;
    id_reno++;

    pthread_mutex_lock(&mutex_renos);

    // el primer reno es el que le da el paso a los demás
    // hasta que santa no diga que los renos pueden entrar, no entra ninguno
    if (sem_trywait(&sem_renos) == -1) {
        sem_wait(&sem_renos_pueden_entrar);
    } else {
        sem_post(&sem_renos);
    }
    sem_post(&sem_renos);
    
    printf("\nReno %i volvió de vacaciones\n", (int)id_reno);

    sem_wait(&sem_maxima_renos);
    // si soy el último reno en llegar, despierto a Santa
    if (sem_trywait(&sem_maxima_renos) == -1) {
        printf("\nYa somos 9 renos, vamos a acudir a Santa\n");
        sem_post(&sem_santa);
    } else {
        sem_post(&sem_maxima_renos);
    }

    pthread_mutex_unlock(&mutex_renos);

    return NULL;
}

void *actividad_elfo(void *args) {
    int id_elfo = (int)(size_t)args;
    id_elfo++;

    pthread_mutex_lock(&mutex_elfos);

    // si ya hay elfos pidiendo ayuda a santa, debo esperar a que terminen
    if (sem_trywait(&sem_puerta_elfos) == -1) {
        
    } else {
        sem_post(&sem_puerta_elfos);
    }

    sem_wait(&sem_elfos_ayuda);
    printf("\nElfo %i necesita ayuda\n", (int)id_elfo);
    // si soy el último elfo de un grupo de tres, acudimos a Santa
    if (sem_trywait(&sem_elfos_ayuda) == -1) {
        printf("\nYa somos 3 elfos, vamos a acudir a Santa\n");
        sem_wait(&sem_puerta_elfos);
        sem_post(&sem_santa);
    } else {
        sem_post(&sem_elfos_ayuda);
    }

    pthread_mutex_unlock(&mutex_elfos);

    return NULL;
}

int main(int argc, char const *argv[]) {
    sem_init(&sem_santa, 0, 0);
    sem_init(&sem_santa_volvio, 0, 1);
    sem_init(&sem_maxima_renos, 0, 9);
    sem_init(&sem_renos, 0, 0);
    sem_init(&sem_elfos_ayuda, 0, 3);
    sem_init(&sem_puerta_elfos, 0, 1);
    sem_init(&sem_renos_pueden_entrar, 0, 1);
    sem_init(&sem_elfos_grupo, 0, 0);

    pthread_mutex_init(&mutex_renos, NULL);
    pthread_mutex_init(&mutex_elfos, NULL);

    for (int i = 0; i < CANT_RENOS; i++) {
        pthread_create(&threads_renos[i], NULL, actividad_reno, (void *)(size_t)i);
    }
    for (int i = 0; i < CANT_ELFOS; i++) {
        pthread_create(&threads_elfos[i], NULL, actividad_elfo, (void *)(size_t)i);
    }
    pthread_create(&thread_santa, NULL, actividad_santa, NULL);

    for (int i = 0; i < CANT_RENOS; i++) {
        pthread_join(threads_renos[i], NULL);
    }
    for (int i = 0; i < CANT_ELFOS; i++) {
        pthread_join(threads_elfos[i], NULL);
    }
    pthread_join(thread_santa, NULL);

    sem_destroy(&sem_maxima_renos);
    sem_destroy(&sem_elfos_ayuda);
    sem_destroy(&sem_santa);
    sem_destroy(&sem_santa_volvio);
    sem_destroy(&sem_puerta_elfos);
    sem_destroy(&sem_renos);
    sem_destroy(&sem_renos_pueden_entrar);
    sem_destroy(&sem_elfos_grupo);

    pthread_mutex_destroy(&mutex_renos);
    pthread_mutex_destroy(&mutex_elfos);

    printf("\n\nFin del programa, gracias por su atencion!\nAtte.: Comision 20 :)\n");

    return 0;
}
