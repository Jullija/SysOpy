#include "common.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <mqueue.h>

#define MAX_NO_CLIENTS 100

const char* RES_FILE_PATH = "report.txt";

int clientQueues[MAX_NO_CLIENTS] = { 0 };
int serverQueue = -1;





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


void sigintHandler(int sig_no) {
    printf("Received '%d' signal. Closing the server...\n", sig_no);
    exit(0);
}

struct tm* getTime(void) {
    time_t currTime = time(NULL);
    struct tm *localTime= localtime(&currTime);
    if (!localTime) {
        perror("Unable to get a local time.\n");
        return NULL;
    }
    return localTime;
}



int saveMsg(message* msg) {
    FILE *file = fopen(RES_FILE_PATH, "a");
    if (!file) {
        perror("Failed to open a results file.\n");
        return -1;
    }

    struct tm *localTime = getTime();

    if (fprintf(file, "%d-%02d-%02d %02d:%02d:%02d\nClient id: %d\nMessage type: %ld\nMessage body:\n'%s'\n\n",
                localTime->tm_year + 1900,
                localTime->tm_mon + 1,
                localTime->tm_mday,
                localTime->tm_hour,
                localTime->tm_min,
                localTime->tm_sec,
                msg->senderID,
                msg->messageType,
                msg->messageText
    ) < 0) {
        fprintf(stderr, "Unable to write data to a file.\n");
        return -1;
    }

    fclose(file);

    return 0;
}


int getFreeID(void) {
    for (int i = 0; i < MAX_NO_CLIENTS; i++) {
        if (!clientQueues[i]) return i;
    }
    return -1;
}

int initClient(char* text) {
    int id = getFreeID();
    if (id == -1) {
        fprintf(stderr, "Impossible to initialize a new client. The number of possible clients (%d) was exceeded.\n", MAX_NO_CLIENTS);
        return -1;
    }
    printf("Assigned '%d' id to the new client.\n", id);

    if (!(clientQueues[id] = mq_open(text, O_WRONLY))) {
        perror("Unable to get a client queue identifier.\n");
        return -1;
    }

    message msg = {
        .messageType = INIT
    };
    sprintf(msg.messageText, "%d", id);

    return sendMsg(clientQueues[id], msg, INIT, "Unable to send INIT.\n");
}

int stopClient(int clientID) {
    if (!clientQueues[clientID]) {
        fprintf(stderr, "Cannot stop a client. There is no client with id '%d'.\n", clientID);
        return -1;
    }
    if (mq_close(clientQueues[clientID]) == -1){
        perror("Unable to close client's queue.\n");
        exit(-1);
    }

    clientQueues[clientID] = 0;
    return 0;
}

int sendOne(int receiverID, int senderID, char* text) {
    if (!clientQueues[receiverID]) {
        fprintf(stderr, "Cannot send a message to a client. There is no client with id '%d'.\n", receiverID);
        return -1;
    }

    message msg = {
            .messageType = ONE,
            .senderID = senderID,
            .receiverID = receiverID,
            .timeSent = time(NULL)
    };
    strcpy(msg.messageText, text);

    if (sendMsg(clientQueues[receiverID], msg, ONE, "Unable to send message.\n") == -1) {
        return -1;
    }
    else{
        printf("Message from '%d' to '%d' was successfully sent.\n", senderID, receiverID);
        return 0;
    }

    
}

int sendAll(int senderID, char* text) {
    for (int receiverID = 0; receiverID < MAX_NO_CLIENTS; receiverID++) {
        if (clientQueues[receiverID] && receiverID != senderID) {
            if (sendOne(receiverID, senderID, text) == -1) {
                return -1;
            }
        }
    }

    printf("Message from '%d' was successfully sent to all remaining clients.\n", senderID);
    return 0;
}

int listAll(int senderID) {
    printf("All active clients:\n");
    char buff[MAX_BODY_LENGTH];
    char temp[32];
    buff[0] = '\0';

    for (int receiverID = 0; receiverID < MAX_NO_CLIENTS; receiverID++) {
        if (clientQueues[receiverID]) {
            printf("%d\n", receiverID);
            sprintf(temp, "%d\n", receiverID);
            strcat(buff, temp);
        }
    }

    message msg = { 
        .messageType = LIST 
        };
    strcpy(msg.messageText, buff);

    if (sendMsg(clientQueues[senderID], msg, LIST , "Unable to send a message to the receiver client.\n") == -1) {
        return -1;
    }

    puts("List message was successfully sent.");
    return 0;
}


int handleMsg(char* msgStr) {

    message* msg = parseMsg(msgStr);
    if (saveMsg(msg) == -1) {
        return -1;
    }
    int status;

    switch (msg->messageType) {
        case INIT:
            printf("Received INIT from %d client.\n", msg->senderID);
            status = initClient(msg->messageText);
            break;
        case STOP:
            printf("Received STOP from %d client.\n", msg->senderID);
            status = stopClient(msg->senderID);
            break;
        case ALL:
            printf("Received 2ALL from %d client.\n", msg->senderID);
            status = sendAll(msg->senderID, msg->messageText);
            break;
        case ONE:
            printf("Received 2ONE from %d client.\n", msg->senderID);
            status = sendOne(msg->receiverID, msg->senderID, msg->messageText);
            break;
        case LIST:
            printf("Received LIST from %d client.\n", msg->senderID);
            status = listAll(msg->senderID);
            break;
        default:
            fprintf(stderr, "Unrecognized message type '%ld'.\n", msg->messageType);
            return -1;
    }
    free(msg);
    return status;
}

int receiveMsg(int queueID) {
    char msg[MAX_MSG_TOTAL_LENGTH];
    size_t bytes = mq_receive(serverQueue, msg, MAX_MSG_TOTAL_LENGTH, NULL); 
    if (bytes == -1) {
        perror("Error while trying to receive a message.\n");
        return -1;
    }
    return handleMsg(msg);
}


int listen(int queueID) {
    printf("Starting listening to clients...\n");
    while (true) {
        if (receiveMsg(queueID) == -1) {
            return -1;
        }
    }
}





void exitHandler(void) {
    message msgObj = { 
        .messageType = STOP 
    };

    char msg[MAX_MSG_TOTAL_LENGTH];
    createMessage(msgObj, msg);

    for (int receiverID = 0; receiverID < MAX_NO_CLIENTS; receiverID++) {
        if (!clientQueues[receiverID]) {
            continue;
        } else if (mq_send(clientQueues[receiverID], msg, strlen(msg), STOP) == -1) {
            fprintf(stderr, "Unable to send a STOP message to the '%d' client.\n", receiverID);
        } else {
            printf("STOP message was successfully sent to the '%d' client.\n", receiverID);
        }
    }

    if (mq_close(serverQueue) == -1 || mq_unlink(SERVER_QUEUE_PATH)){
        perror("Unable to delete a queue");
        exit(-1);
    }
}




int main(void) {
    if (atexit(exitHandler) == -1) {
        perror("Unable to set the exit handler.\n");
        exit(-1);
    }
    if (setHandler(SIGINT, 0, sigintHandler) == -1) {
        exit(-1);
    }

    serverQueue = createQueue(SERVER_QUEUE_PATH, MAX_SERVER_MSG_COUNT);
    if (serverQueue == -1){
        exit(-1);
    }
    printf("Server queue was successfully created.\n");


    listen(serverQueue);
    return 1;
}
