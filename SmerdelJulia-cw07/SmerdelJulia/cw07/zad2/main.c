#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_TAB_LENGTH 100
static const char* SEM_DATA = "/sem_data";
static const char* SEM_CHAIRS = "/sem_chairs";

static sem_t* semData;
static sem_t* semChairs;

static int howManyHairdressers;

typedef struct Client{
    int time;
    int isReserved; //0 - not reserved, 1 reserved
    int id;
} Client;

typedef struct Data{
    int sleepingHairdressers[MAX_TAB_LENGTH];
    Client waitingClients[MAX_TAB_LENGTH];
    int hairdressersNum;
    int waitingRoomCapacity;
    int hairdresserLength;
    int clientsLength;
} Data;

static Data* data;

typedef union Semaphore{
    int setVal;
    struct semid_ds *buffer; /*Buffer for IPC_STAT & IPC_SET*/
    unsigned short *arrays; /*array for GETALL & SETALL*/
    struct seminfo *buffer2; /*buffer for IPC_INFO*/
} Semaphore;

Semaphore args;


sem_t* createSemaphore(const char* name, int n){

   sem_t* sem = sem_open(name, O_CREAT | O_RDWR, 0666, n);
   if (sem == SEM_FAILED){
    printf("Error opening semaphore.\n");
    exit(-1);
   }
   return sem;
}

int getSemaphoreValue(sem_t* sem){
    int val;
    sem_getvalue(sem, &val);
    return val;
}


void closeSemaphore(sem_t* sem){
    if (sem_close(sem) == -1){
        perror("Error closing semaphore");
        exit(-1);
    }
}

void deleteSemaphores(){
    if (sem_unlink(SEM_DATA) == -1){
        perror("Sem_unlink data error");
    }
    
    if (sem_unlink(SEM_CHAIRS) == -1){
        perror("Sem_unlink chairs erorr");
    }
}




void startUp(){//creating data
    int i;
    if ((i = shm_open(SEM_DATA, O_CREAT | 0666, 0)) == -1){
        printf("Error creating shared memory");
        exit(-1);
    }

    //declaring size of memory
    if ((ftruncate(i, sizeof(Data*))) == -1){
        printf("Error declaring size of shared memory");
        exit(-1);
    }

    //adding to address
    if ((data = mmap(NULL, sizeof(Data*), PROT_READ | PROT_WRITE, MAP_SHARED, i, 0)) == MAP_FAILED){
        perror("Error adding an address");
        exit(-1);
    }
}



void cleaningBeforeOpening(int hairdressersNo, int waitingNo){
    data -> hairdresserLength = 0;
    data -> clientsLength = 0;
    data -> hairdressersNum = hairdressersNo;
    data -> waitingRoomCapacity = waitingNo;

    for (int i = 0; i < MAX_TAB_LENGTH; i++){
        data -> sleepingHairdressers[i] = 0;
        data -> waitingClients[i].time = 0;
        data -> waitingClients[i].isReserved = 0;
    }
}

void addWaitingClient(int time, int id){
    if (data->hairdresserLength == data->waitingRoomCapacity){
        printf("No free spots for clients");
        exit(-1);
    }

    int idx = data -> clientsLength;
    data->waitingClients[idx].time = time;
    data->waitingClients[idx].isReserved = 0;
    data->waitingClients[idx].id = id;
    data->clientsLength++;
}

void addSleepingHairdresser(int pid){
    if (data->hairdresserLength == data->hairdressersNum){
        printf("No free spots for sleeping at work");
        exit(-1);
    }

    int idx = data -> hairdresserLength;
    data->sleepingHairdressers[idx] = pid;
    data->hairdresserLength++;
}


Client* getWaitingClient(){
    if (data->clientsLength == 0){
        return NULL;
    }

    Client* client = malloc(sizeof(Client));
    client->id = data->waitingClients[0].id;
    client->isReserved = data->waitingClients[0].isReserved;
    client->time = data->waitingClients[0].time;

    for (int i = 1; i < data->clientsLength; i++){
        data->waitingClients[i-1] = data->waitingClients[i];
    }
    data->clientsLength--;
    return client;
}

int firstHairdresser(){
    if (data->clientsLength == 0){
        return -1;
    }

    int pid = data->sleepingHairdressers[0];
    for (int i = 0; i < data->hairdresserLength; i++){
        data->sleepingHairdressers[i-1] = data->sleepingHairdressers[i];
    }
    data->hairdresserLength--;
    return pid;
}

int reserveClient(){
    for (int i = 0; i < data->clientsLength; i++){
        if (data->waitingClients[i].isReserved == 0){
            data->waitingClients[i].isReserved = 1;
            return data->waitingClients[i].time;
        }
    }
    return -1;
}

void endHandler(){
    if (data -> clientsLength == 0){
        addSleepingHairdresser(getpid());
    }
    else{
        int reservedOne = reserveClient();
        if (reservedOne == -1){
            addSleepingHairdresser(getpid());
        }
        else{
            int freeHairdresser = firstHairdresser();
            if (freeHairdresser == -1){
                freeHairdresser = getpid();
            }
            else{
                addSleepingHairdresser(getpid());
            }
            kill(freeHairdresser, SIGUSR1);
        }
    }
}


char* currTime(){
    time_t curr = time(NULL);
    struct tm *t = localtime(&curr);
    char* timeStr = malloc(sizeof(char) * 9);
    strftime(timeStr, 9, "%H:%M:%S", t);
    return timeStr;
}


void handler(){
    int val = getSemaphoreValue(semChairs);
    printf("%d is waiting, chairs: %d\n", getpid(), val);

    sem_wait(semChairs);
    printf("%d found his precious chair!\n", getpid());

    sem_wait(semData);
    Client* client = getWaitingClient();

    sem_post(semData);

    char* startTime = currTime();
    printf("Hairdresser %d started doing client %d at %s for %d s.\n", getpid(), client->id, startTime, client->time);
    sleep(client->time);

    char* endTime = currTime();

    printf("Hairdresser %d finished client %d at %s.\n", getpid(), client->id, endTime);

    free(client);
    free(startTime);
    free(endTime);

    sem_post(semChairs);

    sem_wait(semData);
    endHandler();
    sem_post(semData);
}


void exitHandler(){

    for (int i =0; i < howManyHairdressers; i++){
        wait(NULL);
    }

    closeSemaphore(semData);
    closeSemaphore(semChairs);
    deleteSemaphores();

    if (munmap(data, sizeof(Data)) == -1){
        perror ("Error detaching memory");
        exit(-1);
    }
    if (shm_unlink(SEM_DATA) == -1){
        perror("Unlink error");
        exit(-1);
    }
    exit(0);
}

void handlerSigint(){
    closeSemaphore(semData);
    closeSemaphore(semChairs);

    //detach shared memory
    if (munmap(data, sizeof(Data)) == -1){
        perror ("Error detaching memory");
        exit(-1);
    }
    exit(0);
}



int main(int argc, char** argv){
    if (argc != 4){
        printf("Wrong number of arguments. <number of hairdressers>-<number of chairs>-<waiting room capacity>");
        exit(-1);
    }

    int howManyHairdressers = atoi(argv[1]);
    int howManyChairs = atoi(argv[2]);
    int waitingRoomCapacity = atoi(argv[3]);

    if (howManyHairdressers < howManyChairs){
        printf("Number of hairdressers has to be greater than number of chairs.");
        exit(-1);
    }
    if (howManyHairdressers > MAX_TAB_LENGTH || waitingRoomCapacity > MAX_TAB_LENGTH){
        printf("MAX_TAB_LENGTH achieved");
        exit(-1);
    }
    
    setbuf(stdout, NULL);
    signal(SIGINT, exitHandler);

    startUp();
    cleaningBeforeOpening(howManyHairdressers, waitingRoomCapacity);


    semData = createSemaphore(SEM_DATA, 1);
    semChairs = createSemaphore(SEM_CHAIRS, waitingRoomCapacity);

    pid_t pid;
    printf("Aaa fryzjerzy dwa ida spac obydwa");

    for (int i =0; i<howManyHairdressers; i++){
        pid = fork();

        if (pid < 0){
            printf("Error forking");
            exit(-1);
        }

        if (pid == 0){
            sem_wait(semData);
            addSleepingHairdresser(getpid());
            sem_post(semData);
            signal(SIGUSR1, handler);
            signal(SIGINT, handlerSigint);

            while (1){
                sleep(1);
            }
        }

    }

    printf("Waiting for hairdressers to go to sleep. Some of them have insomnia, give them some time\n");

    while (1){
        sem_wait(semData);
        int currHairdresser = data->hairdresserLength;
        sem_post(semData);

        if (currHairdresser != howManyHairdressers){
            sleep(1);
        }
        else{
            break;
        }
    }

    printf("Welcoming clients! Fireworks, free snacks etc.\n");
    
    int i = 0;
    while(1){
        printf("%d client came, welcome!\n", i);
        int time = rand() % 5 + 10;
        sem_wait(semData);

        if (data->clientsLength == data->waitingRoomCapacity){
            printf("Waiting room is full, sorry %d\n", i);
            sem_post(semData);
            sleep(1);
            i++;
            continue;
        }

        int chosenHairdresser = firstHairdresser();

        if (chosenHairdresser == -1){
            printf("No free hairdressers, you must wait %d\n", i);
            addWaitingClient(time, i);
        }
        else{
            printf("Client %d found hairdresser %d\n", i, chosenHairdresser);
            addWaitingClient(time, i);
            reserveClient();
            kill(chosenHairdresser, SIGUSR1);
        }
        sem_post(semData);
        sleep(1);
        i++;
    }

    return 0;

}