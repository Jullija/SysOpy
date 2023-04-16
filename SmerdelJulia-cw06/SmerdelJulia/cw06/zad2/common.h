#ifndef ZAD2_LIBSHARED_H
#define ZAD2_LIBSHARED_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mqueue.h>

#define MSG_SEP "-"
#define SERVER_QUEUE_PATH "/server"

#define MAX_CLIENT_MSG_COUNT 10 // Max is 10
#define MAX_SERVER_MSG_COUNT 10 // Max is 10
#define MAX_BODY_LENGTH 1024
#define MAX_MSG_TOTAL_LENGTH MAX_BODY_LENGTH + 58 // +58 for other parameters sent with message

typedef struct message {
    long messageType;
    char messageText[MAX_BODY_LENGTH];
    int senderID;
    int receiverID;
    time_t timeSent;
} message;


typedef enum msg_type {
    STOP = 1,
    LIST = 2,
    ALL = 3,
    ONE = 4,
    INIT = 5
} msg_type;




static int parseString(char* target, char* msgOriginal) {
    char* rest;
    char* body;
    if (!(body = strtok_r(msgOriginal, MSG_SEP, &rest))) {
        fprintf(stderr, "Unable to parse string. There are no more tokens in a string.\n");
        return -1;
    }
    strcpy(target, body);
    strcpy(msgOriginal, rest);
    return 0;
}


static long parseToNum(char* msg) {
    char* rest;
    char* num_str;
    if (!(num_str = strtok_r(msg, MSG_SEP, &rest))) {
        fprintf(stderr, "Unable to parse long. There are no more tokens in a string.\n");
        return -1;
    }

    long num = strtol(num_str, NULL, 10);
    strcpy(msg, rest);
    if (num == 0 && errno) {
        perror("Unable to convert string to the number.\n");
        return -1;
    }

    return num;
}


message *parseMsg(char* msgOriginal) {
    char msgCopy[MAX_MSG_TOTAL_LENGTH];
    strcpy(msgCopy, msgOriginal);
    // <type>-<text>-<senderID>-<receiverID>-<timeSent>
    message *msg = calloc(1, sizeof(message));

    if ((msg->messageType = (int) parseToNum(msgCopy)) == -1
        || parseString(msg->messageText, msgCopy) == -1
        || (msg->senderID = (int) parseToNum(msgCopy)) == -1
        || (msg->receiverID = (int) parseToNum(msgCopy)) == -1
        || (msg->timeSent = parseToNum(msgCopy)) == -1) {
        fprintf(stderr, "Something went wrong while parsing a message string");
        free(msg);
        return NULL;
    }

    return msg;
}


void createMessage(message msg, char* buff) {
    char temp[MAX_MSG_TOTAL_LENGTH];
    sprintf(temp, "%ld%s%s%s%d%s%d%s%ld",
            msg.messageType,
            MSG_SEP,
            strlen(msg.messageText) ? msg.messageText : NULL,
            MSG_SEP,
            msg.senderID,
            MSG_SEP,
            msg.receiverID,
            MSG_SEP,
            msg.timeSent
    );

    strcpy(buff, temp);
    buff[strlen(buff)] = '\0';
}

int createQueue(char* name, int max_count) {
    struct mq_attr attr = {
            .mq_msgsize = MAX_MSG_TOTAL_LENGTH,
            .mq_maxmsg = max_count
    };

    mqd_t queue = mq_open(name, O_RDONLY | O_CREAT, 0666, &attr); //creating queue
    if (queue == -1){
        perror("Unable to create a queue.\n");
    } 
    return queue;
}

int sendMsg(int queue, message msg_obj, int type, char* error_msg) {
    char msg[MAX_MSG_TOTAL_LENGTH];
    createMessage(msg_obj, msg);

    if (mq_send(queue, msg, strlen(msg), type) == -1) {
        perror(error_msg);
        return -1;
    }
    return 0;
}





#endif //ZAD2_LIBSHARED_H