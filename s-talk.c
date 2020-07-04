#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


struct addrinfo hints;
struct addrinfo *servinfo;
int status;
int sockfd;




int main(int argc, char **argv){

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    
    if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
        fprintf(stderr,"getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    freeaddrinfo(servinfo);






    return 0;
}