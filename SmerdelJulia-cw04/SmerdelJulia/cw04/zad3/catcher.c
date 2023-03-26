#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

volatile int num_request = 0;
volatile int current_mode = 0;
pid_t sender_pid;
int is_handled = 0;

void handle_request(int sig, siginfo_t *si, void *idkwhatisthis){
    setbuf(stdout, NULL);
    if (sig != SIGUSR1){
        printf("Catcher: Received unsupported signal: %d, waiting for signal: %d\n", sig, SIGUSR1);
        return;
    }
    printf("Catcher: Received signal with mode %d from sender with PID %d\n", si->si_value.sival_int, si->si_pid);

    // union sigval val;
    // sigqueue(si->si_pid, SIGUSR1, val);
    sender_pid = si->si_pid;

    current_mode = si->si_value.sival_int;

    num_request++;

    is_handled = 0;

    // if (current_mode == 5){
    //     sigqueue(si->si_pid, SIGINT, val);
    // }
    // printf("Mode: %d\n", current_mode);

}

void wait_for_signal(){
    sigset_t mask;
    sigemptyset(&mask);
    sigsuspend(&mask);

}

int main(){
    setbuf(stdout, NULL);
    printf("Catcher: Startin process, PID: %d\n", getpid());

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handle_request;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);
    union sigval val;
    while(1){
        // if(current_mode != 5){
        //     wait_for_signal();
        // }   
        if (is_handled){
            continue;
        }
        is_handled = 1;
        switch (current_mode){
            
        case 0:
            continue;
        case 1:
            printf("Catcher: Mode changed to 1: Printing numbers from 1 to 100\n");
            fflush(stdout);
            for (int i = 1; i <= 100; i++) {
                printf("%d ", i);
                fflush(stdout);
            }
            printf("\n");
            fflush(stdout);
            
            sigqueue(sender_pid, SIGUSR1, val);
            break;
        case 2:
            printf("Catcher: Mode changed to 2: Printing current time\n");
            fflush(stdout);
            time_t current_time;
            time(&current_time);
            printf("%s", ctime(&current_time));
            fflush(stdout);
            sigqueue(sender_pid, SIGUSR1, val);
            break;
        case 3:
            printf("Catcher: Mode changed to 3: Number of requests received: %d\n", num_request);
            fflush(stdout);
            sigqueue(sender_pid, SIGUSR1, val);
            break;
        case 4:
            printf("Catcher: Mode changed to 4: Printing current time every second until next mode change\n");
            fflush(stdout);
            sigqueue(sender_pid, SIGUSR1, val);
            while (current_mode == 4) {
                time_t current_time;
                time(&current_time);
                printf("%s", ctime(&current_time));
                fflush(stdout);
                sleep(1);
            }
            printf("out of the while\n");
            break;
        case 5: 
            printf("Catcher: Mode changed to 5: Terminating\n");
            fflush(stdout);
            sigqueue(sender_pid, SIGINT, val);
            
            exit(EXIT_SUCCESS);
        default:
            printf("Catcher: Invalid mode %d received\n", current_mode);
            fflush(stdout);
            break;
    }   
    
    }
}