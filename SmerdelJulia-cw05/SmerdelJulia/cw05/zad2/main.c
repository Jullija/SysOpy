#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>


double function(double x){
    return 4/(x*x+1);
}

double integral(double recWidth, double startPoint, double endPoint){
    double sum = 0.0;
    for(double i = startPoint; i < endPoint; i+=recWidth){
        sum += recWidth * function(i);
    }

    return sum;

}

int main(int argc, char** argv){
    if (argc != 3){
        printf("Wrong number of arguments given");
        exit(-1);
    }

    double recWidth = atof(argv[1]);
    int n = atoi(argv[2]); //how many processes
    double step = 1.0/n;


    int* pipes = calloc(n, sizeof(int));

    clock_t readySteadyGo = clock();

    for (int i = 0; i < n; i++){
        int fd[2];
        pipe(fd);
        pid_t pid = fork();

        if(pid == 0){ //child -> will write in pipe
            close(fd[0]); //closing read end
            double startPoint = i * step;
            double endPoint = (i+1) * step;
            double res = integral(recWidth, startPoint, endPoint);
            write(fd[1], &res, sizeof(double));//now write the res into fd[1]
            
            exit(0);
        }
        else{ //mommy -> she will read the pipe
            close(fd[1]);
            pipes[i] = fd[0];
        }
        
    }

    while(wait(NULL) > 0);

    //adding everything together
    double finalCountdown = 0.0;
    for(int i = 0; i < n; i++){
        double result;
        read(pipes[i], &result, sizeof(double));
        finalCountdown += result;
    }
    
    free(pipes);

    clock_t IHaveHeartAttack = clock();
    double newWorldRecord = (double)(IHaveHeartAttack - readySteadyGo)/(CLOCKS_PER_SEC);
    printf("RecWidth: %lf\nNumber of processes: %d\nFinal answer: %lf\nTime: %lf\n\n", recWidth, n, finalCountdown, newWorldRecord);


    return 0;
}