#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>


int main(int argc, char** argv){
    if (argc != 2){
        fprintf(stderr, "Wrong number of arguments");
        exit(-1);
    }


    //convert string to an integer
    int num = atoi(argv[1]);

    if (num <= 0){
        fprintf(stderr, "Number has to be positive");
        exit(-1);
    }


    pid_t child_pid;
    for (int i = 0; i < num; i++){
        child_pid = fork();
        if (child_pid < 0){
            fprintf(stderr, "error forking");
            exit(-1);
        }
        if (child_pid == 0){ //if this is a child
            printf("Parent PID: %d, child PID: %d\n", getppid(), getpid()); //ppid - parentPID, pid - childPID
            exit(0);
        }
    }

    for (int i =0; i < num; i++){
        wait(NULL);
    }
    printf("argv[i]: %s\n\n", argv[1]);

}