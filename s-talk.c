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
#define PORT 2200

struct sockaddr_in their_addr;
struct hostent *h, *gethostbyname();
int sockfd;
char hostname[128];

socklen_t addr_len;


int main(int argc, char **argv){
    // need to pass in hostname, port, etc

  
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&their_addr, 0, sizeof(their_addr));
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(PORT); // ports below 1024 are reserve, can use any port up to 65535
    their_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // clear my_addr pointer
    
    
    if (bind(sockfd, (struct sockaddr *)&their_addr, sizeof(their_addr)) == -1){
        // check if is failed
        perror("listener: bind");
    }

    //client connection
    struct sockaddr_in their_addr;
    char buffer[MSG_MAX_LENGTH];
    addr_len = sizeof(their_addr);
  

    while(1){
       
        recvfrom(sockfd, buffer, MSG_MAX_LENGTH -1 , 0, (struct sockaddr *)&their_addr, &addr_len);
        printf("Received from client: %s\n:", buffer);

        memset(&buffer, 0, sizeof(buffer));
        printf("Send message:");
        
        char messageTx[MSG_MAX_LENGTH];
        fgets(messageTx, MSG_MAX_LENGTH, stdin);
        sendto(sockfd, messageTx, strlen(messageTx), 0, (struct sockaddr *)&their_addr, addr_len);
        

    }

    close(sockfd);
}