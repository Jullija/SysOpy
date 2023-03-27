#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>


int call;
int onionLayer;

//handler for siginfo flag
void handlerSiginfo(int sig, siginfo_t * info, void * idk){
    printf("Received signal %d from process with PID %d\n", sig, info->si_pid);
    printf("Uid === %d\n", info->si_uid);
    printf("Status == %d\n", (info->si_status));
    printf("Value ===: %d\n", info->si_code);
}

//handler for other flags
void OgresAreLikeOnions(int sig, siginfo_t * info, void* idk){
    printf("Testing signal START! ID: %d, onionLayer/depth %d\n", call, onionLayer);

    call+=1;
    onionLayer+=1;
    if (call <= 3){
        raise(SIGUSR1);
    }
    onionLayer-=1;
    printf("Testing signal END! ID %d, onionLayer/depth %d\n", call, onionLayer);
}


//SIGINFO FLAG
void testSiginfo(struct sigaction action){
    printf("\nTesting SIGINFO flag:\n");
    sigemptyset(&action.sa_mask); //initializing an empty collection 
    action.sa_sigaction = handlerSiginfo; //creating a handler (what to do if we receive a signal from kernel)
    action.sa_flags = SA_SIGINFO; //react just like this flag
    sigaction(SIGUSR1, &action, NULL); //changing the behaviour - when receiving a signal SIGUSR1 do what in action


    
    if(fork() == 0){
        printf("Sending signal from child pid = === %d\n", getpid());
        raise(SIGUSR1);
        exit(0);
    }else{
        wait(NULL);
        printf("Sending signal from parent with pid === %d\n", getpid());
        raise(SIGUSR1);
    }
}




//NODEFER FLAG
void testNodefer(struct sigaction action){
    printf("\nTesting SA_NODEFER flag:\n");
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = OgresAreLikeOnions;
    action.sa_flags = SA_NODEFER;
    sigaction(SIGUSR1, &action, NULL);

    raise(SIGUSR1);
}


//RESETHAND FLAG
void testResetHand(struct sigaction action){
    printf("\nTesting SA_RESETHAND flag:\n");
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = OgresAreLikeOnions;
    action.sa_flags = SA_RESETHAND;
    sigaction(SIGUSR1, &action, NULL);

    raise(SIGUSR1);
}



int main(){
    setbuf(stdout, NULL);
    struct sigaction action; 
    testSiginfo(action);

    testNodefer(action);

    call = 0;
    onionLayer = 0;
    testResetHand(action);
    
}