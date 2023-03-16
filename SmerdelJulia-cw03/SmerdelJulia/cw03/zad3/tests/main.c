#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


int main(int argc, char** argv){
    if (argc != 2){
        fprintf(stderr, "Wrong number of arguments given");
        exit(-1);
    }

    printf("Program name: %s", argv[0]);
    fflush(stdout); //printing buffer data before execl
    execl("/bin/ls", "ls", argv[1], NULL);

    return 0;


}
