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
char buf[MSG_MAX_LENGTH];
socklen_t addr_len;


int main(int argc, char **argv){

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&their_addr, 0, sizeof(their_addr));
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(PORT);
    their_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    

    char buffer[MSG_MAX_LENGTH];
    while(1){
        memset(&buffer, 0, sizeof(buffer));
        printf("Send Message:");
        fgets(buffer, MSG_MAX_LENGTH, stdin);

        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&their_addr, sizeof(their_addr));
        memset(&buffer, 0, sizeof(buffer));

        //receive from Server
        recvfrom(sockfd, buffer, MSG_MAX_LENGTH-1, 0, NULL,NULL);
        printf("server response: %s\n", buffer);

    }

   
   

    close(sockfd);
    


    return 0;
}