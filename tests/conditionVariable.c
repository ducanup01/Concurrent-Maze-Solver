#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

pthread_mutex_t mutexFuel;
pthread_cond_t condFuel;

int fuel = 0;

void* fuel_filling(void* args) {
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&mutexFuel);
        fuel += 15;
        printf("Filled fuel... %d\n", fuel);
        pthread_mutex_unlock(&mutexFuel);
        pthread_cond_signal(&condFuel);
        sleep(1);
    }
}

void* car(void* args) {
    // printf("Here to get fuel\n");
    pthread_mutex_lock(&mutexFuel);
    while (fuel < 40) {
        printf("No fuel. waiting...\n");
        pthread_cond_wait(&condFuel, &mutexFuel);
    }
    fuel -= 40;
    printf("Got fuel... now left %d\n", fuel);
    pthread_mutex_unlock(&mutexFuel);
}

int main()
{
    pthread_t threadArray[2];
    pthread_mutex_init(&mutexFuel, NULL);
    pthread_cond_init(&condFuel, NULL);
    if (pthread_create(&threadArray[1], NULL, &fuel_filling, NULL) != 0) perror("Failed to fill");
    if (pthread_create(&threadArray[0], NULL, &car, NULL) != 0) perror("Failed to make car");
    for (int i = 0; i < 2; i++)
        if (pthread_join(threadArray[i], NULL) != 0) perror("Failed to join thread");
    pthread_mutex_destroy(&mutexFuel);
    pthread_cond_destroy(&condFuel);
    return 0;
}