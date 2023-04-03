#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/times.h>

#define PATHNAME "path_for_ex3"

struct timespec realStart, realEnd;

void readySteadyGo(){
    clock_gettime(CLOCK_REALTIME, &realStart);
}
void IHaveHeartAttack(){
    clock_gettime(CLOCK_REALTIME, &realEnd);
}
double newWorldRecord(){
    double i = (double)(realEnd.tv_sec - realStart.tv_sec);
    double r = (double)(realEnd.tv_nsec - realStart.tv_nsec)/1e9;
    return i+r;
}

int main(int argc, char** argv){
    if (argc != 3){
        printf("Wrong number of arguments given");
        exit(-1);
    }

    double recWidth = atof(argv[1]);
    int n = atoi(argv[2]); //how many programs
    double step = 1.0/n;

    readySteadyGo();

    mkfifo(PATHNAME, 0777); //creating a pip; 0777 permissions 

    for (int i = 0; i < n; i++){
        pid_t pid = fork();

        if(pid == 0){ 
            double startPoint = i * step;
            double endPoint = (i+1) * step;

            //converting double to string which is necessary in execl
            char bufferStart[4096], bufferEnd[4096];
            snprintf(bufferStart, 4096, "%lf", startPoint);
            snprintf(bufferEnd, 4096, "%lf", endPoint);

            execl("./child", "child", argv[1], bufferStart, bufferEnd, NULL); //telling child to do the dirty stuff
        }
        
    }

    int fifOpen = open(PATHNAME, O_RDONLY); //will be reading data from this
    if (fifOpen == -1){
        printf("Error opening fifo");
        exit(-1);
    }
    //adding everything together
    double finalCountdown = 0.0;

    int curr = 0;
    char buff[4096]="";
    while (curr < n){
        size_t size = read(fifOpen, buff, 4096);
        buff[size] = 0;
        
        char* smallResult;
        smallResult = strtok(buff, "\n");
        
        for(;smallResult;){
            finalCountdown += atof(smallResult);
            smallResult = strtok(NULL, "\n");
            curr += 1;
        }
            
        

    }
    close(fifOpen);


    IHaveHeartAttack();

    printf("RecWidth: %.10lf\nNumber of processes: %d\nFinal answer: %lf\nTime: %lf\n\n", recWidth, n, finalCountdown, newWorldRecord());


    return 0;
}