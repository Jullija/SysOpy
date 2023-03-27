#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>

volatile int layersOfRequests = 0; //volatile - it will change while program is working
volatile int currMode = 0;
pid_t senderPID;
int handledTask = 0;

void handleRequest(int signal, siginfo_t *si, void *catInBoots){
    setbuf(stdout, NULL);
    if (signal != SIGUSR1){
        printf("Catcher: Received unsupported signal: %d, waiting for signal: %d\n", signal, SIGUSR1);
        return;
    }
    printf("Catcher: Received signal with mode %d from sender with PID %d\n", si->si_value.sival_int, si->si_pid);


    senderPID= si->si_pid;
    currMode = si->si_value.sival_int;
    layersOfRequests++;
    handledTask = 0;

}

void waitForIt(){
    sigset_t set;
    sigemptyset(&set);
    sigsuspend(&set); //receiving a signal waiting

}

int main(){
    setbuf(stdout, NULL);
    printf("Catcher: Let the fun begin, process PID: %d\n", getpid());

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handleRequest;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL); //when receiving a signal SIGUSR1 do what sa says
    union sigval val;
    while(1){

        if (handledTask){
            continue;
        }

        handledTask = 1;
        switch (currMode){
            
            case 0:
                continue;
            case 1:
                printf("Catcher: Mode changed to 1: Printing numbers from 1 to 100\n");
                for (int i = 1; i <= 100; i++) {
                    printf("%d ", i);
                }
                printf("\n");
                
                sigqueue(senderPID, SIGUSR1, val); //sending a signal to the process with senderPID
                break;
            case 2:
                printf("Catcher: Mode changed to 2: Printing current time\n");
                time_t current_time;
                time(&current_time);
                printf("%s", ctime(&current_time));

                sigqueue(senderPID, SIGUSR1, val);
                break;
            case 3:
                printf("Catcher: Mode changed to 3: Number of requests received: %d\n", layersOfRequests);

                sigqueue(senderPID, SIGUSR1, val);
                break;
            case 4:
                printf("Catcher: Mode changed to 4: Printing current time every second until next mode change\n");

                sigqueue(senderPID, SIGUSR1, val);
                while (currMode == 4) {
                    time_t current_time;
                    time(&current_time);
                    printf("%s", ctime(&current_time));
                    sleep(1);
                }

                break;
            case 5: 
                printf("Catcher: Mode changed to 5: Terminating\n");
                sigqueue(senderPID, SIGINT, val);
                
                exit(EXIT_SUCCESS);
            default:
                printf("Catcher: Invalid mode %d received\n", currMode);

                break;
    }   
    
    }
}