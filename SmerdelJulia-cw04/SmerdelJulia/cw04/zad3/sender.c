#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define MAX_MODES 5

pid_t catcher_pid;

void handler(int sig, siginfo_t *si, void * unimportant){
    if (sig == SIGUSR1) {
        printf("Sendef: Received conformation from catcher with PID %d\n", si->si_pid);
        fflush(stdout);
    }
    if (sig == SIGINT){
        printf("Sender: catcher ended its work, terminating sender\n");
    }
}



void send_signal(int mode){
    printf("Sender: Sending signal with mode %d to catcher with PID %d\n", mode, catcher_pid);
    fflush(stdout);
    union sigval val;
    val.sival_int = mode;
    sigqueue(catcher_pid, SIGUSR1, val);
}

void wait_for_confirmation(){
    sigset_t mask;
    sigemptyset(&mask);
    // sigaddset(&mask, SIGUSR1);
    sigsuspend(&mask);
    // sleep(2);
}

int main(int argc, char **argv){
    if (argc < 2){
        printf("Sender: Error: catcher PID not provided\n");
        return 1;
    }

    catcher_pid = atoi(argv[1]);

    if (catcher_pid <= 0){
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
            printf("Sendef: Invalid mode: %d\n", mode);
            continue;
        }

        send_signal(mode);
        wait_for_confirmation();

    }
}

