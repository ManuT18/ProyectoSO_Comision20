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

#define TIMES_RENOS_COMEBACK 3 // cuántas veces vacacionan los renos
#define CANT_RENOS 9
#define CANT_ELFOS 28 // si no es multiplo de 3, van a quedar elfos sin atender

sem_t sem_santa; // para que Santa se despierte. Tiene un comportamiento preferiblemente binario (ver la función actividad_santa para entender por qué no es exactamente binario).
sem_t sem_renos_grupo; // indica la cantidad máxima de renos y sirve para sincronizar los grupos de nueve renos con los que Santa va a atender, así como para identificar al último reno encargado de despertarlo
sem_t sem_max_renos; // indica la cantidad máxima de renos y sirve para que los renos que se Santa libera no acudan a él inmediatamente después
sem_t sem_elfos_grupo; // indica la cantidad máxima de elfos y sirve para sincronizar los grupos de tres elfos con los que Santa va a atender, así como para identificar al último elfo del grupo encargado de despertar a Santa.
sem_t sem_max_elfos; // indica la cantidad máxima de elfos y sirve para que otros elfos no quieran acudir a Santa cuando ya haya un grupo con él

pthread_t threads_renos[CANT_RENOS];
pthread_t threads_elfos[CANT_ELFOS];
pthread_t thread_santa;
pthread_mutex_t mutex_renos;
pthread_mutex_t mutex_elfos;

void *actividad_santa() {
    while(1) {
        sem_wait(&sem_santa);
        printf("\nSanta Claus: Despierta\n\n");

        // si están los 9 renos esperando...
        if (sem_trywait(&sem_renos_grupo) == -1) {
            printf("Santa Claus: Tengo renos esperándome\n");
            printf("Santa Claus: Preparando trineo\n");
            printf("Santa Claus: Trineo listo\n");
            printf("Santa Claus: Entregando regalos\n");
            printf("Santa Claus: Volverá en un momento\n");
            for (int i = 5; i > 0; i--) {
                sleep(1);
                printf(". ");
                fflush(stdout);
            }
            printf("\n\nSanta Claus: Volvió\n\n");

            // si aparecieron elfos mientras estaba viajando, hicieron un signal de más en sem_santa. Debo consumirlo, total los voy a atender cuando vuelva.
            // Nota: el enunciado no aclara que los elfos no puedan acudir a Santa mientras éste esté viajando
            if (sem_trywait(&sem_elfos_grupo) == -1) {
                sem_wait(&sem_santa);
            } else {
                sem_post(&sem_elfos_grupo);
            }

            // libero a los nueve renos
            for (int i = 0; i < CANT_RENOS; i++) {
                sem_post(&sem_renos_grupo);
            }

            for (int i = 0; i < CANT_RENOS; i++) {
                sem_post(&sem_max_renos);
            }

        } else {
            printf("Santa Claus: Me desperté y no hay renos esperándome\n");
            sem_post(&sem_renos_grupo);
        }

        // si hay 3 elfos esperando...
        if (sem_trywait((&sem_elfos_grupo)) == -1) {
            printf("Santa Claus: Tengo elfos esperándome\n");
            printf("Santa Claus: Ayudando a los elfos\n");

            for (int i = 0; i < 3; i++) {
                printf("\nSanta Claus: Ayudando al %i° elfo \n", i + 1);
                sleep(1);
                sem_post(&sem_elfos_grupo);
            }

            printf("\nSanta Claus: Terminé de ayudar a los elfos\n");

            for (int i = 0; i < 3; i++) {
                sem_post(&sem_max_elfos);
            }
        } else {
            printf("Santa Claus: No hay elfos esperándome\n");
            sem_post(&sem_elfos_grupo);
        }

        printf("\nSanta Claus: Volviendo a dormir\n\n");
    }

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

    sem_wait(&sem_max_renos);

    pthread_mutex_lock(&mutex_renos);

    printf("\nReno %i volvió de vacaciones\n", (int)id_reno);
    sleep(1);
    sem_wait(&sem_renos_grupo);

    // si soy el último reno de un grupo de nueve, acudimos a Santa
    if (sem_trywait(&sem_renos_grupo) == -1) {
        printf("\nYa somos 9 renos, vamos a acudir a Santa\n");
        sem_post(&sem_santa);
    } else {
        sem_post(&sem_renos_grupo);
    }

    pthread_mutex_unlock(&mutex_renos);

    return NULL;
}

void *actividad_elfo(void *args) {
    int id_elfo = (int)(size_t)args;
    id_elfo++;

    sem_wait(&sem_max_elfos);

    pthread_mutex_lock(&mutex_elfos);
    printf("\nElfo %i necesita ayuda\n", (int)id_elfo);
    sleep(1);

    sem_wait(&sem_elfos_grupo);
    // si soy el último elfo de un grupo de tres, acudimos a Santa
    if (sem_trywait(&sem_elfos_grupo) == -1) {
        printf("\nYa somos 3 elfos, vamos a acudir a Santa\n");
        sem_post(&sem_santa);
    } else {
        sem_post(&sem_elfos_grupo);
    }

    pthread_mutex_unlock(&mutex_elfos);

    return NULL;
}

int main(int argc, char const *argv[]) {
    sem_init(&sem_santa, 0, 0);
    sem_init(&sem_max_renos, 0, 9);
    sem_init(&sem_elfos_grupo, 0, 3);
    sem_init(&sem_max_elfos, 0, 3);
    sem_init(&sem_renos_grupo, 0, 9);

    pthread_mutex_init(&mutex_renos, NULL);
    pthread_mutex_init(&mutex_elfos, NULL);

    for (int i = 0; i < CANT_RENOS; i++) {
        pthread_create(&threads_renos[i], NULL, actividad_reno, (void *)(size_t)i);
    }

    for (int i = 0; i < CANT_ELFOS; i++) {
        pthread_create(&threads_elfos[i], NULL, actividad_elfo, (void *)(size_t)i);
    }

    pthread_create(&thread_santa, NULL, actividad_santa, NULL);

    for (int i = 0; i < TIMES_RENOS_COMEBACK; i++) {
        for (int j = 0; j < CANT_RENOS; j++) {
            pthread_join(threads_renos[j], NULL);
            pthread_create(&threads_renos[j], NULL, actividad_reno, (void *)(size_t)j);
        }
    }

    for (int i = 0; i < CANT_RENOS; i++) {
        pthread_join(threads_renos[i], NULL);
    }

    for (int i = 0; i < CANT_ELFOS; i++) {
        pthread_join(threads_elfos[i], NULL);
    }

    pthread_join(thread_santa, NULL);

    sem_destroy(&sem_max_renos);
    sem_destroy(&sem_elfos_grupo);
    sem_destroy(&sem_santa);
    sem_destroy(&sem_elfos_grupo);
    sem_destroy(&sem_max_elfos);
    sem_destroy(&sem_renos_grupo);

    pthread_mutex_destroy(&mutex_renos);
    pthread_mutex_destroy(&mutex_elfos);

    printf("\n\nFin del programa, gracias por su atencion!\nAtte.: Comision 20 :)\n");

    return 0;
}
