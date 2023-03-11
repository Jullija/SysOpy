#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

//if using dynamic library
#ifndef NO_DYNAMIC_LIB  
#include <dlfcn.h>
#include "../zad1/myLibrary.h"
#else
#include "../zad1/myLibrary.h"
#endif

#define BUFFER_SIZE 100


clock_t startTime, endTime;
struct tms cpuStart, cpuEnd;

clock_t startTimeFull, endTimeFull;
struct tms cpuStartFull, cpuEndFull;

void startTimer(){
    startTime = times(&cpuStart);
}

void stopTimer(){
    endTime = times(&cpuEnd);
}

double diffTime(clock_t end, clock_t start){
    return ((double)(end - start) / (double) sysconf(_SC_CLK_TCK));
}

void startTimerFull(){
    startTimeFull = times(&cpuStartFull);
}

void stopTimerFull(){
    endTimeFull = times(&cpuEndFull);
}

void printTime(){
    // rzeczywisty
    printf("real time: %.10f\n", diffTime(endTime, startTime));
    // użytkownika
    printf("user time: %.10f\n", diffTime(cpuEnd.tms_cutime,cpuStart.tms_cutime));
    // systemowy
    printf("core time: %.10f\n", diffTime(cpuEnd.tms_cstime,cpuStart.tms_cstime));
}

void printTimeFull(){
    // rzeczywisty
    printf("real time: %.10f\n", diffTime(endTimeFull, startTimeFull));
    // użytkownika
    printf("user time: %.10f\n", diffTime(cpuEndFull.tms_cutime,cpuStartFull.tms_cutime));
    // systemowy
    printf("core time: %.10f\n", diffTime(cpuEndFull.tms_cstime,cpuStartFull.tms_cstime));
}

int main(){

    #ifndef NO_DYNAMIC_LIB

    void * handle = dlopen("../zad1/libmyLibraryShared.so", RTLD_LAZY);
    
    if (!handle){
        fprintf(stderr, "Error: %s\n", dlerror());
        return 1;
    }

    pointerTable *(*createPointerTable)(int) = dlsym(handle, "createPointerTable");
    void (*fillPointerTable)(pointerTable *, char[]) = dlsym(handle, "fillPointerTable");
    char *(*contentOnIndex)(pointerTable *, int) = dlsym(handle, "contentOnIndex");
    void (*deleteOnIndex)(pointerTable *, int) = dlsym(handle, "deleteOnIndex");
    void (*deleteAll)(pointerTable *) = dlsym(handle, "deleteAll");
    

    // #else
    
    #endif

    pointerTable * table;

    char buffer[BUFFER_SIZE];

    startTimerFull();

    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL){
        char * command;
        command = strtok(buffer, " " ); //cutting the given command to get command and argument
        char * rest = strtok(NULL, " ");
        if (strcmp(command, "init") == 0){
            printf("Init operation:\n");
            startTimer();
            table = createPointerTable(atoi(rest));
            stopTimer();
            printTime();

        }

        else if (strcmp(command, "count") == 0){
            printf("Count operation:\n");
            startTimer();
            fillPointerTable(table, rest);
            stopTimer();
            printTime();
        }
        
        else if (strcmp(command, "show") == 0){
            printf("Show operation:\n");
            startTimer();
            printf("%s\n", contentOnIndex(table, atoi(rest)));
            stopTimer();
            printTime();
        }
        
        else if (strcmp(command, "delete") == 0){
            printf("Delete operation:\n");
            startTimer();
            deleteOnIndex(table, atoi(rest));
            stopTimer();
            printTime();

        }

        else if (strcmp(command, "deleteAll") * strcmp(command, "deleteAll\n") == 0){
            printf("DeleteAll operation:\n");
            startTimer();
            while (table->currSize != 0){
                deleteOnIndex(table, 0);
            }
            stopTimer();
            printTime();
        }

        else if (strcmp(command, "destroy") * strcmp(command, "destroy\n") == 0){ //newline reading
            printf("Destroy operation:\n");
            startTimer();
            deleteAll(table);
            stopTimer();
            printTime();
        }
        else{
            printf("Incorrect command!\n");
        }
    }
    printf("Full runtime of programm:\n");
    stopTimerFull();
    printTimeFull();

    #ifndef NO_DYNAMIC_LIB

    dlclose(handle);

    #endif

}