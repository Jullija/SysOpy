#include<stdio.h>
#include<string.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>


void princessFiona(char * command){ //for child process
    if(strcmp(command, "ignore") == 0){
        raise(SIGUSR1);
        printf("-Ignoring signal just like Shrek ignored Donkey\n\n");
        
    } 
    else if(strcmp(command, "handler") == 0){
        printf("-Not supported, handler is far far away.\n\n");

    } 
    else if(strcmp(command, "mask") == 0){
        raise(SIGUSR1);
        printf("-Signal blocked, you shall not pass my swamp!\n\n");

    } 
    else if((strcmp(command, "pending") == 0)){
        ;sigset_t FarFarAway;
        sigemptyset(&FarFarAway);
        sigpending(&FarFarAway);
        if (sigismember(&FarFarAway, SIGUSR1)){
            printf("-Signal is pending, Donkey is waiting, are we there yet?\n\n");
        } else{
            printf("-Signal is not pending\n");
        }
    } else{
        fprintf(stderr, "Invalid input arguments. Should be: <command: {ignore, handler, mask, pending}>\n\n");
        exit(EXIT_FAILURE);
    }
}
int main(int argc, char ** argv){
    if (argc != 2) {
        fprintf(stderr, "Invalid input arguments. Should be: %s <command: {ignore, handler, mask, pending}>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    princessFiona(argv[1]);
}