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

sem_t sem_renos, sem_elfos;
pthread_t threads[3];

void *process_llegaReno() {
    return NULL;
}

void *process_elfo() {
    return NULL;
}

void *process_santaClaus() {
    return NULL;
}

int main(int argc, char const *argv[]) {
    sem_init(&sem_renos, 0, 0);
    sem_init(&sem_elfos, 0, 0);

    pthread_create(&threads[0], NULL, process_llegaReno, NULL);
    pthread_create(&threads[1], NULL, process_elfo, NULL);
    pthread_create(&threads[2], NULL, process_santaClaus, NULL);

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n");
    printf("\n");
    fflush(stdout);

    return 0;
}