#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


struct sockaddr_in my_addr;
int sockfd;




int main(int argc, char **argv){

    // passing arguments to the program (ie. ./s-talk[]
    for(int i = 0; i < argc; i++){
        printf("Argv %d: %s\n", i, argv[i]);
    }

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(2200); // ports below 1024 are reserve, can use any port up to 65535
    my_addr.sin_addr.s_addr = INADDR_ANY;

    // clear my_addr pointer
    memset(&my_addr, 0, sizeof(my_addr));
    
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1){
        
        // check if is failed
        perror("listener: bind");

    }

    close(sockfd);
    


    return 0;
}