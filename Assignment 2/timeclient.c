/*
    COMPUTER NETWORKS LAB(CS39006) - SEM 6
    ASSIGNMENT 2 - PROBLEM 1
    NAME : PRANAV NYATI
    ROLL : 20CS30037
*/

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <poll.h>

#define MAX_BUFF_SIZE 200

int main(){

    int sockfd;     /* Socket descriptor */
    struct sockaddr_in server_addr;
    struct pollfd fdset[1];
    int nfds = 1, timeout = 10000, try_ctr = 0;

    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Cannot create the socket: socket() system call failed !\n");
        exit(EXIT_FAILURE);  
    }

    memset(&server_addr, 0, sizeof(server_addr));

    // server information
    server_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &server_addr.sin_addr);
    server_addr.sin_port = htons(8080);

    memset(fdset, 0, sizeof(fdset));
    fdset[0].fd = sockfd;
    fdset[0].events = POLLIN;

    int bytes_recv = 0;
    socklen_t server_addr_len = sizeof(server_addr);

    char buffer[MAX_BUFF_SIZE];

    while(1){

        printf("Attempt no. %d\n\n", try_ctr + 1);

        printf("Sending a HELLO message to the server...\n");

        strcpy(buffer, "Hello, this is a client.\n");
        if (sendto(sockfd, buffer, strlen(buffer) + 1, 0, (struct sockaddr *) &server_addr, sizeof(server_addr)) != strlen(buffer) + 1){
            perror("send() system call sent a different number of bytes than expected !\n");
            exit(0);
        }
        

        int ret = poll(fdset, nfds, timeout);

        if (ret < 0){
            perror("poll() system call failed !\n");
            exit(EXIT_FAILURE);
        }

        if (ret == 0){
            printf("\nTimeout occured! No data from server yet. Trying again...\n\n");
            try_ctr++;
            if (try_ctr >= 5){
                printf("No response from server. Timeout exceeded. Exiting...\n");
                break;
            }
            continue;
        }

        if (fdset[0].revents & POLLIN){
            if ((bytes_recv = recvfrom(sockfd, (char *)buffer, MAX_BUFF_SIZE, 0, (struct sockaddr *) &server_addr, &server_addr_len)) < 0){
                perror("Unable to receive message from client: recvfrom() system call failed !\n");
                exit(EXIT_FAILURE);
            }
            printf("\nMessage from the server:\n");
            printf("SERVER: %s\n", buffer);
            break;
        }

        else if (fdset[0].revents != POLLIN){
            printf(" Error! revents = %d\n", fdset[0].revents);
            exit(EXIT_FAILURE);
        }
    }

    close(sockfd);
    printf("\nSocket closed successfully!\n");

    return 0;
}