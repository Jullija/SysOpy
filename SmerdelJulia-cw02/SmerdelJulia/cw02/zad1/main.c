#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/times.h>
#include <time.h>


/* time */
clock_t startTimeLib, endTimeLib;
struct tms cpuStartLib, cpuEndLib;

clock_t startTimeSys, endTimeSys;
struct tms cpuStartSys, cpuEndSys;

void startTimerLib(){
    startTimeLib = times(&cpuStartLib);
}

void stopTimerLib(){
    endTimeLib = times(&cpuEndLib);
}

double diffTime(clock_t end, clock_t start){
    return ((double)(end - start) / (double) sysconf(_SC_CLK_TCK));
}

void startTimerSys(){
    startTimeSys = times(&cpuStartSys);
}

void stopTimerSys(){
    endTimeSys = times(&cpuEndSys);
}

void printTimeLib(){
    printf("Lib functions\n");
    // rzeczywisty
    printf("real time: %.10f\n", diffTime(endTimeLib, startTimeLib));
    // użytkownika
    printf("user time: %.10f\n", diffTime(cpuEndLib.tms_cutime,cpuStartLib.tms_cutime));
    // systemowy
    printf("core time: %.10f\n", diffTime(cpuEndLib.tms_cstime,cpuStartLib.tms_cstime));
}

void printTimeSys(){
    printf("Sys functions\n");
    // rzeczywisty
    printf("real time: %.10f\n", diffTime(endTimeSys, startTimeSys));
    // użytkownika
    printf("user time: %.10f\n", diffTime(cpuEndSys.tms_cutime,cpuStartSys.tms_cutime));
    // systemowy
    printf("core time: %.10f\n", diffTime(cpuEndSys.tms_cstime,cpuStartSys.tms_cstime));
}

/* time */





int getFileSizeLib(FILE * f){
    fseek(f, 0, SEEK_END);
    long int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
}

int getFileSizeSys(int f){
    int len =  lseek(f, 0, SEEK_END);
    lseek(f, 0, SEEK_SET);
    return len;
}


int main(int argc, char ** argv){
    if (argc != 5){
        fprintf(stderr, "Incorrect input arguments. Should be: [ASCI_1] [ASCI_2] [INPUT_FILE] [OUTPUT_FILE]");
        exit(-1);
    }

    char toFind = argv[1][0];
    char toChange = argv[2][0];
    

    #ifndef SYS_VERSION
     /*             lib version start        */

    startTimerLib();
    FILE * inputFile = fopen(argv[3], "r"); //opening file to read from

    if (inputFile == NULL){
        fprintf(stderr, "Error opening the input file");
        exit(-1);
    }

    FILE * outputFile = fopen(argv[4], "w"); //opening file to write in

    if (outputFile == NULL){
        fprintf(stderr, "Error opening the output file");
        fclose(inputFile);
        exit(-1);
    }

    
    int filesize = getFileSizeLib(inputFile); 

    char* buffer = malloc(filesize + sizeof(char)); //the data will be written here
    fread(buffer, filesize + 1, 1, inputFile); //reading the data to the buffer
    buffer[filesize] = '\0';

    char* changedText = malloc(filesize + sizeof(char)); // changed data will be written here

    for (int i = 0; i < strlen(buffer) + 1; i++){
        char tmp = buffer[i];
        if (tmp == toFind){
            tmp = toChange;
        }
        changedText[i] = tmp; 
    }

    fwrite(changedText, filesize, 1, outputFile); //data from changedText is being written to outputFile
    free(buffer);
    fclose(inputFile);
    fclose(outputFile);

    stopTimerLib();
    printTimeLib();

    /*               lib version end         */

    #endif



   






    #ifndef LIB_VERSION //if LIB_VERSION is not defined
    /*              sys version start           */

    startTimerSys();
    int inputFile2 = open(argv[3], O_RDONLY);
    
    if (inputFile2 == -1){
        printf("Failed to open input file\n");
        exit(1);
    }

    int outputFile2 = open(argv[4], O_WRONLY | O_CREAT);
    
    if (outputFile2 == -1){
        printf("Failed to open output file\n");
        close(inputFile2);
        exit(1);
    }

    int filesize2 = getFileSizeSys(inputFile2);

    char* buffer2 = malloc(filesize2 + sizeof(char));
    read(inputFile2, buffer2, filesize2); //red from inputFile to buffer
    buffer2[filesize2] = '\0';

    char* changedText2 = malloc(filesize2 + sizeof(char));

    for (int i = 0; i < strlen(buffer2) + 1; i++){
        char temp = buffer2[i];
        if (temp == toFind){
            temp = toChange;
        }
        changedText2[i] = temp; //write to changedText2
    }

    write(outputFile2, changedText2, filesize2); //write to outputFile2 data from changedText2

    free(buffer2);
    close(inputFile2);
    close(outputFile2);

    stopTimerSys();
    printTimeSys();


/*              sys version end          */
    #endif

}