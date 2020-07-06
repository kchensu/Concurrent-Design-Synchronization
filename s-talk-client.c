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
char buf[MSG_MAX_LENGTH];
socklen_t addr_len;


int main(int argc, char **argv){


    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(5000); // ports below 1024 are reserve, can use any port up to 65535
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, "127.0.0.1",&my_addr.sin_addr);


    // clear my_addr pointer
    
    
    // if (bind(sockfd, (struct sockaddr *)&their_addr, sizeof(their_addr)) == -1){
        
    //     // check if is failed
    //     perror("listener: bind");

    // }
  
  
    while(1){
        struct sockaddr_in their_addr;
        their_addr.sin_family = AF_INET;
        their_addr.sin_port = htons(6000);
        their_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        

        

       

        char messageTx[MSG_MAX_LENGTH];
        unsigned int sin_len = sizeof(their_addr);
        // printf("Enter the message you want to send to client: ");
        scanf("%s\n", messageTx);
        sendto(sockfd, messageTx, strlen(messageTx), 0, (struct sockaddr *)&their_addr, sin_len);


        


        addr_len = sizeof(their_addr);
        int byteRx = recvfrom(sockfd, buf, MSG_MAX_LENGTH -1 , 0, (struct sockaddr *)&their_addr, &addr_len);
        printf("This is from s-talk: %s", buf);
     

    }

   
   

    close(sockfd);
    


    return 0;
}