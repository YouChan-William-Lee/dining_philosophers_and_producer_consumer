#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
 
#define NUM_THREADS 5
 
pthread_mutex_t mutexes[NUM_THREADS];
pthread_cond_t conditionVars[NUM_THREADS];
 
int forks[NUM_THREADS];
pthread_t philosophers[NUM_THREADS];
 
int main() {
    
 
    return EXIT_SUCCESS;
}