#include "common.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


#define LIST_COMMAND "LIST"
#define ALL_CLIENTS_COMMAND "2ALL"
#define ONE_CLIENT_COMMAND "2ONE"
#define STOP_COMMAND "STOP"

int clientKey = 0;
int clientQueue = 0;
int serverQueue = 0;
int id = -1;
pid_t pid = -1;


char* getMsgText(char* rest) {
    char* body;
    if ((body = strtok(rest, "\n\0")) == NULL) {
        fprintf(stderr, "Unable to get message body. There are no more tokens in a string.\n");
        return NULL;
    }
    return body;
}

int getReceiverID(char* rest) {
    char* receiverID_str;
    char* writeHere = "";
    if ((receiverID_str = strtok_r(rest, " \0", &writeHere)) == NULL) { //saving pointer into writeHere
        fprintf(stderr, "Unable to get receiver_id. There are no more tokens in a string.\n");
        return -1;
    }

    int receiverID = (int) strtol(receiverID_str, NULL, 10);
    if (receiverID < 0 || errno) {
        fprintf(stderr, "Invalid receiver id. Expected a non-negative integer, got '%s'\n", receiverID_str);
        return -1;
    }

    strcpy(rest, writeHere);
    return receiverID;
}





int setHandler(int sig_no, int sa_flags, void* handler) {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = sa_flags;
    sa.sa_handler = handler;

    if (sigaction(sig_no, &sa, NULL) == -1) {
        perror("Action cannot be set for the signal.\n");
        return -1;
    }

    return 0;
}

int createQueues() {
    char* homePath = getenv("HOME"); //keys have to be generated based on $HOME path

    key_t serverKey = ftok(homePath, 'A'); //ftok -> generating keys' values

    if ((serverQueue = msgget(serverKey, 0)) == -1) { //msget -> getting queue id
        perror("Unable to get a server queue identifier.\n");
        exit(-1);
    }
    printf("Server queue with key '%d' was found.\n", serverKey);

    clientKey = ftok(homePath, getpid());
    if ((clientQueue = msgget(clientKey, IPC_CREAT | 0666)) == -1) {
        perror("Unable to create a client queue.\n");
        exit(-1);
    }
    printf("Client queue with '%d' key was successfully created.\n", clientKey);

    return 0;
}

int showMsg(message msg) {
    time_t timeSent = msg.timeSent;
    struct tm *local_time = localtime(&timeSent);
    if (!local_time) {
        perror("Unable to get a local time.\n");
        return -1;
    }

    printf("Sender id: %d\nMessage type: %ld\nMessage body:\n%s\nTime of sending %d-%02d-%02d %02d:%02d:%02d\n\n",
           msg.senderID,
           msg.messageType,
           msg.messageText,
           local_time->tm_year + 1900,
           local_time->tm_mon + 1,
           local_time->tm_mday,
           local_time->tm_hour,
           local_time->tm_min,
           local_time->tm_sec
    );
    return 0;
}

int sendMsg(message msg) {
    //msgsnd -> sending the message to the queue
    if (msgsnd(serverQueue, &msg, MSG_SIZE, 0) == -1) {
        perror("Unable to send a message to the server queue.\n");
        return -1;
    }
    return 0;
}

int sendINIT() {
    message msg = {
            .messageType = INIT,
    };
    sprintf(msg.messageText, "%d", clientKey);

    if (sendMsg(msg) == -1) {
        return -1;
    }

    message answer;
    //msgrcv -> receiving the message
    if (msgrcv(clientQueue, &answer, MSG_SIZE, INIT, 0) == -1) {
        perror("Unable to receive a message from the client queue.\n");
        return -1;
    }

    //giving client's id
    id = (int) strtol(answer.messageText, NULL, 10);
    printf("Client was assigned '%d' id.\n\n", id);

    return 0;
}

//STOP command given
int sendSTOP() {
    puts("Sending STOP message...\n");
    message msg = {
            .messageType = STOP,
            .senderID = id
    };

    if (pid > 0) kill(pid, SIGKILL);

    if (sendMsg(msg) == 0){
        printf("Client was successfully stopped.\n");
        exit(0);
    }
    else{
        fprintf(stderr, "Something went wrong while trying to stop a client.\n");
        exit(-1);
    }

}

int sendONE(int receiverID, char* body) {
    puts("Sending 2ONE message...\n");
    message msg = {
            .messageType = ONE,
            .senderID = id,
            .receiverID = receiverID,
    };
    strcpy(msg.messageText, body);
    return sendMsg(msg);
}

int sendALL(char* body) {
    puts("Sending 2ALL message...\n");
    message msg = {
            .messageType = ALL,
            .senderID = id
    };
    strcpy(msg.messageText, body);
    return sendMsg(msg);
}

int sendLIST(void) {
    puts("Sending LIST message...\n");
    message msg = {
            .messageType = LIST,
            .senderID = id
    };
    if (sendMsg(msg) == -1) {
        return -1;
    }

    message answer;
    if (msgrcv(clientQueue, &answer, MSG_SIZE, LIST, 0) == -1) {
        perror("Unable to receive a list of active clients from the server.\n");
        return -1;
    }

    printf("Active clients:\n%s\n", answer.messageText);
    return 0;
}

char* getCommand(char* msg) {
    printf("%s\n", msg);
    char* command = "";
    size_t length = 0;
    getline(&command, &length, stdin);

    command[strlen(command) - 1] = '\0';
    return command;
}


int handleQueueMessage(void) {
    puts("\n");
    message msg;

    // Get the oldest communication from the client queue
    if (msgrcv(clientQueue, &msg, MSG_SIZE, 0, 0) == -1) {
        perror("Unable to get communication from the client queue.\n");
        return -1;
    }

    switch (msg.messageType) {
        case ONE:
            printf("Received 2ONE message.\n");
            return showMsg(msg);
        case ALL:
            printf("Received 2ALL message.\n");
            return showMsg(msg);
        default:
            fprintf(stderr, "Unrecognized message type '%ld'.\n", msg.messageType);
            return -1;
    }
}

int handleQueueInput(void) {
    char* input = getCommand("Enter a command:");
    while (!strlen(input)) {
        puts("Input not recognized. Please try again");
        input = getCommand("Enter a command:");
    }

    char* rest;
    char* command = strtok_r(input, " \0", &rest);

    if (!strcmp(command, LIST_COMMAND)) {
        return sendLIST();

    } else if (!strcmp(command, ALL_CLIENTS_COMMAND)) {
        char* text = getMsgText(rest);
        if (!text) {
            fprintf(stderr, "Something went wrong while sending a message to all receivers.\n");
            sendSTOP();
            return -1;
        }
        return sendALL(rest);

    } else if (!strcmp(command, ONE_CLIENT_COMMAND)) {
        int receiverID; ;
        char* text ;

        if ((receiverID = getReceiverID(rest) < 0 || !(text=getMsgText(rest)))) {
            fprintf(stderr, "Something went wrong while sending a message to one receiver.\n");
            sendSTOP();
            return -1;
        }
        return sendONE(receiverID, text);

    } else if (!strcmp(command, STOP_COMMAND)) {
        return sendSTOP();
    }

    fprintf(stderr, "Command '%s' is not recognized.\n", command);
    return sendSTOP();
}

int run(void) {
    struct msqid_ds queueStats;

    pid= fork();
    if (pid < 0) {
        perror("The child process could not be created.\n");
        return -1;
    }

    // Handle queued messages in a child process
    if (pid == 0) {
        while (true) {
            if (msgctl(clientQueue, IPC_STAT, &queueStats) == -1) { //IPC_STAT -> getting stats
                perror("Unable to retrieve information about client queue.\n");
                return -1;
            }
            if (queueStats.msg_qnum) { //msg.qnum -> number of messages in a queue
                if (handleQueueMessage() == -1) {
                    exit(1);
                }
                else {
                printf("Enter a command:\n");
                }
            }
        }
        // Handle input in a parent process
    } else {
        while (true) {
            if (handleQueueInput() == -1) {
                return -1;
            }
        }
    }
}

int exitHandler(){
    //msgctl -> modification and reading queue's properties
    if (msgctl(clientQueue, IPC_RMID, NULL) == -1) {  //IPC_RMID - delete queue
        perror("Cannot remove queue.\n");
        return -1;
    }
    return 0;
}






int main() {
    atexit((void(*)(void))exitHandler);
    

    if(createQueues() == -1){
        exit(-1);
    };
    
    if (setHandler(SIGINT, 0, sendSTOP)) {
        exit(-1);
    }
    if (sendINIT() == -1) {
        fprintf(stderr, "Unable to initialize a client.\n");
        exit(-1);
    }

    // The run function will only return if there was some error
    run();
    return 1;
}