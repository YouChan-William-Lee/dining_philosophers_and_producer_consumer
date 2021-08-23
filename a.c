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
        // generate a random number between 1 and 99
        item = rand() % MAX_RANDOM_NUMBER + 1;

        // lock here
        pthread_mutex_lock(&mutex);
        {
            // producer checks if full and bucket isn't empty
            while (full && buckets[bucketIn] != 0) {
                pthread_cond_wait(&wait_here, &mutex);
            }

            // put the random number into bucket
            buckets[bucketIn] = item;
            // Make full true
            full = true;
            printf("Producer%d produced %2d in buckets[%d]\n",*((int *)p_no) , buckets[bucketIn], bucketIn);
            
            // jump to next bucket 
            bucketIn = (bucketIn + 1) % NUM_BUCKETS;
        }
        // unlock here
        pthread_mutex_unlock(&mutex);
        // send signal
        pthread_cond_signal(&wait_here);
    }
    return NULL;
}

void * consumer(void * c_no) { 
   int i; 
   for (i = 0; i < NUM_BUCKETS; i++) {

        // lock here
        pthread_mutex_lock(&mutex);
        {  
            // consumer checks if empty and bucket is empty
            while (!full && buckets[bucketOut] == 0) {
                pthread_cond_wait(&wait_here, &mutex);
            } 

            // Make full false
            full = false;
            printf("Consumer%d consumed %2d in buckets[%d]\n",*((int *)c_no) , buckets[bucketOut], bucketOut);
            buckets[bucketOut] = 0;
            // jump to next bucket
            bucketOut = (bucketOut + 1) % NUM_BUCKETS;
        }
        // unlock here
        pthread_mutex_unlock(&mutex);
        // send signal
        pthread_cond_signal(&wait_here);
    }
    return NULL;
}

int main(void) {
    // numbering for threads
    int threadNum[NUM_THREADS] = {1, 2, 3, 4, 5};
    int result;

    // initialize random number generator
    srand((unsigned) time(NULL));

    // initialize pthread_mutex
    pthread_mutex_init(&mutex, NULL);
    // initialize pthread_cond
    pthread_cond_init(&wait_here, NULL);

    // 5 producers and 5 consumers
    pthread_t producers[NUM_THREADS];
    pthread_t consumers[NUM_THREADS];

    // create 5 threads which are producers
    int t;
    for(t = 0; t < NUM_THREADS; t++) {
        result = pthread_create(&producers[t], NULL, (void *)producer, (void *)&threadNum[t]);
        if(result) {
            printf("\nThread can't be created : [%s]", strerror(result));
        }
    }

    // create 5 threads which are consumers
    for(t = 0; t < NUM_THREADS; t++) {
        result = pthread_create(&consumers[t], NULL, (void *)consumer, (void *)&threadNum[t]);
        if(result) {
            printf("\nThread can't be created : [%s]", strerror(result));
        }
    }

    // wait for producers termination
    for(t = 0; t < NUM_THREADS; t++) {
        pthread_join(producers[t], NULL);
    }   

    // wait for consumers termination
    for(t = 0; t < NUM_THREADS; t++) {
        pthread_join(consumers[t], NULL);
    }
    
    // destroy pthread_mutex
    pthread_mutex_destroy(&mutex);
    // exit phtread
    pthread_exit(NULL);
    return EXIT_SUCCESS;
}
