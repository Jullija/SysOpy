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

#define MAX_TAB_LENGTH 100
#define HOME_PATH getenv("HOME")
#define CHAIRS_ID_SEMAPHORE 'C'
#define DATA_ID_SEMAPHORE 'D'
#define DATA_ID 'E'

static int semData;
static int semChairs;
static int dataID;
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


int createSemaphore(int semValue, char id){
    key_t key = ftok(HOME_PATH, id);
    int semID;
    if ((semID = semget(key, 1, 0666 | IPC_CREAT)) == -1){ //creating semaphores
        perror("Error creating semaphores");
        exit(-1);
    }

    args.setVal = semValue;
    if (semctl(semID, 0, SETVAL, args) == -1){ //semctl enables to set semaphore's value, get semaphore's value and remove semaphores
        perror("Error semctl");
        exit(-1);
    }

    return semID;
}

int getSemaphoreValue(int semID){
    return semctl(semID, 0, GETVAL, NULL);
}

void waitFun(int semID){
    struct sembuf request;
    request.sem_num = 0; //semaphore number
    request.sem_op = -1; //operations to do (<0 - decresing semaphore value)
    request.sem_flg = 0; // flags

    if (semop(semID, &request, 1)){ //doing operation in semaphores
        perror("Error semop");
        exit(-1);
    }
}


void post(int semID){
    struct sembuf request;
    request.sem_num = 0; //semaphore number
    request.sem_op = 1; //operations to do (> 0 - increase semaphore value)
    request.sem_flg = 0; // flags

    if (semop(semID, &request, 1)){ //doing operation in semaphores
        perror("Error semop");
        exit(-1);
    }
}

void deleteSemaphore(int semID){
    if ((semctl(semID, 0, IPC_RMID, NULL)) == -1){
        perror("Error deleting semaphore");
        exit(-1);
    }
}




void startUp(){//creating data
    key_t key;
    if ((key = ftok(HOME_PATH, DATA_ID)) == -1){
        perror("Error creating key.");
        exit(-1);
    }



    if ((dataID= shmget(key, sizeof(Data), 0666 | IPC_CREAT)) == -1){ //creating sharing memory
        perror("Error creating sharing memory");
        exit(-1);
    }


    if ((data = shmat(dataID, 0, 0)) == (void *) -1){//getting address where sharing memory is created
        perror("Error getting the addres of sharing memory");
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

void cleaningAfterAllDay(){
    if (shmdt(data) == -1){
        perror("Shmdt error");     
    }

    if (shmctl(dataID, IPC_RMID, NULL) == -1){ //removing sharing memory
        perror("Error removing sharing memory");
    }
    else{
        printf("Deleted %d data", dataID);
    }
}

void addWaitingClient(int time, int id){
    if (data->clientsLength == data->waitingRoomCapacity){
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
    if (data->hairdresserLength == 0){
        return -1;
    }

    int pid = data->sleepingHairdressers[0];
    for (int i = 1; i < data->hairdresserLength; i++){
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

    waitFun(semChairs);
    printf("%d found his precious chair!\n", getpid());

    waitFun(semData);
    Client* client = getWaitingClient();

    post(semData);

    char* startTime = currTime();
    printf("Hairdresser %d started doing client %d at %s for %d s.\n", getpid(), client->id, startTime, client->time);
    sleep(client->time);

    char* endTime = currTime();

    printf("Hairdresser %d finished client %d at %s.\n", getpid(), client->id, endTime);

    free(client);
    free(startTime);
    free(endTime);

    post(semChairs);

    waitFun(semData);
    endHandler();
    post(semData);
}


void exitHandler(){

    for (int i =0; i < howManyHairdressers; i++){
        wait(NULL);
    }

    deleteSemaphore(semData);
    deleteSemaphore(semChairs);
    cleaningAfterAllDay();
    exit(0);
}

void handlerSigint(){
    if (shmdt(data) != 0){
        perror("Error removing memory.\n");
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


    semData = createSemaphore(1, DATA_ID_SEMAPHORE);
    semChairs = createSemaphore(howManyChairs, CHAIRS_ID_SEMAPHORE);

    pid_t pid;
    printf("Aaa fryzjerzy dwa ida spac obydwa");

    for (int i =0; i<howManyHairdressers; i++){
        pid = fork();

        if (pid < 0){
            printf("Error forking");
            exit(-1);
        }

        if (pid == 0){
            waitFun(semData);
            addSleepingHairdresser(getpid());
            post(semData);
            signal(SIGUSR1, handler);
            signal(SIGINT, handlerSigint);

            while (1){
                sleep(1);
            }
        }

    }

    printf("Waiting for hairdressers to go to sleep. Some of them have insomnia, give them some time\n");

    while (1){
        waitFun(semData);
        int currHairdresser = data->hairdresserLength;
        post(semData);

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
        waitFun(semData);

        if (data->clientsLength == data->waitingRoomCapacity){
            printf("Waiting room is full, sorry %d\n", i);
            post(semData);
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
        post(semData);
        sleep(1);
        i++;
    }

    return 0;

}