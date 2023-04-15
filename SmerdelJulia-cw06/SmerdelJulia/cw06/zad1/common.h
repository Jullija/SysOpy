#ifndef ZAD1_SHARED_H
#define ZAD1_SHARED_H

#include <time.h>

#define MAX_LENGTH 2048

typedef struct message {
    long messageType;
    char messageText[MAX_LENGTH];
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

const int MSG_SIZE = sizeof(message) - sizeof(long);

#endif //ZAD1_SHARED_H