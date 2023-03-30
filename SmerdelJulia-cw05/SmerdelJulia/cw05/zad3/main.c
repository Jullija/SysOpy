#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PATHNAME "path_for_ex3"



int main(int argc, char** argv){
    if (argc != 3){
        printf("Wrong number of arguments given");
        exit(-1);
    }

    double recWidth = atof(argv[1]);
    int n = atoi(argv[2]); //how many programs
    double step = 1.0/n;

    clock_t readySteadyGo = clock();

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

    // double result = 0.0;
    // int fifo = open(PATHNAME, O_RDONLY);
    // int already_read = 0;
    // char read_buff[4096] = "";
    // while (already_read < n) {
    //     size_t size = read(fifo, read_buff, 4096);
    //     read_buff[size] = 0;

    //     char delim[] = "\n";
    //     char* token;

    //     token = strtok(read_buff, delim);
    //     for (;token; token = strtok(NULL, delim)) {
    //         result += strtod(token, NULL);
    //         already_read++;
    //     }
    // }
    // close(fifo);



    //while(wait(NULL) > 0);

    int fifOpen = open(PATHNAME, O_RDONLY); //will be reading data from this
    //adding everything together
    double arr[n];
    double finalCountdown = 0.0;
    for(int i = 0; i < n; i++){
        wait(NULL);
        read(fifOpen, &arr[i], sizeof(double));
    }
    close(fifOpen);

    for(int i = 0; i < n; i++){
        finalCountdown += arr[i];
    }
    

    clock_t IHaveHeartAttack = clock();
    double newWorldRecord = (double)(IHaveHeartAttack - readySteadyGo)/(CLOCKS_PER_SEC);
    printf("RecWidth: %lf\nNumber of processes: %d\nFinal answer: %lf\nTime: %lf\n\n", recWidth, n, finalCountdown, newWorldRecord);


    return 0;
}