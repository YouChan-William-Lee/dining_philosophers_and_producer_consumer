#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define NUM_THREADS 5
#define NUM_BUCKETS 10
#define MAX_RANDOM_NUMBER 99

int bucketIn = 0;
int bucketOut = 0;
int NumOfItems = 0;
int buckets[NUM_BUCKETS];
struct timeval t1, t2, t3, t4;
double elapsedTime;

pthread_cond_t wait_here;
pthread_mutex_t mutex;

void * producer(void * p_no) {
    int item;
    while(true) {
        // check the running time
        gettimeofday(&t2, NULL);
        elapsedTime = t2.tv_sec - t1.tv_sec;
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000000.0;

        // check if the running time is greater than or equal to 10
        if(elapsedTime >= 10.0) {
            break;
        }

        // generate a random number between 1 and 99
        item = rand() % MAX_RANDOM_NUMBER + 1;

        // lock here
        pthread_mutex_lock(&mutex);
        {   
            // producer checks if buckets are full or current bucket isn't empty
            while (NumOfItems == 10) {
                pthread_cond_wait(&wait_here, &mutex);
            }

            // put the random number into bucket
            buckets[bucketIn] = item;
            // increase the number of items
            NumOfItems += 1;
            printf("Producer%d produced %2d in buckets[%d]\n",*((int *)p_no) , buckets[bucketIn], bucketIn);
            
            // jump to next bucket 
            bucketIn = (bucketIn + 1) % NUM_BUCKETS;
        }
        // unlock here
        pthread_mutex_unlock(&mutex);
        // send signal
        pthread_cond_signal(&wait_here);
        usleep(10000);
    }
    return NULL;
}

void * consumer(void * c_no) { 
    while(true) {
        // check the running time
        gettimeofday(&t3, NULL);
        elapsedTime = t3.tv_sec - t1.tv_sec;
        elapsedTime += (t3.tv_usec - t1.tv_usec) / 1000000.0;

        // check if the running time is greater than or equal to 10
        if(elapsedTime >= 10.0) {
            break;
        }

        // lock here
        pthread_mutex_lock(&mutex);
        {  
            // consumer checks if buckets are empty or current bucket is empty
            while (NumOfItems == 0) {
                pthread_cond_wait(&wait_here, &mutex);
            } 
            // decrease the number of items
            NumOfItems -= 1;
            printf("Consumer%d consumed %2d in buckets[%d]\n",*((int *)c_no) , buckets[bucketOut], bucketOut);
            buckets[bucketOut] = 0;
            
            // jump to next bucket
            bucketOut = (bucketOut + 1) % NUM_BUCKETS;
        }
        // unlock here
        pthread_mutex_unlock(&mutex);
        // send signal
        pthread_cond_signal(&wait_here);
        usleep(10000);
    }
    return NULL;
}

int main(void) {
    // check the start time
    gettimeofday(&t1, NULL);

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

    // check the total running time
    gettimeofday(&t4, NULL);
    elapsedTime = t4.tv_sec - t1.tv_sec;
    elapsedTime += (t4.tv_usec - t1.tv_usec) / 1000000.0;

    printf("Time = %.2f sec\n", elapsedTime);
    
    // destroy pthread_mutex
    pthread_mutex_destroy(&mutex);
    // exit phtread
    pthread_exit(NULL);
    return EXIT_SUCCESS;
}
