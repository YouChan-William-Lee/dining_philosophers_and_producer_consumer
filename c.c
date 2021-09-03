#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

//Constants to be used in the program
#define NUM_PHILOSOPHER 5
#define MAX_MEALS 10
#define LEFT -1
#define RIGHT 1

//States of philosophers
enum {THINKING, HUNGRY, EATING} state[NUM_PHILOSOPHER];

//Array to hold the thread identifiers
pthread_t philosophers[NUM_PHILOSOPHER];

//Mutex lock
pthread_mutex_t forks[NUM_PHILOSOPHER];

//Condition variables.
pthread_cond_t wait_here;

//Array to hold the number of meals eaten for each philosopher
int NumOfEaten[NUM_PHILOSOPHER];

void test(int philosopher_number)
{
    if((state[(philosopher_number + NUM_PHILOSOPHER + LEFT) % NUM_PHILOSOPHER] != EATING) && 
        (state[philosopher_number] == HUNGRY) &&
        (state[(philosopher_number + RIGHT) % NUM_PHILOSOPHER] != EATING)) {
        state[philosopher_number] = EATING;
        pthread_cond_signal(&wait_here);
    }
}

void * return_forks(void * p_no)
{
    int philosopher_number = *(int *)p_no;
    int left_fork = philosopher_number;
    int right_fork = (philosopher_number + RIGHT) % NUM_PHILOSOPHER;
    int left_philosopher = (philosopher_number + NUM_PHILOSOPHER + LEFT) % NUM_PHILOSOPHER;
    int right_philosopher = (philosopher_number + RIGHT) % NUM_PHILOSOPHER;

    pthread_mutex_lock(&forks[right_fork]);
    printf("Philosopher %d returned fork %d\n",philosopher_number, right_fork);
    pthread_mutex_lock(&forks[left_fork]);
    printf("Philosopher %d returned fork %d\n",philosopher_number, left_fork);

    state[philosopher_number] = THINKING;

    // test left philosopher 
    test(left_philosopher);
    // test right philosopher 
    test(right_philosopher);

    pthread_mutex_unlock(&forks[right_fork]);
    pthread_mutex_unlock(&forks[left_fork]);
    return NULL;
}

void * pickup_forks(void * p_no)
{
    int philosopher_number = *(int *)p_no;
    int left_fork = philosopher_number;
    int right_fork = (philosopher_number + RIGHT) % NUM_PHILOSOPHER;

    while(NumOfEaten[philosopher_number] < MAX_MEALS)
    {
        int sleeptime = rand() % 5 + 1;
        printf("Philosopher %d is thinking for %d seconds\n", philosopher_number, sleeptime);
        // think
        sleep(sleeptime);

        pthread_mutex_lock(&forks[left_fork]);
        printf("Philosopher %d got fork %d\n",philosopher_number, left_fork);
        pthread_mutex_lock(&forks[right_fork]);
        printf("Philosopher %d got fork %d\n",philosopher_number, right_fork);

        state[philosopher_number] = HUNGRY;
        test(philosopher_number);

        while(state[philosopher_number] != EATING)
        {
            pthread_cond_wait(&wait_here, &forks[left_fork]);
            pthread_cond_wait(&wait_here, &forks[right_fork]);
        }
        

        (NumOfEaten[philosopher_number])++;

        printf("Philosopher %d is eating meal %d.\n", philosopher_number, NumOfEaten[philosopher_number]);
        
        // eat
        sleep(rand() % 5 + 1);

        pthread_mutex_unlock(&forks[right_fork]);
        pthread_mutex_unlock(&forks[left_fork]);

        return_forks(&philosopher_number);
    }
    return NULL;
}

int main(void)
{
    // initialize random number generator
    srand((unsigned) time(NULL));

    // numbering for threads
    int threadNum[NUM_PHILOSOPHER] = {0, 1, 2, 3, 4};

    //Initialize arrays.
    pthread_cond_init(&wait_here, NULL);
    int t;
    for(t = 0; t < NUM_PHILOSOPHER; t++) {
        state[t] = THINKING;
        NumOfEaten[t] = 0;
        pthread_mutex_init(&forks[t], NULL);
    }


    //Initialize the mutex lock.
    

    //Create threads for the philosophers.
    for(t = 0; t < NUM_PHILOSOPHER; t++) {
        pthread_create(&philosophers[t], NULL, pickup_forks, (void *)&threadNum[t]);
    }

    //Join the threads.
    for(t = 0; t < NUM_PHILOSOPHER; t++) {
        pthread_join(philosophers[t], NULL);
    }

    //Print the number of meals that each philosopher ate.
    for(t = 0; t < NUM_PHILOSOPHER; t++) {
        printf("Philosopher %d: %d meals\n", threadNum[t], NumOfEaten[t]);
    }

    // destroy pthread_mutex
    for(t = 0; t < NUM_PHILOSOPHER; t++) {
        pthread_mutex_destroy(&forks[t]);
    }

    // exit phtread
    pthread_exit(NULL);     

    return EXIT_SUCCESS;
}
