#include<stdio.h>
#include<string.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

void signalHandler(int nothing){
    printf("Signal handled!\n");
}

void parentSignal(char * command){
    if(strcmp(command, "ignore") == 0){
        signal(SIGUSR1, SIG_IGN);
        raise(SIGUSR1);
        printf("Signal ignored\n");
        
    } else if(strcmp(command, "handler") == 0){
        signal(SIGUSR1, signalHandler);
        raise(SIGUSR1);
    } else if(strcmp(command, "mask") == 0){
        ;sigset_t newmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        sigprocmask(SIG_BLOCK, &newmask, NULL);
        raise(SIGUSR1);
        printf("is blocked\n");

    } else if((strcmp(command, "pending") == 0)){
        ;sigset_t newmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        sigprocmask(SIG_BLOCK, &newmask, NULL);
        raise(SIGUSR1);
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

void childSignal(char * command){

    if(strcmp(command, "ignore") == 0){
        raise(SIGUSR1);
        printf("Signal ignored\n");
        
    } else if(strcmp(command, "handler") == 0){
        raise(SIGUSR1);
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
    setbuf(stdout, NULL);
    if (argc != 2) {
        fprintf(stderr, "Invalid input arguments. Should be: %s <command: {ignore, handler, mask, pending}>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    printf("testin parent process:\n");
    parentSignal(argv[1]);
    
    int pid = fork();


    if (pid == 0){
        printf("\ntesting child process:\n");
        childSignal(argv[1]);
    } else{
        wait(NULL);
        printf("\nexcl version:\n");
        execl("./child", "./child", argv[1], NULL);
    }
    


}