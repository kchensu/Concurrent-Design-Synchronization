#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MSG_MAX_LENGTH 1024

struct sockaddr_in my_addr;
struct hostent *h, *gethostbyname();
int sockfd;
char hostname[128];

socklen_t addr_len;


int main(int argc, char **argv){

    // passing arguments to the program (ie. ./s-talk[]
    for(int i = 0; i < argc; i++){
        printf("Port name: %d: %s\n", i, argv[i]);
    }

    // hostname
    gethostname(hostname, sizeof(hostname));
    printf("The host name is: %s\n", hostname);

  
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(6000); // ports below 1024 are reserve, can use any port up to 65535
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // clear my_addr pointer
    
    
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1){
        
        // check if is failed
        perror("listener: bind");

    }
  

    while(1){

        struct sockaddr_in their_addr;
        their_addr.sin_family = AF_INET;
        their_addr.sin_port = htons(5000);
        their_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 
        char buf[MSG_MAX_LENGTH];
        int byteRx = recvfrom(sockfd, buf, MSG_MAX_LENGTH -1 , 0, (struct sockaddr *)&their_addr, &addr_len);


        

        printf("Message receive: %s \n", buf);
        memset(&buf, 0, sizeof(buf));
        char messageTx[MSG_MAX_LENGTH];
  
        unsigned int sin_len = sizeof(their_addr);
        printf("Type your message:");
        scanf("%s", messageTx);
        sendto(sockfd, messageTx, strlen(messageTx), 0, (struct sockaddr *)&their_addr, sin_len);
        memset(&messageTx, 0, sizeof(messageTx));



    }


   
   

    close(sockfd);
    


    return 0;
}