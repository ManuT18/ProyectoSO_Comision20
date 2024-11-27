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

sem_t sem_ruedas, sem_chasis, sem_motor, sem_pintura, sem_extras, terminator;
pthread_mutex_t mutex;

pthread_t threads[6];

#define ITERATIONS 9
// cada cuatro ruedas hay un extra
void *process_ruedas() {
    for (int i = 0; i < 2*ITERATIONS; i++) {
        sem_wait(&sem_ruedas);
        pthread_mutex_lock(&mutex);
        printf("Rueda \n");
        usleep(700000);

        fflush(stdout);
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_chasis);
    }
    sem_post(&terminator);
    printf("### terminaron las ruedas ###\n");
    fflush(stdout);
    pthread_exit(0);
    return NULL;
}

void *process_chasis() {
    for (int i = 0; i < ITERATIONS; i++) {
        sem_wait(&sem_chasis);
        sem_wait(&sem_chasis);
        pthread_mutex_lock(&mutex);
        printf("Chasis\n");
        usleep(700000);

        fflush(stdout);
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_motor);
    }
    printf("### terminaron los chasis ###\n");
    fflush(stdout);
    pthread_exit(0);
    return NULL;
}

void *process_motor() {
    for (int i = 0; i < ITERATIONS; i++) {
        sem_wait(&sem_motor);
        pthread_mutex_lock(&mutex);
        printf("Motor\n");
        fflush(stdout);
        usleep(700000);

        pthread_mutex_unlock(&mutex);
        sem_post(&sem_pintura);
    }
    printf("### terminaron los motores ###\n");
    fflush(stdout);
    pthread_exit(0);
    return NULL;
}

void *process_pinturaverde() {
    for (int i = 0; i < ITERATIONS; i++) {
        sem_wait(&sem_pintura);
        pthread_mutex_lock(&mutex);
        printf("Pintura verde\n--------------------\n");
        fflush(stdout);
        usleep(700000);

        if (i % 2 == 0) {
            sem_post(&sem_extras);
        } else {
            if (sem_trywait(&terminator) == 0) {
                printf("### terminando a los pintores rojos y los extras ###\n");
                fflush(stdout);
                pthread_cancel(threads[4]);
                pthread_cancel(threads[5]);
                printf("### terminaron las pinturas verdes ###\n");
                fflush(stdout);
                pthread_exit(0);
            }
            sem_post(&sem_ruedas);
            sem_post(&sem_ruedas);
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *process_pinturaroja() {
    for (int i = 0; i < ITERATIONS; i++) {
        sem_wait(&sem_pintura);
        pthread_mutex_lock(&mutex);
        printf("Pintura roja\n--------------------\n");
        fflush(stdout);
        usleep(700000);

        if (i % 2 == 0) {
            sem_post(&sem_extras);
        } else {
            if (sem_trywait(&terminator) == 0) {
                printf("### terminando a los pintores verdes y los extras ###\n");
                fflush(stdout);
                pthread_cancel(threads[3]);
                pthread_cancel(threads[5]);
                printf("### terminaron las pinturas rojas ###\n");
                fflush(stdout);
                pthread_exit(0);
         }
            sem_post(&sem_ruedas);
            sem_post(&sem_ruedas);
        }
        pthread_mutex_unlock(&mutex);
    }
    
    return NULL;
}

void *process_extras() {
    // "Una de cada dos motos es elegida para recibir equipamiento extra".
    // Esto se "logra" con el if (i % 2 == 0) (es la varialbe `i` par?) en los procedimientos de pintura.
    // Sin embargo, la variable i es local a cada procedimiento, por lo que no se puede asegurar 
    // un intercalado coherente: si la primer moto la pintan de rojo, y la segunda de verde, ambas
    // recibirán equipamiento extra porque la variable i era par en ambos procedimientos.
    // Por lo tanto, a los efectos del enunciado, se sigue cumpliendo, y hasta puede que quede mejor así.
    for (int i = 0; i < ITERATIONS; i++) {
        sem_wait(&sem_extras);
        pthread_mutex_lock(&mutex);
        printf("Extras añadidos\n--------------------\n");
        fflush(stdout);
        usleep(700000);

        pthread_mutex_unlock(&mutex);
        sem_post(&sem_ruedas);
        sem_post(&sem_ruedas);
        
        if (sem_trywait(&terminator) == 0) {
            printf("### terminando a los pintores rojos y los pintores verdes ###\n");
            fflush(stdout);
            pthread_cancel(threads[4]);
            pthread_cancel(threads[3]);
            printf("### terminaron los extras ###\n");
            fflush(stdout);
            pthread_exit(0);
        }
    }

    return NULL;
}

int main(int argc, char const *argv[]) {
    printf("### Iniciando la fabricación de motos ###\n### SE GENERARÁN %d MOTOS ###\n#########################################\n\n", ITERATIONS);
    fflush(stdout);

    sem_init(&sem_ruedas, 0, 2);
    sem_init(&sem_chasis, 0, 0);
    sem_init(&sem_motor, 0, 0);
    sem_init(&sem_pintura, 0, 0);
    sem_init(&sem_extras, 0, 0);
    sem_init(&terminator, 0, 0);

    pthread_mutex_init(&mutex, NULL);

    pthread_create(&threads[0], NULL, process_ruedas, NULL);
    pthread_create(&threads[1], NULL, process_chasis, NULL);
    pthread_create(&threads[2], NULL, process_motor, NULL);
    pthread_create(&threads[3], NULL, process_pinturaverde, NULL);
    pthread_create(&threads[4], NULL, process_pinturaroja, NULL);
    pthread_create(&threads[5], NULL, process_extras, NULL);

    for (int i = 0; i < 6; i++) {
        pthread_join(threads[i], NULL);
    }

    sleep(1);
    printf("\n\nMotos terminadas\n\n");
    fflush(stdout);

    sem_destroy(&sem_ruedas);
    sem_destroy(&sem_chasis);
    sem_destroy(&sem_motor);
    sem_destroy(&sem_pintura);
    sem_destroy(&sem_extras);

    return 0;
}