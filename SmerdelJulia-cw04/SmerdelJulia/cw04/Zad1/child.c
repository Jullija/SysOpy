#include<stdio.h>
#include<string.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>


void childSignal(char * command){

    if(strcmp(command, "ignore") == 0){
        raise(SIGUSR1);
        printf("Signal ignored\n");
        
    } else if(strcmp(command, "handler") == 0){
        printf("Handler test not supperote\n");
    } else if(strcmp(command, "mask") == 0){
        raise(SIGUSR1);
        printf("maska sie dalej robi brr\n");

    } else if((strcmp(command, "pending") == 0)){
        ;sigset_t currmask;
        sigemptyset(&currmask);
        sigpending(&currmask);
        if (sigismember(&currmask, SIGUSR1)){
            printf("signal is pending\n");
        } else{
            printf("signal is not pending\n");
        }
    } else{
        fprintf(stderr, "Invalid input arguments. Should be: <command: {ignore, handler, mask, pending}>\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char ** argv){
    if (argc != 2) {
        fprintf(stderr, "Invalid input arguments. Should be: %s <command: {ignore, handler, mask, pending}>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    childSignal(argv[1]);
}