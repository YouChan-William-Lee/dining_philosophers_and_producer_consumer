#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 5
#define NUM_BUCKETS 10

int buffer = 0;
int buckets[NUM_BUCKETS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool full = false;

pthread_cond_t wait_here;
pthread_mutex_t buffer_lock;

void * producer(void * param) {
    srand((unsigned) time(NULL));
    for (int bucket = 0; bucket < NUM_BUCKETS; bucket++) {
        pthread_mutex_lock(&buffer_lock);
        {
            // producer checks if full
            while (full) {
                pthread_cond_wait(&wait_here, &buffer_lock);
            }
            buffer = rand() % 100 + 1;
            full = true;
            buckets[bucket] = buffer;
            printf("Producer%d produces %d in buckets[%d] -> ", param + 1, buckets[bucket], bucket);
            printf("buckets[] = ");
            for(int bucket = 0; bucket < NUM_BUCKETS; bucket++) {
                printf("%d ", buckets[bucket]);
            }
            printf("\n");
        }
        pthread_mutex_unlock(&buffer_lock);
        pthread_cond_signal(&wait_here);
    }
}

void * consumer(void * param) {
    srand((unsigned) time(NULL));
    for (int bucket = 0; bucket < NUM_BUCKETS; bucket++) {
        pthread_mutex_lock(&buffer_lock);
        {
            // consumer checks if empty
            while (!full) {
                pthread_cond_wait(&wait_here, &buffer_lock);
            }   
            printf("Consumer%d consumes %d in buckets[%d] -> ", param + 1, buckets[bucket], bucket);
            full = false;
            buckets[bucket] = 0;
            printf("buckets[] = ");
            for(int bucket = 0; bucket < NUM_BUCKETS; bucket++) {
                printf("%d ", buckets[bucket]);
            }
            printf("\n");
        }
        pthread_mutex_unlock(&buffer_lock);
        pthread_cond_signal(&wait_here);
    }
}

int main(void) {
    
    printf("buckets[] = ");
    for(int bucket = 0; bucket < NUM_BUCKETS; bucket++) {
        printf("%d ", buckets[bucket]);
    }
    printf("\n");

    pthread_mutex_init(&buffer_lock, NULL);
    pthread_cond_init(&wait_here, NULL);

    pthread_t producers[NUM_THREADS];
    pthread_t consumers[NUM_THREADS];

    for(int t = 0; t < NUM_THREADS; t++) {
        int result;
        result = pthread_create(&producers[t], NULL, producer, (void *)(intptr_t)t);
        if(result) {
            return EXIT_FAILURE;
        }

        result = pthread_create(&consumers[t], NULL, consumer, (void *)(intptr_t)t);
        if(result) {
            return EXIT_FAILURE;
        }
    }
    
    pthread_exit(NULL);
    return EXIT_SUCCESS;
}