#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define NUM_THREADS 5
#define NUM_BUCKETS 10
#define MAX_RANDOM_NUMBER 99

int bucketIn = 0;
int bucketOut = 0;
int buckets[NUM_BUCKETS];
bool full = false;

pthread_cond_t wait_here;
pthread_mutex_t mutex;

void * producer(void * p_no) {
    int i, item;

    for (i = 0; i < NUM_BUCKETS; i++) {
        item = rand() % MAX_RANDOM_NUMBER + 1;
        pthread_mutex_lock(&mutex);
        {
            // producer checks if full
            while (full) {
                pthread_cond_wait(&wait_here, &mutex);
            }
            if (buckets[bucketIn] == 0) {
                buckets[bucketIn] = item;

                // Display current state of buckets[]
                printf("Producer%d produced %2d in buckets[%d]\n",*((int *)p_no) , buckets[bucketIn], bucketIn);
            
                // jump to next bucket 
                bucketIn = (bucketIn + 1) % NUM_BUCKETS;
            }
        }
        pthread_mutex_unlock(&mutex);
        full = true;
        pthread_cond_signal(&wait_here);
    }
    return NULL;
}

void * consumer(void * c_no) { 
   int i; 
   for (i = 0; i < NUM_BUCKETS; i++) {
        pthread_mutex_lock(&mutex);
        {  
            // consumer checks if empty
            while (!full) {
                pthread_cond_wait(&wait_here, &mutex);
            }   
            if (buckets[bucketOut != 0]) {
                printf("Consumer%d consumed %2d in buckets[%d]\n",*((int *)c_no) , buckets[bucketOut], bucketOut);
                buckets[bucketOut] = 0;
                // jump to next bucket
                bucketOut = (bucketOut + 1) % NUM_BUCKETS;
            }
        }
        pthread_mutex_unlock(&mutex);
        full = false;
        pthread_cond_signal(&wait_here);
    }
    return NULL;
}

int main(void) {
    int threadNum[NUM_THREADS] = {1, 2, 3, 4, 5};
    int result;
    srand((unsigned) time(NULL));
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&wait_here, NULL);

    pthread_t producers[NUM_THREADS];
    pthread_t consumers[NUM_THREADS];
    int t;
    for(t = 0; t < NUM_THREADS; t++) {
        result = pthread_create(&producers[t], NULL, (void *)producer, (void *)&threadNum[t]);
        if(result) {
            printf("\nThread can't be created : [%s]", strerror(result));
        }
    }

    for(t = 0; t < NUM_THREADS; t++) {
        result = pthread_create(&consumers[t], NULL, (void *)consumer, (void *)&threadNum[t]);
        if(result) {
            printf("\nThread can't be created : [%s]", strerror(result));
        }
    }

    for(t = 0; t < NUM_THREADS; t++) {
        pthread_join(producers[t], NULL);
    }   

    for(t = 0; t < NUM_THREADS; t++) {
        pthread_join(consumers[t], NULL);
    }
    
    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);
    return EXIT_SUCCESS;
}
