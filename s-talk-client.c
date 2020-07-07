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
struct hostent *h;
int local_port;
int remote_port;


int main(int argc, char **argv){

    // argv[1]
    local_port = atoi(argv[1]);
    //argv[2] remote name
    h = gethostbyname(argv[2]);
    //argv[3] remote port
    remote_port = atoi(argv[3]);

    printf("Hostname: %s\n", h->h_name);
    printf("IP address: %s\n", inet_ntoa(*((struct in_addr *)h->h_addr )));




    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(local_port);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);


    struct sockaddr_in their_addr;
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(remote_port);
    their_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)h->h_addr )));
    

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