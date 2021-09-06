#ifdef DINING_PHILOSOPHERS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#define NUM_PHILOSOPHER 5
#define LEFT -1
#define RIGHT 1
#define MAX_DELAY 500000
#define MIN_DELAY 100000
#define INC_ONE_INDEX 1

/* 3 states of philosophers */
enum {THINKING, HUNGRY, EATING} state[NUM_PHILOSOPHER];

/* how many meals philosophers have */
int NumOfEaten[NUM_PHILOSOPHER];
struct timeval t1, t2, t3;
double elapsed_time;

pthread_mutex_t forks[NUM_PHILOSOPHER];
pthread_cond_t wait_here[NUM_PHILOSOPHER];

/* check left neighbour and right neighbour */
void check_neighbours(int philosopher_number) {
    int left_philosopher = (philosopher_number + NUM_PHILOSOPHER + LEFT) % NUM_PHILOSOPHER;
    int right_philosopher = (philosopher_number + RIGHT) % NUM_PHILOSOPHER;

    /* check left neighbour                    check this philosopher */                  
    if((state[left_philosopher] != EATING) && (state[philosopher_number] == HUNGRY) 
        /* check right neighbour */
        && (state[right_philosopher] != EATING)) {
        /* if both left neighbour and right neighbour are not eating, then this philosopher can start eating */
        state[philosopher_number] = EATING;  
        /* send a signal "that you can eat" */
        pthread_cond_signal(&wait_here[philosopher_number]);      
    }
}

/* check the running time*/
bool check_running_time() {
    /* check the running time */
    gettimeofday(&t2, NULL);
    elapsed_time = t2.tv_sec - t1.tv_sec;
    elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000000.0;
    /* check if the running time is greater than or equal to 10 */
    if(elapsed_time >= 10.0) {
        return true;
    }
    return false;
}

/* Philosopher is thinking */
void think(int philosopher_number, int think_time) {
    printf("Philosopher%d is thinking\n----------------------------------\n", philosopher_number + INC_ONE_INDEX);
    /* think */
    usleep(think_time);
}

void pickup_forks(int philosopher_number, int left_fork, int right_fork) {
    /* try to grab the first fork (left fork) */
    pthread_mutex_lock(&forks[left_fork]);
    printf("philosopher%d picked up fork %d\n", philosopher_number + INC_ONE_INDEX, left_fork + INC_ONE_INDEX);    

    /* try to grab the second fork (right fork) */
    pthread_mutex_lock(&forks[right_fork]);
    /*
    printf("Philosopher%d picked up fork %d\n",philosopher_number + INC_ONE_INDEX, left_fork + INC_ONE_INDEX);
    */
    printf("Philosopher%d picked up fork %d\n",philosopher_number + INC_ONE_INDEX, right_fork + INC_ONE_INDEX);

    /* check both left and right neighbour are eating */
    check_neighbours(philosopher_number);

    /* if neighbours are eating, then wait until they finish */
    while(state[philosopher_number] != EATING)
    {
        pthread_cond_wait(&wait_here[philosopher_number], &forks[left_fork]);
        pthread_cond_wait(&wait_here[philosopher_number], &forks[right_fork]);
    }
}

void eat(int philosopher_number, int eat_time) {
    /* increment of this philoshpher's meal */
    (NumOfEaten[philosopher_number ])++;

    printf("Philosopher%d is eating, %d time(s)\n----------------------------------\n", philosopher_number + INC_ONE_INDEX, NumOfEaten[philosopher_number]);
    /* eat */
    usleep(eat_time);
}

void return_forks(int philosopher_number, int left_fork, int right_fork, int left_philosopher, int right_philosopher) {
    /* return two forks */
    printf("Philosopher%d returned fork %d\n",philosopher_number + INC_ONE_INDEX, right_fork + INC_ONE_INDEX);
    printf("Philosopher%d returned fork %d\n----------------------------------\n",philosopher_number + INC_ONE_INDEX, left_fork + INC_ONE_INDEX);
    
    /* Let neighbours know that I'm done */
    check_neighbours(left_philosopher);
    check_neighbours(right_philosopher);

    /* turn to think */
    state[philosopher_number] = THINKING;

    /* unlock */
    pthread_mutex_unlock(&forks[right_fork]);
    pthread_mutex_unlock(&forks[left_fork]);  
}

void * philosopher(void * p_no) {
    int philosopher_number = *(int *)p_no;
    int left_fork = philosopher_number;
    int right_fork = (philosopher_number + RIGHT) % NUM_PHILOSOPHER;
    int left_philosopher = (philosopher_number + NUM_PHILOSOPHER + LEFT) % NUM_PHILOSOPHER;
    int right_philosopher = (philosopher_number + RIGHT) % NUM_PHILOSOPHER;

    while(true)
    {        
        /* CHECKING THE RUNNING TIME */
        if(check_running_time()) break;

        /* THINKING */
        think(philosopher_number, MIN_DELAY + (rand() % (MAX_DELAY - MIN_DELAY)));

        /* HUNGRY */
        state[philosopher_number] = HUNGRY;

        /* PICK UP TWO FORKS*/
        pickup_forks(philosopher_number, left_fork, right_fork);

        /* EATING */
        eat(philosopher_number, MIN_DELAY + (rand() % (MAX_DELAY - MIN_DELAY)));
        
        /* RETURN TWO FORKS*/
        return_forks(philosopher_number, left_fork, right_fork, left_philosopher, right_philosopher);    
    }
    return NULL;
}

int main(void)
{
    /* numbering for threads */
    int threadNum[NUM_PHILOSOPHER] = {0, 1, 2, 3, 4};
    int result, t;

    pthread_t philosophers[NUM_PHILOSOPHER];

    /* initialize random number generator */
    srand((unsigned) time(NULL));    

    /* initialize philosophers' state, num of meals, pthread_mutex_t and pthread_cond_t */
    for(t = 0; t < NUM_PHILOSOPHER; t++) {
        state[t] = THINKING;
        NumOfEaten[t] = 0;
        pthread_mutex_init(&forks[t], NULL);
        pthread_cond_init(&wait_here[t], NULL);
    }

    /* check the start time */
    gettimeofday(&t1, NULL);    

    /* 5 philosopher threads */
    for(t = 0; t < NUM_PHILOSOPHER; t++) {
        result = pthread_create(&philosophers[t], NULL, philosopher, (void *)&threadNum[t]);
        if(result) {
            printf("\nThread can't be created : [%s]", strerror(result));
        }
    }
    
    /* wait for philosophers termination */
    for(t = 0; t < NUM_PHILOSOPHER; t++) {
        pthread_join(philosophers[t], NULL);
    }
    
    /* check the total running time */
    gettimeofday(&t3, NULL);
    elapsed_time = t3.tv_sec - t1.tv_sec;
    elapsed_time += (t3.tv_usec - t1.tv_usec) / 1000000.0;

    printf("\nTime = %.2f sec\n", elapsed_time);

    /* display the result how many meals each philosopher have */
    printf("\n/-------- RESULT --------/\n");
    for(t = 0; t < NUM_PHILOSOPHER; t++) {
        printf("Philosopher %d ate %d times\n", threadNum[t]  + INC_ONE_INDEX, NumOfEaten[t]);
    }

    /* destroy pthread_mutex */
    for(t = 0; t < NUM_PHILOSOPHER; t++) {
        pthread_mutex_destroy(&forks[t]);
    }

    /* exit phtread */
    pthread_exit(NULL);     

    return EXIT_SUCCESS;
}

#elif defined PRODUCER_CONSUMER

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#define NUM_THREADS 5
#define NUM_BUCKETS 10
#define MAX_RANDOM_NUMBER 99
#define SLEEP_TIME 100000

int bucketIn = 0;
int bucketOut = 0;
int NumOfItems = 0;
int buckets[NUM_BUCKETS];
struct timeval t1, t2, t3, t4;
double elapsed_time;

pthread_cond_t wait_here;
pthread_mutex_t mutex;

void * producer(void * p_no) {
    int item;
    while(true) {
        /* check the running time */
        gettimeofday(&t2, NULL);
        elapsed_time = t2.tv_sec - t1.tv_sec;
        elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000000.0;

        /* check if the running time is greater than or equal to 10 */
        if(elapsed_time >= 10.0) break;

        /* generate a random number between 1 and 99 */
        item = rand() % MAX_RANDOM_NUMBER + 1;

        /* lock here */
        pthread_mutex_lock(&mutex);
        {   
            /* producer checks if buckets are full or current bucket isn't empty */
            while (NumOfItems == 10) {
                pthread_cond_wait(&wait_here, &mutex);
            }
            
            /* put the random number into bucket */
            buckets[bucketIn] = item;
            /* increase the number of items */
            NumOfItems += 1;
            printf("Producer%d produced %2d in buckets[%d]\n",*((int *)p_no) , buckets[bucketIn], bucketIn);
            
            /* jump to next bucket */
            bucketIn = (bucketIn + 1) % NUM_BUCKETS;
        }
        /* unlock here */
        pthread_mutex_unlock(&mutex);
        /* send signal */
        pthread_cond_signal(&wait_here);
        /* sleep for other threads */
        usleep(SLEEP_TIME);
    }
    return NULL;
}

void * consumer(void * c_no) { 
    while(true) {
        /* check the running time */
        gettimeofday(&t3, NULL);
        elapsed_time = t3.tv_sec - t1.tv_sec;
        elapsed_time += (t3.tv_usec - t1.tv_usec) / 1000000.0;

        /* check if the running time is greater than or equal to 10 */
        if(elapsed_time >= 10.0) break;

        /* lock here */
        pthread_mutex_lock(&mutex);
        {  
            /* consumer checks if buckets are empty or current bucket is empty */
            while (NumOfItems == 0) {
                pthread_cond_wait(&wait_here, &mutex);
            }

            /* decrease the number of items */
            NumOfItems -= 1;
            printf("Consumer%d consumed %2d in buckets[%d]\n",*((int *)c_no) , buckets[bucketOut], bucketOut);
            buckets[bucketOut] = 0;
            
            /* jump to next bucket */
            bucketOut = (bucketOut + 1) % NUM_BUCKETS;
        }
        /* unlock here */
        pthread_mutex_unlock(&mutex);
        /* send signal */
        pthread_cond_signal(&wait_here);
        /* sleep for other threads */
        usleep(SLEEP_TIME);
    }
    return NULL;
}

int main(void) {
    /* numbering for threads */
    int threadNum[NUM_THREADS] = {1, 2, 3, 4, 5};
    int result, t;

    /* 5 producers and 5 consumers */
    pthread_t producers[NUM_THREADS];
    pthread_t consumers[NUM_THREADS];

    /* initialize random number generator */
    srand((unsigned) time(NULL));

    /* check the start time */
    gettimeofday(&t1, NULL);

    /* initialize pthread_mutex */
    pthread_mutex_init(&mutex, NULL);
    /* initialize pthread_cond */
    pthread_cond_init(&wait_here, NULL);

    /* create 5 threads which are producers */
    for(t = 0; t < NUM_THREADS; t++) {
        result = pthread_create(&producers[t], NULL, producer, (void *)&threadNum[t]);
        if(result) {
            printf("\nThread can't be created : [%s]", strerror(result));
        }

        result = pthread_create(&consumers[t], NULL, consumer, (void *)&threadNum[t]);
        if(result) {
            printf("\nThread can't be created : [%s]", strerror(result));
        }
    }

    /* wait for producers termination */
    for(t = 0; t < NUM_THREADS; t++) {
        pthread_join(producers[t], NULL);
    }   

    /* wait for consumers termination */
    for(t = 0; t < NUM_THREADS; t++) {
        pthread_join(consumers[t], NULL);
    }

    /* check the total running time */
    gettimeofday(&t4, NULL);
    elapsed_time = t4.tv_sec - t1.tv_sec;
    elapsed_time += (t4.tv_usec - t1.tv_usec) / 1000000.0;

    printf("Time = %.2f sec\n", elapsed_time);

    /* destroy pthread_mutex */
    pthread_mutex_destroy(&mutex);
    /* exit phtread */
    pthread_exit(NULL);
    return EXIT_SUCCESS;
}

#endif
