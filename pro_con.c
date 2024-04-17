#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include<semaphore.h>

#define BUFFER_MAX 5 //Defines the maximum capacity of the buffer to 10.
#define PRODUCER_COUNT 2 //Defines the number of producer
#define CONSUMER_COUNT 3 //Defines the number of 

int buffer[BUFFER_MAX];  //shared buffer where producers place items and from which consumers take items.
int in = 0;  //track the positions where producers insert items
int out = 0; //track the positions where consumer remove items

sem_t sem_empty; 
sem_t sem_full;  
pthread_mutex_t mutex; //ensure that only one thread manipulates the buffer at any given time.

//simulates the producer's behavior
void *pro_thread(void *arg) {
    intptr_t index = (intptr_t)arg;

    for(int i = 0; i < 6; i++) {
        sem_wait(&sem_empty); // Wait for empty slot
        pthread_mutex_lock(&mutex); // Lock buffer access
        buffer[in] = rand() % 100;
        printf("Producer %ld: Produced item %d at position %d\n", index+1, buffer[in], in);
        in = (in + 1) % BUFFER_MAX; //Updates the in index in a circular manner.
        pthread_mutex_unlock(&mutex); // Unlock buffer
        sem_post(&sem_full); // Signal that there is a new product
    }
    return NULL;
}

void *con_thread(void *arg) {
    intptr_t index = (intptr_t)arg;
    for(int i = 0; i < 4; i++) {
        sem_wait(&sem_full); // Wait for product
        pthread_mutex_lock(&mutex); // Lock buffer access
        printf("Consumer %ld: Consumed item %d at position %d\n", index+1, buffer[out], out);
        out = (out + 1) % BUFFER_MAX;
        pthread_mutex_unlock(&mutex); // Unlock buffer
        sem_post(&sem_empty); // Signal that there is a new empty slot
    }
    return NULL;
}

int main() {
    sem_init(&sem_empty, 0, BUFFER_MAX); // Initialize semaphore for empty slots
    sem_init(&sem_full, 0, 0); // Initialize semaphore for full slots

    pthread_mutex_init(&mutex, NULL); // Initialize mutex

    pthread_t pro_id[PRODUCER_COUNT]; // Producer threads
    pthread_t con_id[CONSUMER_COUNT]; // Consumer threads

    //Creates threads for producers and consumers.
    for(int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_create(&pro_id[i], NULL, pro_thread, (void*)(intptr_t)i); // Create producer threads
    }

    for(int i = 0; i < CONSUMER_COUNT; i++) {
        pthread_create(&con_id[i], NULL, con_thread, (void*)(intptr_t)i); // Create consumer threads
    }

    for(int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_join(pro_id[i], NULL); // Wait for producers to finish
    }

    for(int i = 0; i < CONSUMER_COUNT; i++) {
        pthread_join(con_id[i], NULL); // Wait for consumers to finish
    }

    sem_destroy(&sem_empty); // Destroy semaphores
    sem_destroy(&sem_full);

    pthread_mutex_destroy(&mutex); // Destroy mutex

    printf("The buffer is empty!\n");
    exit(0);
}