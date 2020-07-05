#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MSG_MAX_LEN 1024
#define PORT  3333

char hostname[128];


int main(int argc, char *argv[])
{
    struct addrinfo hints, *servinfo, *p; int rv;
    int numbytes;
    struct sockaddr_in my_addr, server;
    char *hello = "Hello from client"; 


    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    memset(&my_addr, 0, sizeof(my_addr));
    
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT); // ports below 1024 are reserve, can use any port up to 65535
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    struct sockaddr_in sinRemote;
    char messageTx[MSG_MAX_LEN] = "hello";
    unsigned int sin_len = sizeof(sinRemote);
		sin_len = sizeof(server);
		sendto(sockfd,
			(const char *)hello, strlen(hello),
			0,
			(struct sockaddr *) &server, sin_len);
    char buffer[MSG_MAX_LEN]; 

    int n = recvfrom(sockfd, (char *)buffer, MSG_MAX_LEN,  
                MSG_WAITALL, (struct sockaddr *) &server, 
                &sin_len); 

    buffer[n] = '\0'; 
    printf("Server : %s\n", buffer);
    close(sockfd);
    return 0;
}