#include "common.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <mqueue.h>


#define LIST_COMMAND "LIST"
#define ALL_CLIENTS_COMMAND "2ALL"
#define ONE_CLIENT_COMMAND "2ONE"
#define STOP_COMMAND "STOP"


int clientQueue = 0;
int serverQueue = 0;
int id = -1;
pid_t childPid = -1;
pid_t parentPid;



int getQueueName(char * name){
    char* homePath = getenv("HOME");
    if (!homePath) {
        fprintf(stderr, "Unable to get HOME path value.\n");
        return -1;
    }

    char homePathCopy[MAX_BODY_LENGTH];
    strcpy(homePathCopy, homePath);
    char* rest = "";
    if (!strtok_r(homePathCopy, "/", &rest)) {
        fprintf(stderr, " There are no more tokens in a string.\n");
        return -1;
    }
    sprintf(name, "/%s%d", rest, getpid());

    return 0;
}



void exitHandler(){
    if (getpid() == parentPid){
        if (childPid > 0){
            kill(childPid, SIGKILL);
        }

        if (mq_close(serverQueue) == -1){
            perror("Cannot close server queue.\n");
            exit(-1);
        }

         if (mq_close(clientQueue) == -1){
            perror("Cannot close client queue.\n");
            exit(-1);
        }

        char name[MAX_BODY_LENGTH];
        if (getQueueName(name) == -1){
            exit(-1);
        }

        if (mq_unlink(name) == 0){ //deleting a queue
            printf("Old message queue '%s' deleted.\n", name);
        }
        else if (errno != ENOENT){
            perror("Unable to unlink the old message queue.\n");
            exit(-1);
        }

        exit(0);
    }
}



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
    char name[MAX_BODY_LENGTH];

    if (getQueueName(name) == -1){
        return -1;
    }

    printf("Client queue '%s'\n", name);
    if ((clientQueue = createQueue(name, MAX_CLIENT_MSG_COUNT)) == -1 ){
        return -1;
    }

    if ((serverQueue = mq_open(SERVER_QUEUE_PATH, O_WRONLY)) == -1) {
        perror("Unable to get a server queue identifier.\n");
        exit(-1);
    }
    return 0;
}

int sendMsgToServer(message msg){
    return sendMsg(serverQueue, msg, msg.messageType, "Unable to send a message to a server.");
}

message* getMsgFromServer(int type){
    char msg[MAX_MSG_TOTAL_LENGTH];
    unsigned int type2 = ((unsigned int) type);
    if (mq_receive(clientQueue, msg, MAX_MSG_TOTAL_LENGTH, type < 0 ? NULL : &type2) == -1){
        perror("Cannot get message from the server queue");
        return NULL;
    }

    return parseMsg(msg);
}


int showMsg(message *msg) {
    time_t timeSent = msg->timeSent;
    struct tm *local_time = localtime(&timeSent);
    if (!local_time) {
        perror("Unable to get a local time.\n");
        return -1;
    }

    printf("Sender id: %d\nMessage type: %ld\nMessage body:\n%s\nTime of sending %d-%02d-%02d %02d:%02d:%02d\n\n",
           msg->senderID,
           msg->messageType,
           msg->messageText,
           local_time->tm_year + 1900,
           local_time->tm_mon + 1,
           local_time->tm_mday,
           local_time->tm_hour,
           local_time->tm_min,
           local_time->tm_sec
    );
    return 0;
}


int sendINIT() {
    char name[MAX_BODY_LENGTH];
    if (getQueueName(name) == -1){
        return -1;
    }
    message msg = {
        .messageType = INIT,
    };
    sprintf(msg.messageText, "%s", name);

    if (sendMsgToServer(msg) == -1) {
        return -1;
    }

    message *answer;
    if (!(answer = getMsgFromServer(INIT))) {
        return -1;
    }

    //giving client's id
    id = (int) strtol(answer->messageText, NULL, 10);
    if (id == 0 && errno){
        perror("Cannot convert the message into number");
        free(answer);
        return -1;
    }

    printf("Client was assigned '%d' id.\n\n", id);
    free(answer);

    return 0;
}

//STOP command given
void sendSTOP() {

    if (getpid() != parentPid){
        kill(parentPid, SIGINT);
    }
    else{
        puts("Sending STOP message...\n");
        message msg = {
                .messageType = STOP,
                .senderID = id
        };

        int status;

        if (sendMsgToServer(msg) == -1){
            status = 1;
        }
        else{
            status = 0;
        }

        if (status == 0){
            printf("Successfully stopped client");
        }
        else{
            fprintf(stderr, "Something went wrong stopping the client");
        }

        exit(status);
        

    }

   
}

int sendONE(int receiverID, char* body) {
    puts("Sending 2ONE message...\n");
    message msg = {
            .messageType = ONE,
            .senderID = id,
            .receiverID = receiverID,
            .timeSent = time(NULL)
    };
    strcpy(msg.messageText, body);
    return sendMsgToServer(msg);
}

int sendALL(char* body) {
    puts("Sending 2ALL message...\n");
    message msg = {
            .messageType = ALL,
            .senderID = id,
            .timeSent = time(NULL)
    };
    strcpy(msg.messageText, body);
    return sendMsgToServer(msg);
}

int sendLIST(void) {
    puts("Sending LIST message...\n");
    message msg = {
            .messageType = LIST,
            .senderID = id,
    };
    if (sendMsgToServer(msg) == -1) {
        return -1;
    }

    message* answer;
    if (!(answer = getMsgFromServer(LIST))) {
        return -1;
    }

    printf("Active clients:\n%s\n", answer->messageText);
    free(answer);
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
    message* msg;
    if (!(msg = getMsgFromServer(-1))){
        return -1;
    }
    int status;

    // Get the oldest communication from the client queue
    switch (msg->messageType) {
        case ONE:
            printf("Received 2ONE message.\n");
            status = showMsg(msg);
            break;
        case ALL:
            printf("Received 2ALL message.\n");
            status = showMsg(msg);
            break;
        case STOP:
            printf("Received STOP message.\n");
            exitHandler();
            break;
        default:
            fprintf(stderr, "Unrecognized message type '%ld'.\n", msg->messageType);
            status = -1;
    }

    free(msg);
    return status;
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
        sendSTOP();
    }

    fprintf(stderr, "Command '%s' is not recognized.\n", command);
    sendSTOP();
    return -1;
}

int run(void) {
    struct mq_attr attr;

    childPid= fork();
    if (childPid < 0) {
        perror("The child process could not be created.\n");
        return -1;
    }

    // Handle input in parent proccess
    if (childPid == 0) {
        while (true) {
            if (handleQueueInput() == -1) { 
                return -1;
            }
        }
        // Handle input in a parent process
    } else {
        while (true) {
            if (mq_getattr(clientQueue, &attr) == -1) {
                perror("Unable to get information about client's queue");
                return -1;
            }

            if (attr.mq_curmsgs > 0){
                if(handleQueueMessage() == -1){
                    exit(-1);
                }
                else{
                    printf("Enter a command:\n");
                }
            }
        }
    }
}














int main() {

    parentPid = getpid();
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);

    if (atexit(exitHandler) == -1){
        perror("Cannot set exit handler");
        exit(-1);
    }
    

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