#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5

sem_t chopsticks[N];
pthread_mutex_t mutex;
int philosophers[N] = {0, 1, 2, 3, 4};

void delay(int len) {
    usleep(rand() % (len + 1));
}

void* philosopher(void* args) {
    int i = *(int*)args;
    int left = i;
    int right = (i + 1) % N;
    while (1) {
        printf("Philosopher %d is thinking...\n", i);
        delay(1000000);
        printf("Philosopher %d is hungry.\n", i);
        pthread_mutex_lock(&mutex);
        sem_wait(&chopsticks[left]);
        printf("Philosopher %d picks up chopstick %d, cannot eat with one chopstick.\n", i, left);
        sem_wait(&chopsticks[right]);
        pthread_mutex_unlock(&mutex);
        printf("Philosopher %d picks up chopstick %d. Now eating...\n", i, right);
        delay(1000000);
        sem_post(&chopsticks[left]);
        printf("Philosopher %d puts down chopstick %d.\n", i, left);
        sem_post(&chopsticks[right]);
        printf("Philosopher %d puts down chopstick %d.\n", i, right);
    }
    return NULL;
}

int main(int argc, char **argv) {
    srand(time(NULL));
    pthread_t philo[N];

    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < N; i++) {
        sem_init(&chopsticks[i], 0, 1);
    }

    for (int i = 0; i < N; i++) {
        pthread_create(&philo[i], NULL, philosopher, &philosophers[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(philo[i], NULL);
    }

    for (int i = 0; i < N; i++) {
        sem_destroy(&chopsticks[i]);
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}
