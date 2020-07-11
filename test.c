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


#define MSG_MAX_LENGTH 1024
struct addrinfo h_in, *result_in; //h_in points to a struct addrinfo
struct addrinfo h_out, *result_out;
struct hostent *h;
int sockfd;

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






int main(int argc, char **argv)
{

    pthread_t waitKeyboardInput;
    pthread_t waitUDPdatagram;
    pthread_t printCharacters;
    pthread_t sendDataOver;
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

    // http://beej.us/guide/bgnet/pdf/bgnet_usl_c_2.pdf page 26
    getaddrinfo(NULL, argv[1], &h_in, &result_in);
    //make socket
    sockfd = socket(result_in->ai_family, result_in->ai_socktype, result_in->ai_protocol);

    // bind it to the port we passed in to addrinfo

    bind(sockfd, result_in->ai_addr, result_in->ai_addrlen);

    // hostname
    printf("Hostname: %s\n", h->h_name);
    //host IP
    h = gethostbyname(argv[2]);
    printf("IP address: %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));

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
    pthread_join(&waitKeyboardInput, NULL);
    pthread_join(waitUDPdatagram, NULL);
    pthread_join(printCharacters, NULL);
    pthread_join(sendDataOver, NULL);

    return 0;
    

        




















}
