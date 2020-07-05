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

struct sockaddr_in my_addr;
struct hostent *h;
int sockfd;
char hostname[128];




int main(int argc, char **argv){

    // passing arguments to the program (ie. ./s-talk[]
    for(int i = 0; i < argc; i++){
        printf("Argv %d: %s\n", i, argv[i]);
    }
    // hostname
    gethostname(hostname, sizeof(hostname));
    printf("The host name is: %s\n", hostname);
    int mySocket = (int)argv[1];
    int theirsocket = (int)argv[2];
  
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    
    // clear my_addr pointer
    memset(&my_addr, 0, sizeof(my_addr));

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(mySocket); // ports below 1024 are reserve, can use any port up to 65535
    my_addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1){
        // check if is failed
        perror("listener: bind");
    }

    struct sockaddr_in sinRemote;
            unsigned int sin_len = sizeof(sinRemote);
            char messageRx[MSG_MAX_LEN];
            int bytesRx = recvfrom(sockfd,
                messageRx, MSG_MAX_LEN, 0,
                (struct sockaddr *) &sinRemote, &sin_len);

    printf("Message received (%d bytes): \n\n'%s'\n", bytesRx, messageRx);
	long remotePort = ntohs(sinRemote.sin_port);
    printf("(Port %ld) %s", remotePort, messageRx);

	char messageTx[MSG_MAX_LEN];
    sin_len = sizeof(sinRemote);
		sendto( sockfd,
			messageTx, strlen(messageTx),
			0,
			(struct sockaddr *) &sinRemote, sin_len);

    close(sockfd);
    return 0;
}