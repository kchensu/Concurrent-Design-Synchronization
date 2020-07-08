#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "list.h"


#define MSG_MAX_LENGTH 1024
// #define PORT 2200

struct sockaddr_in my_addr;
struct hostent *h;
int sockfd;
char hostname[128];
socklen_t addr_len;


//From Brian's Workshop # 8
// created two List to keep track of the msgs to be printed and the msgs to be sent
List* printMessageList;
List* sendMessageList;

//create a mutex and a cond_var
pthread_cond_t printCond, sndCond;
pthread_mutex_t rvlock, sndlock;



//Thread which awaits input from the keyboard (add the message from keyboard and put it in a list...) # maybe use List_add?
//since we add a new message to the List, we must signal to wake up a process that got blocked because there was no messages
void* inputFromKeyboard(void* unused){
    //some code goes here
}

//Thread which awaits a UDP datagram (receive from the client) # recvfrom
void* receiveUDPDatagram(void* unused){

    // some code goes here
    
}


//Thread which prints characters to the screen (checks the list for msgs)
//use List_count? if is zero, then wait, otherwise print msg
void* printsMessages(void* unused){

    // some code goes here
}


//Thread which sends data to the remote Unix process over the network using UDP (send to client) # sendto
//check if there is a message to be sent in the list? if is zero, we make this thread wait, is there is a message waiting,
//then we will remove it from the list, copy to buffer and send it.
void * sendUDPDatagram(void * unused){
    // some code goes here
}










// https://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread_mutex_lock.html

int main(int argc, char **argv){

    printMessageList = List_create();
    sendMessageList = List_create();

    


    // need to pass in hostname, port, etc
    // argv[1] = my_port
    // argv[2] = remove machine name 
    // argv[3] = remote port number
    int local_port;
    int remote_port;
    // this host port number
    local_port = atoi(argv[1]);
    //their local name
    // convert it to an ip address
    h = gethostbyname(argv[2]);
    printf("Hostname %s\n", h->h_name);
    printf("IP address %s\n", inet_ntoa(*((struct in_addr *)h->h_addr )));
    remote_port = atoi(argv[3]);

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(local_port); // ports below 1024 are reserve, can use any port up to 65535
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // clear my_addr pointer
    
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1){
        // check if is failed
        perror("listener: bind");
    }

    //client connection
    struct sockaddr_in their_addr;
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(remote_port);
    their_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)h->h_addr )));

    char buffer[MSG_MAX_LENGTH];
    addr_len = sizeof(their_addr);
  
    while(1){

        //receive a message from client
        recvfrom(sockfd, buffer, MSG_MAX_LENGTH -1 , 0, (struct sockaddr *)&their_addr, &addr_len); 
        printf("Received from client: %s\n", buffer);
        memset(&buffer, 0, sizeof(buffer));
        printf("Send message:"); 
        char messageTx[MSG_MAX_LENGTH];
        fgets(messageTx, MSG_MAX_LENGTH, stdin);

        //check for '!' to terminate both programs
        sendto(sockfd, messageTx, strlen(messageTx), 0, (struct sockaddr *)&their_addr, addr_len);
        if (strcmp(messageTx, "!\n") == 0){
            printf("Terminating the app.....\n");
            exit(0);
        }
        memset(&messageTx, 0, sizeof(messageTx));

        pthread_t printThread;
        pthread_create(&printThread, NULL, printsMessages, NULL);
        pthread_join(printThread, NULL);


    }
    

    close(sockfd);
}