#ifndef _PRINTMSG_H_
#define _PRINTMSG_H_
#include <pthread.h>
#include "list.h"

// Start background print thread
// void Receiver_init(char* rxMessage, pthread_mutex_t sharedMutexWithOtherThread);
void Print_init(List* rxList, pthread_mutex_t receiveMsgMutex);

// Stop background receive thread and cleanup
void Print_shutdown(void);

#endif /* _PRINTMSG_H_ */