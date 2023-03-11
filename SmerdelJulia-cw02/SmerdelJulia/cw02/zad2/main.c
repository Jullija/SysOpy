#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include<sys/times.h>

/* time */
clock_t startTimeOne, endTimeOne;
struct tms cpuStartOne, cpuEndOne;

clock_t startTimeMore, endTimeMore;
struct tms cpuStartMore, cpuEndMore;

void startTimerOne(){
    startTimeOne = times(&cpuStartOne);
}

void stopTimerOne(){
    endTimeOne = times(&cpuEndOne);
}

double diffTime(clock_t end, clock_t start){
    return ((double)(end - start) / (double) sysconf(_SC_CLK_TCK));
}

void startTimerMore(){
    startTimeMore = times(&cpuStartMore);
}

void stopTimerMore(){
    endTimeMore = times(&cpuEndMore);
}

void printTimeOne(){
    printf("One at time\n");
    // rzeczywisty
    printf("real time: %.10f\n", diffTime(endTimeOne, startTimeOne));
    // użytkownika
    printf("user time: %.10f\n", diffTime(cpuEndOne.tms_cutime,cpuStartOne.tms_cutime));
    // systemowy
    printf("core time: %.10f\n", diffTime(cpuEndOne.tms_cstime,cpuStartOne.tms_cstime));
}

void printTimeMore(){
    printf("1024 at time\n");
    // rzeczywisty
    printf("real time: %.10f\n", diffTime(endTimeMore, startTimeMore));
    // użytkownika
    printf("user time: %.10f\n", diffTime(cpuEndMore.tms_cutime,cpuStartMore.tms_cutime));
    // Moretemowy
    printf("core time: %.10f\n", diffTime(cpuEndMore.tms_cstime,cpuStartMore.tms_cstime));
}

/* time */




void reverseBuffer(char * buff, int buffSize){
    char temp;
    for (int i = 0; i < buffSize/2; i++){
        temp = buff[i];
        buff[i] = buff[buffSize-i-1];
        buff[buffSize-i-1] = temp;
    }
}

int main(int argc, char ** argv){

    FILE * inputFile = fopen(argv[1], "r");

    if (inputFile == NULL){
        fprintf(stderr, "Error opening the input file");
        exit(-1);
    }

    FILE * outputFile = fopen(argv[2], "w");

    if (outputFile == NULL){
        fprintf(stderr, "Error opening the output file");
        fclose(inputFile);
        exit(-1);
    }




    /* one byte at time*/
    #ifndef MORE_AT_TIME
    startTimerMore();
    char * buffer = malloc(2 * sizeof(char));
    buffer[1] = '\0';

    fseek(inputFile, -1, SEEK_END); //going to the end
    int size = ftell(inputFile);

    for (int i = size; i >=0 ; i--) { //from the end to the beginning
        fseek(inputFile, i, SEEK_SET); // move one "forward"
        fread(buffer, sizeof(char), 1, inputFile); //read the byte from inputFile to the buffer
        fwrite(buffer, sizeof(char), 1, outputFile); //write a byte from buffer to the outputFile 
    }
    stopTimerMore();
    printTimeOne();

    #endif





    /* 1024 bytes */
    #ifndef ONE_AT_TIME
    startTimerMore();
    char * buffer = malloc (1024*sizeof(char));

    fseek(inputFile, -1, SEEK_END);
    int size = ftell(inputFile);
    int i = size;
    int currLocation = size + 1;

    if (size > 1024){
        i -= 1023;

        while (i >= 0) {
            fseek(inputFile, i, SEEK_SET); 
            fread(buffer, sizeof(char), 1024, inputFile);
            reverseBuffer(buffer, 1024);
            fwrite(buffer, sizeof(char), 1024, outputFile);
            i -= 1024;
        }
        currLocation = ftell(inputFile) - 1024;
    }

    fseek(inputFile, 0, SEEK_SET);
    fread(buffer, sizeof(char), currLocation, inputFile);
    reverseBuffer(buffer, currLocation);
    fwrite(buffer, sizeof(char), currLocation, outputFile);

    stopTimerMore();
    printTimeMore();



    #endif

    fclose(inputFile);
    fclose(outputFile);
    free(buffer);
}