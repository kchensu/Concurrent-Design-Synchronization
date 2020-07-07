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
// #define PORT 2200

struct sockaddr_in my_addr;

struct hostent *h;
int sockfd;
char hostname[128];

socklen_t addr_len;


int main(int argc, char **argv){
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
    char ip_address[1024];
    




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


       
        recvfrom(sockfd, buffer, MSG_MAX_LENGTH -1 , 0, (struct sockaddr *)&their_addr, &addr_len);
        printf("Received from client: %s\n", buffer);

        memset(&buffer, 0, sizeof(buffer));
        printf("Send message:");
        
        char messageTx[MSG_MAX_LENGTH];
        fgets(messageTx, MSG_MAX_LENGTH, stdin);
        sendto(sockfd, messageTx, strlen(messageTx), 0, (struct sockaddr *)&their_addr, addr_len);
        

    }

    close(sockfd);
}