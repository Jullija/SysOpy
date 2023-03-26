#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

void handlerSiginfo(int sig, siginfo_t * info, void * idk){
    printf("received signal %d from process with PID %d\n", sig, info->si_pid);
    printf("Uid === %d\n", info->si_uid);
    printf("status == %d\n", (info->si_status));
    printf("value ===: %d\n", info->si_code);
}

int call;
int depth;

void testSiginfo(struct sigaction action){
    printf("testing siginfo flag:\n");
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = handlerSiginfo;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &action, NULL);


    
    if(fork() == 0){
        printf("sending signal from child pid = === %d\n", getpid());
        raise(SIGUSR1);
        exit(0);
    }else{
        wait(NULL);
        printf("sending signal from parent with pid === %d\n", getpid());
        raise(SIGUSR1);
    }
}

void handlerDepth(int sig, siginfo_t * info, void* idk){
    printf("testing signal START::: call id %d, depth %d\n", call, depth);

    call+=1;
    depth+=1;
    if (call <= 3){
        raise(SIGUSR1);
    }
    depth-=1;
    printf("testing signal  END::: call id %d, depth %d\n", call, depth);
}



void testNodeffer(struct sigaction action){
    printf("testing SA_NODEFFER flag:\n");
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = handlerDepth;
    action.sa_flags = SA_NODEFER;
    sigaction(SIGUSR1, &action, NULL);

    raise(SIGUSR1);
}

void testResethanda(struct sigaction action){
    printf("testing SA_resethadn flag:\n");
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = handlerDepth;
    action.sa_flags = SA_RESETHAND;
    sigaction(SIGUSR1, &action, NULL);

    raise(SIGUSR1);
}



int main(){
    setbuf(stdout, NULL);
    struct sigaction action; 
    testSiginfo(action);

    testNodeffer(action);

    call = 0;
    depth = 0;
    testResethanda(action);
    
}