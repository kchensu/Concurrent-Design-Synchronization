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
#include <pthread.h>

#define MSG_MAX_LENGTH 1024
struct addrinfo h_in, *result_in; //h_in points to a struct my addrinfo
struct addrinfo h_out, *result_out; // h_out points to a struct their addrinfo
struct hostent *h;
int sockfd;
List* list_of_print_msgs;
List* list_of_send_msgs;
pthread_mutex_t receive_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;

// condition variable to check if there is an item on the list.
// if no item available, we would wait
pthread_cond_t print_wait;
pthread_cond_t send_wait;

void* inputFromKeyboard(void* unused);
void* receiveUDPDatagram(void* unused);
void* printsMessages(void* unused);
void* sendUDPDatagram(void* unused);


int main(int argc, char **argv)
{
    printf("Starting......\n");
    pthread_t waitKeyboardInput;
    pthread_t waitUDPdatagram;
    pthread_t printCharacters;
    pthread_t sendDataOver;
    // create two list one to print characters the other to send data over
    list_of_print_msgs = List_create();
    list_of_send_msgs = List_create();

    // http:beej.us/guide/bgnet/pdf/bgnet_usl_c_2.pdf page 26
    // in
    memset(&h_in, 0, sizeof(h_in)); // make sure the struct is empty
    h_in.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    h_in.ai_socktype = SOCK_DGRAM;  // UDP
    h_in.ai_flags = AI_PASSIVE;     // fill in IP 

    // out
    memset(&h_out, 0, sizeof(h_out)); // make sure the struct is empty
    h_out.ai_family = AF_UNSPEC;      // don't care IPv4 or IPv6
    h_out.ai_socktype = SOCK_DGRAM;   // UDP
    h_out.ai_flags = AI_PASSIVE;       // fill in IP
    
    getaddrinfo(NULL, argv[1], &h_in, &result_in);
    //make socket
    sockfd = socket(result_in->ai_family, result_in->ai_socktype, result_in->ai_protocol);

    // bind it to the port we passed in to addrinfo

    bind(sockfd, result_in->ai_addr, result_in->ai_addrlen);

    //hostname
    
    //host IP
   
    h = gethostbyname(argv[2]);
    printf("Hostname: %s\n", h->h_name);
    printf("Local port: %s\n", argv[1]);
    printf("IP address: %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));
    printf("Remote port: %s\n", argv[3]);
    // http://beej.us/guide/bgnet/pdf/bgnet_usl_c_2.pdf page 26 - 27
    getaddrinfo(inet_ntoa(*(struct in_addr *)h->h_addr_list[0]),
                argv[3], &h_out, &result_out);
    
    // create 4 threads for each function
    // Brian workshop # 8
    //One of the threads does nothing other than await input from the keyboard. 
    pthread_create(&waitKeyboardInput, NULL, inputFromKeyboard, NULL );
    
    // The other thread does nothing other than await a UDP datagram. 
    pthread_create(&waitUDPdatagram, NULL,receiveUDPDatagram, NULL);

    // There will also be a thread which prints characters to the screen.
    pthread_create(&printCharacters, NULL, printsMessages, NULL);

    // Finally a thread which sends data to the remote UNIX process over the network
    // using UDP. 
    pthread_create(&sendDataOver, NULL, sendUDPDatagram, NULL);

    // join threads
    pthread_join(waitKeyboardInput, NULL);
    pthread_join(waitUDPdatagram, NULL);
    pthread_join(printCharacters, NULL);
    pthread_join(sendDataOver, NULL);
    printf("Closing.....");
    close(sockfd);
    return 0;

}

//Thread which awaits input from the keyboard (add the message from keyboard and put it in a list...) # maybe use List_add?
//since we add a new message to the List, we must signal to wake up a process that got blocked because there was no messages
void* inputFromKeyboard(void* unused){
    char buffer[MSG_MAX_LENGTH];
    memset(&buffer, 0, sizeof(buffer));
    
    while (1) 
    {   
        read(0, buffer, MSG_MAX_LENGTH);
        buffer[MSG_MAX_LENGTH] = '\0';
        // if msg received =  !, then terminate program
        pthread_mutex_lock(&send_mutex);
        // add the send msg to the list
        List_add(list_of_send_msgs, buffer);
        // wake up the thread that got block because there was no msgs.
        pthread_cond_signal(&send_wait);
        pthread_mutex_unlock(&send_mutex);
        // if (strcmp(buffer, "!\n") == 0)
        // {
        //     printf("Terminating the app.....\n");
        //     exit(1);
        // }
     
    }
}

//Thread which awaits a UDP datagram (receive from the client) # recvfrom
void* receiveUDPDatagram(void* unused)
{
    char buffer[MSG_MAX_LENGTH];
    while(1)
    {

        while(recvfrom(sockfd, buffer, MSG_MAX_LENGTH, 0,result_out->ai_addr,&(result_out->ai_addrlen))!= -1)
        {
            pthread_mutex_lock(&receive_mutex);
            // if msg received =  !, then terminate program
            // add receive msg to the list
            List_add(list_of_print_msgs, buffer);
            // wake up process that was blocked because there was no msgs in the list
            pthread_cond_signal(&print_wait);
            pthread_mutex_unlock(&receive_mutex);    
        }
        memset(&buffer, 0, sizeof(buffer));
    }
}


//Thread which prints characters to the screen (checks the list for msgs)
//use List_count? if is zero, then wait, otherwise print msg
void* printsMessages(void* unused)
{
    char buffer[MSG_MAX_LENGTH];
   
    while(1)
    {
        pthread_mutex_lock(&receive_mutex);
        // if the list is empty, there are no msgs...so we should wait!
        if (List_count(list_of_print_msgs) == 0)
        {
            pthread_cond_wait(&print_wait, &receive_mutex);
        }
        // what if there are msgs in the list? then print them out
        while(List_count(list_of_print_msgs) > 0)
        {
            memset(&buffer, 0, sizeof(buffer));
            // printf("The count for msg list: %d\n", List_count(list_of_print_msgs));
            char * msg;
            memset(&msg, 0, sizeof(msg));
            msg = List_remove(list_of_print_msgs);
            strncpy(buffer, msg, sizeof(buffer));
            printf("from remote server: %s", buffer);
            // write(1, buffer, sizeof(buffer));

        }
        pthread_mutex_unlock(&receive_mutex);
        
    }
}

//Thread which sends data to the remote Unix process over the network using UDP (send to client) # sendto
//check if there is a message to be sent in the list? if is zero, we make this thread wait, is there is a message waiting,
//then we will remove it from the list, copy to buffer and send it.
void * sendUDPDatagram(void * unused)
{
    char buffer[MSG_MAX_LENGTH];
   
    while(1)
    {
        pthread_mutex_lock(&send_mutex);
        // check if the lost contain any msgs that needs to be sent over.
        // if no msgs then we will wait.
        if (List_count(list_of_send_msgs) == 0)
        {
            // wait here
            pthread_cond_wait(&send_wait, &send_mutex);
        }
        // if the list contain msgs that are waiting to get sent over
        while(List_count(list_of_send_msgs) > 0)
        {
            char* msg;
            memset(&msg, 0, sizeof(msg));
            // remove it from the list
            msg = List_remove(list_of_send_msgs);
            // copy it over to the buffer
            strncpy(buffer, msg, sizeof(buffer));
            
            // send data over to the remote address.
            // result_out-> ai_addr
            // result_out-> ai_addrlen
            sendto(sockfd,buffer, sizeof(buffer), 0, 
                    result_out->ai_addr, result_out->ai_addrlen);
        }
        pthread_mutex_unlock(&send_mutex);
        
       
    }
}
