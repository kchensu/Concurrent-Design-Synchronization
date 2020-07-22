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


#define MSG_MAX_LENGTH 512
struct addrinfo h_in, *result_in; 
struct addrinfo h_out, *result_out; 
struct hostent *h;

pthread_mutex_t receive_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t waitKeyboardInput;
pthread_t waitUDPdatagram;
pthread_t printCharacters;
pthread_t sendDataOver;

pthread_cond_t print_wait;
pthread_cond_t send_wait;

List* list_of_print_msgs;
List* list_of_send_msgs;

static int sockfd;
static char* recieve_buffer = NULL;
static char* keyboard_buffer = NULL;
static int byte_Tracker;
static int cond_check = 0;

void* inputFromKeyboard(void* unused);
void* receiveUDPDatagram(void* unused);
void* printsMessages(void* unused);
void* sendUDPDatagram(void* unused);
void FreeItem(void* item);
void shutDownAll();


int main(int argc, char **argv)
{
    if(argc!=4) {
        printf("args: %d", argc);
        fprintf(stderr,"usage: enter all required parameters\n");
        return 1;
    }

    printf("Starting......\n");
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
    h_out.ai_flags = AI_PASSIVE;      // fill in IP
    
    int host_status;
    if ((host_status = getaddrinfo(NULL, argv[1], &h_in, &result_in)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(host_status));
        exit(1);
    }

    //make socket d
    if ((sockfd = socket(result_in->ai_family, result_in->ai_socktype, result_in->ai_protocol)) == -1) {
             perror("socket creation failed"); 
             exit(1);
    }

    // bind it to the port we passed in to addrinfo
    if (bind(sockfd, result_in->ai_addr, result_in->ai_addrlen) == -1) { 
        close(sockfd);
        perror("bind failed");
        exit(1);
    }

    //hostname    
    //host IP
    h = gethostbyname(argv[2]);
    if (h == NULL)
    {
        printf("Error: gethostbyname failed!\n");
        exit(1);
    }
    printf("Hostname: %s\n", h->h_name);
    printf("Local port: %s\n", argv[1]);
    printf("IP address: %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));
    printf("Remote port: %s\n", argv[3]);

    // http://beej.us/guide/bgnet/pdf/bgnet_usl_c_2.pdf page 26 - 27
    int guest_status;
    if ((guest_status = getaddrinfo(inet_ntoa(*(struct in_addr *)h->h_addr_list[0]),
                argv[3], &h_out, &result_out)) != 0) 
    {
        fprintf(stderr, "getaddrinfo (outgoing) %s\n", gai_strerror(guest_status));
        exit(1);
    }

    //fix wierd order of output to txt file
    fflush(stdout);

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
    freeaddrinfo(result_in);
    freeaddrinfo(result_out);

    return 0;
}

//Thread which awaits input from the keyboard (add the message from keyboard and put it in a list...) # maybe use List_add?
//since we add a new message to the List, we must signal to wake up a process that got blocked because there was no messages
void* inputFromKeyboard(void* unused){
    
  
    while (1) {

        if(List_count(list_of_send_msgs) == 100) {
            printf("Error: List is full exiting");
            shutDownAll();
        }
        
        keyboard_buffer = malloc(MSG_MAX_LENGTH);
        memset(keyboard_buffer, 0, sizeof(keyboard_buffer));
        byte_Tracker = read(0, keyboard_buffer, MSG_MAX_LENGTH); 
        if (byte_Tracker < 0) {
            herror("Error reading from keyboard");
        }
        // int terminateIdx = (byte_Tracker < MSG_MAX_LENGTH) ? byte_Tracker : MSG_MAX_LENGTH - 1;
        // keyboard_buffer[terminateIdx] = 0;

        if(byte_Tracker == 0){
            break;
        }
        pthread_mutex_lock(&send_mutex);
        List_last(list_of_send_msgs);
        List_add(list_of_send_msgs, keyboard_buffer);
        // wake up the thread that got block because there was no msgs.
        pthread_cond_signal(&send_wait);
        pthread_mutex_unlock(&send_mutex); 
    }
    return NULL;  
}

//Thread which sends data to the remote Unix process over the network using UDP (send to client) # sendto
//check if there is a message to be sent in the list? if is zero, we make this thread wait, is there is a message waiting,
//then we will remove it from the list, copy to buffer and send it.
void * sendUDPDatagram(void * unused)
{
    // do i need to malloc this?
    char buffer[MSG_MAX_LENGTH];
    
    char* msg;
    int end = 0;
    while(1) {
        memset(&buffer, 0, sizeof(buffer));
        pthread_mutex_lock(&send_mutex);
        // check if the lost contain any msgs that needs to be sent over.
        // if no msgs then we will wait.
        if (List_count(list_of_send_msgs) == 0) {
            // wait here
            pthread_cond_wait(&send_wait, &send_mutex);
        }
        // if the list contain msgs that are waiting to get sent over
        while(List_count(list_of_send_msgs) > 0) {
            int numbytes;
            memset(&msg, 0, sizeof(msg));
            memset(&buffer, 0, sizeof(buffer));
            // remove it from the list
            List_first(list_of_send_msgs);
            msg = List_remove(list_of_send_msgs);
            // copy it over to the buffer 
            strncpy(buffer, msg, strlen(msg));
            char* end_msg_here = strstr((buffer), "\n!\n");
            

            if(end_msg_here) {
                int endIndex = end_msg_here ? end_msg_here - buffer : -1; 

                memset(&buffer, 0, sizeof(buffer));
                strncpy(buffer, msg, endIndex + 3);
            }
            else {
                memset(&buffer, 0, sizeof(buffer));
                strncpy(buffer, msg, strlen(msg));
            }

            if (cond_check == 1)
            {
                char *p = strstr(buffer, "\n");
                int endIndex = p ? p - buffer : -1;
                if (endIndex == 0){
                    end = 1;
                    memset(buffer, 0, sizeof(buffer));
                    strncpy(buffer, msg, endIndex);


                }
                else
                {
                    cond_check = 0;
                }
            }


            char *c = strstr(buffer, "\n!");
            if (c)
            {
                cond_check = 1;

            }

           
            if ((numbytes = sendto(sockfd, buffer, sizeof(buffer), 0, result_out->ai_addr, result_out->ai_addrlen)) == -1) {
                perror("talker: sendto");
                exit(1); 
            }
            //printf("SENDING SENDING SENDING SENDING SENDING SENDING \n %s \n DONE DONE DONE DONE DONE DONE DONE\n", buffer);
            char* endApp = strstr(buffer, "\n!\n");
            if (strcmp(buffer, "!\n") == 0 || endApp != NULL || end == 1) 
            {
                free(msg);
                free(recieve_buffer);
                free(keyboard_buffer);
                recieve_buffer = NULL;
                keyboard_buffer = NULL;
                shutDownAll();
            }
            free(msg);
            memset(&buffer, 0, sizeof(buffer));
        }
        pthread_mutex_unlock(&send_mutex);

    
    }
    return NULL;
}

//list remove is good list free can be used to to free whole list,
//Thread which awaits a UDP datagram (receive from the client) # recvfrom
void* receiveUDPDatagram(void* unused)
{
    int num_bytes;
    while (1) {
        if(List_count(list_of_print_msgs) == 100) {
            printf("Error: List is full exiting");
            shutDownAll();
        }
        recieve_buffer = malloc(MSG_MAX_LENGTH);
        if ((num_bytes = recvfrom(sockfd, recieve_buffer, MSG_MAX_LENGTH, 0,result_out->ai_addr,&(result_out->ai_addrlen))) == -1) {
            perror("recv");
            exit(1);
        }
        
        pthread_mutex_lock(&receive_mutex);
        // if msg received =  !, then terminate program
        // add receive msg to the list
        List_last(list_of_print_msgs);
        List_add(list_of_print_msgs, recieve_buffer);
        // wake up process that was blocked because there was no msgs in the list
        pthread_cond_signal(&print_wait);
        pthread_mutex_unlock(&receive_mutex);
        memset(&recieve_buffer, 0, sizeof(recieve_buffer));
    }
    return NULL;
}

//Thread which prints characters to the screen (checks the list for msgs)
//use List_count? if is zero, then wait, otherwise print msg
void* printsMessages(void* unused)
{
    char buffer[MSG_MAX_LENGTH];
    char* msg;
    int end = 0;
    while(1) {
        pthread_mutex_lock(&receive_mutex);
        // if the list is empty, there are no msgs...so we should wait!
        if (List_count(list_of_print_msgs) == 0) {
            pthread_cond_wait(&print_wait, &receive_mutex);
        }

        while(List_count(list_of_print_msgs) > 0) {
            memset(&msg, 0, sizeof(msg));
            List_first(list_of_print_msgs);
            msg = List_remove(list_of_print_msgs);
            //copy msg to be printed
            strncpy(buffer, msg, sizeof(buffer));
            if (cond_check == 1)
            {
                char *p = strstr(buffer, "\n");
                int endIndex = p ? p - buffer : -1;
                if (endIndex == 0){
                    end = 1;
                    memset(buffer, 0, sizeof(buffer));
                    strncpy(buffer, msg, endIndex);
                    
                    
                }
                else
                {
                    cond_check = 0;
                }
            }
            char *c = strstr(buffer, "\n!");
            if (c)
            {
                cond_check = 1;

            }



            byte_Tracker = write(1, buffer, strlen(buffer));
            if (byte_Tracker < 0) {
                herror("Error writing to screen");
            }
            //search block for terminator
            char* endApp = strstr(buffer, "\n!\n");
            if (strcmp(buffer, "!\n") == 0 || endApp !=NULL||  end == 1) 
            {
                free(msg);
                free(recieve_buffer);
                free(keyboard_buffer);
                recieve_buffer = NULL;
                keyboard_buffer = NULL;
                shutDownAll();
            }  
        }
        pthread_mutex_unlock(&receive_mutex);
        free(msg);
        memset(&buffer, 0, sizeof(buffer));
    }
    return NULL;
}

void shutDownAll()
{
    printf("\nTerminating the app.....\n");
    pthread_cancel(waitKeyboardInput);
    pthread_cancel(waitUDPdatagram);
    pthread_cancel(printCharacters);
    pthread_cancel(sendDataOver);
    pthread_cond_destroy(&print_wait);
    pthread_cond_destroy(&send_wait);
    pthread_mutex_destroy(&receive_mutex);
    pthread_mutex_destroy(&send_mutex);
    close(sockfd);
    List_free(list_of_print_msgs,FreeItem);
    List_free(list_of_send_msgs,FreeItem);
    printf("Closing.....\n");
}

void FreeItem(void* item)
{
    free(item);
}