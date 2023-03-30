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
    if (argc != 4){
        printf("Something went wrong. Again :) ");
        exit(-1);
    }

    double recWidth = atof(argv[1]);
    double startPoint = atof(argv[2]);
    double endPoint = atof(argv[3]);

    double res = integral(recWidth, startPoint, endPoint);

    int fifOpen = open(PATHNAME, O_WRONLY);


    write(fifOpen, &res, sizeof(double)); //writing to fifOpen what was in res
    close(fifOpen);
    return 0;

}