#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define REINDEERS 9
#define ELVES 10

pthread_mutex_t santaMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeersCondition = PTHREAD_COND_INITIALIZER; //Condition Variables for sleeping
pthread_cond_t elvesCondition = PTHREAD_COND_INITIALIZER;
pthread_cond_t santaCondition = PTHREAD_COND_INITIALIZER;

int reindeersWaiting = 0;
int elvesWaiting = 0;
int solvedProblems = 0;
pthread_t elvesWaitingArr[3];

void *santa(void *arg) {
    while (solvedProblems < 3) {
        pthread_mutex_lock(&santaMutex);
        
        //if conditions are not fulfilled
        while (reindeersWaiting < REINDEERS && elvesWaiting < 3){
            printf("Mikołaj: zasypiam.\n");
            pthread_cond_wait(&santaCondition, &santaMutex); //thread is in waiting mode
            printf("Mikołaj: budzę się\n");
        }


        if (elvesWaiting == 3) {
            pthread_mutex_unlock(&santaMutex);
            printf("Mikołaj: rozwiązuje problemy elfów %lu, %lu, %lu.\n", elvesWaitingArr[0], elvesWaitingArr[1], elvesWaitingArr[2]);
            sleep(rand() % 2 + 1); 
            pthread_mutex_lock(&santaMutex);
            elvesWaiting = 0;
            pthread_cond_broadcast(&elvesCondition);
        }

        if (reindeersWaiting == REINDEERS) {
            pthread_mutex_unlock(&santaMutex);
            printf("Mikołaj: dostarczam zabawki\n");
            sleep(rand() % 3 + 2); 
            pthread_mutex_lock(&santaMutex);
            reindeersWaiting = 0;
            solvedProblems++;
            pthread_cond_broadcast(&reindeersCondition);

            if (solvedProblems == 3){
                break;
            }
        }

        pthread_mutex_unlock(&santaMutex);


    }

    pthread_exit(NULL);
}

void *reindeer(void *arg) {
    pthread_t reindeerID = pthread_self();

    while (1){
        sleep(rand() % 6 + 5); 
        pthread_mutex_lock(&santaMutex);
        reindeersWaiting++;
        printf("Renifer: czeka %d reniferów na Mikołaja, %lu\n", reindeersWaiting, reindeerID);

        if (reindeersWaiting == REINDEERS) {
            printf("Renifer: wybudzam Mikołaja, %lu\n", reindeerID);
            pthread_cond_signal(&santaCondition);
        }

        while (reindeersWaiting > 0){
            pthread_cond_wait(&reindeersCondition, &santaMutex);
        }
        pthread_mutex_unlock(&santaMutex);

    }




}

void *elf(void *arg) {
    pthread_t elfID = pthread_self();

    while(1){
        sleep(rand() % 4 + 2); 

        pthread_mutex_lock(&santaMutex);

        if (elvesWaiting < 3){
            elvesWaitingArr[elvesWaiting] = elfID;
            elvesWaiting++;
            printf("Elf: czeka %d elfów na Mikołaja, %lu\n", elvesWaiting, elfID);

            if (elvesWaiting == 3){
                printf("Elf: wybudzam Mikołaja, %lu\n", elfID);
                pthread_cond_signal(&santaCondition);
            }

            pthread_cond_wait(&elvesCondition, &santaMutex);

        }
        else{
            printf("Elf: samodzielnie rozwiązuję swój problem, %lu\n", elfID);
        }

        pthread_mutex_unlock(&santaMutex);

    }

}

int main() {

    pthread_t santaThread, reindeersThreads[REINDEERS], elvesThreads[ELVES];

    if ((pthread_create(&santaThread, NULL, santa, NULL)) != 0){
        printf("Error creating santa's thread. No presents today.");
        exit(-1);
    }

    for (int i = 0; i < ELVES; i++) {
        if((pthread_create(&elvesThreads[i], NULL, elf, NULL))!= 0){
            printf("Error creating elves' threads.");
            exit(-1);
        };
    }

    for (int i = 0; i < REINDEERS; i++) {
        if((pthread_create(&reindeersThreads[i], NULL, reindeer, NULL))!= 0){
            printf("Error creating reindeers' threads.");
            exit(-1);
        };
    }



    pthread_join(santaThread, NULL);

    for (int i = 0; i < ELVES; i++) {
        pthread_cancel(elvesThreads[i]);
    }

    for (int i = 0; i < REINDEERS; i++) {
        pthread_cancel(reindeersThreads[i]);
    }

    pthread_mutex_destroy(&santaMutex);
    pthread_cond_destroy(&reindeersCondition);
    pthread_cond_destroy(&elvesCondition);
    pthread_cond_destroy(&santaCondition);

    return 0;
}