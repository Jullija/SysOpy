#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#define MAX_MODES 5

pid_t catcherPID;

void handler(int signal, siginfo_t *si, void * GingerbreadMan){
    if (signal == SIGUSR1) {
        printf("Sender: Received confirmation from catcher with PID %d\n, we are there. Yet!", si->si_pid);
        fflush(stdout);
    }
    if (signal == SIGINT){
        printf("Sender: Catcher caught everything, autodestruction in 3.. 2.. 1..\n");
    }
}



void sendSignal(int mode){
    printf("Sender: Sending signal with mode %d to catcher with PID %d\n", mode, catcherPID);
    fflush(stdout);
    union sigval val;
    val.sival_int = mode;
    sigqueue(catcherPID, SIGUSR1, val);
}

void waitForConfirmation(){
    sigset_t set;
    sigemptyset(&set);
    sigsuspend(&set);

}

int main(int argc, char **argv){
    if (argc < 2){
        printf("Sender: Error: catcher PID not provided\n");
        return 1;
    }

    catcherPID = atoi(argv[1]);

    if (catcherPID<= 0){
        printf("Sender: Invalid catcher PID\n");
        return 1;
    }

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    for (int i = 2; i < argc; i++){
        int mode = atoi(argv[i]);

        if (mode < 1 || mode > MAX_MODES){
            printf("Sender: Invalid mode: %d\n", mode);
            continue;
        }

        sendSignal(mode);
        waitForConfirmation();

    }
}

