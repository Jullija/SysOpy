#include<stdio.h>
#include<string.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>


void signalHandler(int nothing){
    printf("-Signal handled, welocome to Far Far Away!\n");
}

void kingHarold(char * command){ //for parent process

//if SIG_IGN works, it should ignore raising the signal and print the message.
// If not, signal should be raised and message not printed
    if(strcmp(command, "ignore") == 0){ 
        signal(SIGUSR1, SIG_IGN); //listen to signal SIGUSR1 and when found do SIG_IGN
        raise(SIGUSR1); //send signal to current process
        printf("-Ignoring signal just like Shrek ignored Donkey\n");
        
    } 
    else if(strcmp(command, "handler") == 0){
        signal(SIGUSR1, signalHandler); //listen to signal SIGUSR1 and when found do signalHandler
        raise(SIGUSR1);

//mask - we do not want to send signal to the process. We're blocking the signal
    } 
    else if(strcmp(command, "mask") == 0){
        ;sigset_t ogreSwamp; //creating a collection
        sigemptyset(&ogreSwamp); //initialization of empty signals' collection
        sigaddset(&ogreSwamp, SIGUSR1); //adding signal SIGUSR1 to collection
        sigprocmask(SIG_BLOCK, &ogreSwamp, NULL); //creating a mask for this process. We are blocking the signals added in collection
        raise(SIGUSR1); //sending the signal that should be blocked => printf should appear
        printf("-Signal blocked, you shall not pass my swamp!\n");

    } 
    else if((strcmp(command, "pending") == 0)){
        //collection in which we're blocking the signal

        ;sigset_t ogreSwamp;
        sigemptyset(&ogreSwamp);
        sigaddset(&ogreSwamp, SIGUSR1);
        sigprocmask(SIG_BLOCK, &ogreSwamp, NULL); //blocking signals from collection
        raise(SIGUSR1); //signal should be blocked


        //collection in which we are checking blocked signals

        ;sigset_t FarquaadsCastle; //second mask in the same process
        sigemptyset(&FarquaadsCastle); //initialization of empty collection
        sigpending(&FarquaadsCastle); //checking the signals waiting for unblocking - they will be written in argument->collection(FarquaadsCastle)
        if (sigismember(&FarquaadsCastle, SIGUSR1)){ //checking if the signal is in collection
            printf("-Signal is pending, Donkey is waiting, are we there yet?\n");
        } 
        else{
            printf("-Signal is not pending\n");
        }
    } 
    else{
        fprintf(stderr, "Invalid input arguments. Should be: <command: {ignore, handler, mask, pending}>\n");
        exit(EXIT_FAILURE);
    }
}

void princessFiona(char * command){ //for child process
    if(strcmp(command, "ignore") == 0){
        raise(SIGUSR1);
        printf("-Ignoring signal just like Shrek ignored Donkey\n");
        
    } 
    else if(strcmp(command, "handler") == 0){
        raise(SIGUSR1);

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
            printf("-Signal is pending, Donkey is waiting, are we there yet?\n");
        } else{
            printf("-Signal is not pending\n");
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
    printf("Testing parent process. Oh, I mean king Harold:\n");
    kingHarold(argv[1]);
    
    int pid = fork();


    if (pid == 0){ //kiddo
        printf("\nTesting child process, princess Fiona:\n");
        princessFiona(argv[1]);
    } 
    else{ 
        wait(NULL);
        printf("\nExecl version:\n");
        execl("./child", "./child", argv[1], NULL);
    }
    


}