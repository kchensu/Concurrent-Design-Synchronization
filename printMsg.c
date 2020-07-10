#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			// for strncmp()
#include <unistd.h>			// for close()
#include <pthread.h>
#include <signal.h>
#include "list.h"
#include "printMsg.h"


#define MSG_MAX_LENGTH 1024

static pthread_t threadPID;
static pthread_mutex_t dynamicListMutex;

static char* msg;

void* printThread(void* recievedMsgs) 
{
    while(1) {

        pthread_mutex_lock(&dynamicListMutex);
        // want to read the loist in order so read the first item
        List_first(recievedMsgs);
        msg = List_remove(recievedMsgs);
        printf("Message Received: %s\n", msg);
        pthread_mutex_unlock(&dynamicListMutex);

    }
}

void Print_init(List* receivedMsgs, pthread_mutex_t rxMutex )
{
    msg = malloc(MSG_MAX_LENGTH);
    dynamicListMutex = rxMutex;

    pthread_create(
        &threadPID,         // PID (by pointer)
        NULL,               // Attributes
        printThread,        // Function
        receivedMsgs);
}

void Print_shut(void)
{
    // Cancel thread
    pthread_cancel(threadPID);

    // Waits for thread to finish
    pthread_join(threadPID, NULL);
    
    //free msg memory space
    free(msg);
}
