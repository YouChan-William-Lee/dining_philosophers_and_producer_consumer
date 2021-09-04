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
#define CONVERT_TO_ARRAY_INDEX -1

pthread_t philosophers[NUM_PHILOSOPHER];
pthread_mutex_t forks[NUM_PHILOSOPHER];
pthread_cond_t wait_here;

// 3 states of philosophers
enum {THINKING, HUNGRY, EATING} state[NUM_PHILOSOPHER];

// how many meals philosophers have
int NumOfEaten[NUM_PHILOSOPHER];
struct timeval t1, t2, t3;
double elapsed_time;

// check left neighbour and right neighbour
void check_neighbours(int philosopher_number) {
    int left_philosopher = (philosopher_number + NUM_PHILOSOPHER + LEFT) % NUM_PHILOSOPHER;
    int right_philosopher = (philosopher_number + RIGHT) % NUM_PHILOSOPHER;

    // check left neighbour                    check this philosopher                   check right neighbour
    if((state[left_philosopher] != EATING) && (state[philosopher_number] == HUNGRY) && (state[right_philosopher] != EATING)) {
        // if both left neighbour and right neighbour are not eating, then this philosopher can start eating
        state[philosopher_number] = EATING;        
    }
}

void * philosopher(void * p_no) {
    int philosopher_number = *(int *)p_no;
    int left_fork = philosopher_number;
    int right_fork = (philosopher_number + RIGHT) % NUM_PHILOSOPHER;

    int think_time = MIN_DELAY + (rand() % (MAX_DELAY - MIN_DELAY));
    printf("Philosopher %d is thinking for %d microseconds\n", philosopher_number, think_time);

    while(true)
    {
        // check the running time
        printf("inside of while loop");
        gettimeofday(&t2, NULL);
        elapsed_time = t2.tv_sec - t1.tv_sec;
        elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000000.0;
	printf("\n%.2f seconds\n",elapsed_time);
        // check if the running time is greater than or equal to 10
        if(elapsed_time >= 10.0) {
            break;
        }
        printf("inside of while loop2");
        // think
        usleep(think_time);

        // hungry
        state[philosopher_number + CONVERT_TO_ARRAY_INDEX] = HUNGRY;

        // try to grab the first fork (left fork)
        int rv = pthread_mutex_trylock(&forks[left_fork + CONVERT_TO_ARRAY_INDEX]);
        if(rv == 0) {
            // try to grab the second fork (right fork)
            rv = pthread_mutex_trylock(&forks[right_fork + CONVERT_TO_ARRAY_INDEX]);
            if(rv == 0) {
                printf("Philosopher %d picked up fork %d\n",philosopher_number, left_fork);
                printf("Philosopher %d picked up fork %d\n",philosopher_number, right_fork);

                // check both left and right neighbour are eating
                check_neighbours(philosopher_number + CONVERT_TO_ARRAY_INDEX);

                // if neighbours are eating, then wait until they finish
                while(state[philosopher_number + CONVERT_TO_ARRAY_INDEX] != EATING)
                {
                    pthread_cond_wait(&wait_here, &forks[left_fork + CONVERT_TO_ARRAY_INDEX]);
                    pthread_cond_wait(&wait_here, &forks[right_fork + CONVERT_TO_ARRAY_INDEX]);
                }

                // increment of this philoshpher's meal
                (NumOfEaten[philosopher_number + CONVERT_TO_ARRAY_INDEX])++;

                printf("Philosopher %d is eating meal %d\n", philosopher_number, NumOfEaten[philosopher_number]);
                
                // eat
                int eat_time = MIN_DELAY + (rand() % (MAX_DELAY - MIN_DELAY));
                usleep(eat_time);
                
                // return two forks
                printf("Philosopher %d returned fork %d\n",philosopher_number, right_fork);
                printf("Philosopher %d returned fork %d\n",philosopher_number, left_fork);

                // unlock
                pthread_mutex_unlock(&forks[right_fork + CONVERT_TO_ARRAY_INDEX]);
                pthread_mutex_unlock(&forks[left_fork + CONVERT_TO_ARRAY_INDEX]);

                // send signal
                pthread_cond_signal(&wait_here);

                // turn to think
                state[philosopher_number + CONVERT_TO_ARRAY_INDEX] = THINKING;

                think_time = MIN_DELAY + (rand() % (MAX_DELAY - MIN_DELAY));

                printf("Philosopher %d is thinking for %d microseconds\n", philosopher_number, think_time);
            }
            else {
                // put down the first fork
                pthread_mutex_unlock(&forks[left_fork + CONVERT_TO_ARRAY_INDEX]);
            }
        }
    }
    return NULL;
}

int main(void)
{
    // initialize random number generator
    srand((unsigned) time(NULL));

    // numbering for threads
    int threadNum[NUM_PHILOSOPHER] = {1, 2, 3, 4, 5};

    //Initialize pthread_cond
    pthread_cond_init(&wait_here, NULL);

    // initialize philosophers' state, num of meals and pthread_mutex
    int t;
    for(t = 0; t < NUM_PHILOSOPHER; t++) {
        state[t] = THINKING;
        NumOfEaten[t] = 0;
        pthread_mutex_init(&forks[t], NULL);
    }

    // check the start time
    gettimeofday(&t1, NULL);    

    // 5 philosopher threads
    for(t = 0; t < NUM_PHILOSOPHER; t++) {
        pthread_create(&philosophers[t], NULL, philosopher, (void *)&threadNum[t]);
    }
    printf("test1");	
    // wait for philosophers termination
    for(t = 0; t < NUM_PHILOSOPHER; t++) {
        pthread_join(philosophers[t], NULL);
    }
    printf("test2");
    // check the total running time
    gettimeofday(&t3, NULL);
    elapsed_time = t3.tv_sec - t1.tv_sec;
    elapsed_time += (t3.tv_usec - t1.tv_usec) / 1000000.0;

    printf("\nTime = %.2f sec\n", elapsed_time);

    // display the result how many meals each philosopher have
    printf("\n/* RESULT */\n");
    for(t = 0; t < NUM_PHILOSOPHER; t++) {
        printf("Philosopher %d ate %d times\n", threadNum[t], NumOfEaten[t]);
    }

    // destroy pthread_mutex
    for(t = 0; t < NUM_PHILOSOPHER; t++) {
        pthread_mutex_destroy(&forks[t]);
    }

    // exit phtread
    pthread_exit(NULL);     

    return EXIT_SUCCESS;
}
