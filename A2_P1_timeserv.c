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

#include <time.h>

			/* THE SERVER PROCESS */

#define MAX_BUFF_SIZE 200

int main(){

    int sockfd;     /* Socket descriptor */

    struct sockaddr_in server_addr, client_addr;   /* Server and client address  are stored in these sockaddr_in structures */

    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Cannot create the socket: socket() system call failed !\n");
        exit(EXIT_FAILURE);   // Doubt: Why write exit(EXIT_FAILURE) and not exit(1) here ?
    }
    
    // what is the use of this memset() here ? 
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("Unable to bind local address: bind() system call failed !\n");
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port 8080.\n");

    int bytes_recv = 0;
    socklen_t client_addr_len = sizeof(client_addr);

    char buffer[MAX_BUFF_SIZE];

    while(1){

        if ((bytes_recv = recvfrom(sockfd, (char *)buffer, MAX_BUFF_SIZE, 0, (struct sockaddr *) &client_addr, &client_addr_len)) < 0){
            perror("Unable to receive message from client: recvfrom() system call failed !\n");
            exit(EXIT_FAILURE);
        }

        buffer[bytes_recv] = '\0';

        printf("Message from a client: %s\n", buffer);

        time_t now =  time(0);
        struct tm tstruct = *localtime(&now);
        strftime(buffer, sizeof(buffer), "%x - %I:%M:%S %p", &tstruct);

        if (sendto(sockfd, (const char *)buffer, strlen(buffer) + 1, 0, (const struct sockaddr *) &client_addr, client_addr_len) != strlen(buffer) + 1){
            perror("send() system call sent a different number of bytes than expected !\n");
            exit(EXIT_FAILURE);
        }

        printf("Server date and time has been successfully sent to the client.\n");


    }

    close(sockfd);
    return 0;
}

