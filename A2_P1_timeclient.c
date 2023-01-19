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

int main(){

    int sockfd;     /* Socket descriptor */
    struct sockaddr_in server_addr;

    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Cannot create the socket: socket() system call failed !\n");
        exit(EXIT_FAILURE);  
    }

    memset(&server_addr, 0, sizeof(server_addr));

    // server information
    server_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &server_addr.sin_addr);
    server_addr.sin_port = htons(8080);

    int bytes_recv = 0;
    socklen_t server_addr_len = sizeof(server_addr);

    char buffer[200];

    strcpy(buffer, "Hello, this is a client.\n");
    if (sendto(sockfd, buffer, strlen(buffer) + 1, 0, (struct sockaddr *) &server_addr, sizeof(server_addr)) != strlen(buffer) + 1){
        perror("send() system call sent a different number of bytes than expected !\n");
        exit(0);
    }

    printf("Hello message successfully sent to the server!\n");
    
    if ((bytes_recv = recvfrom(sockfd, (char *)buffer, 200, 0, (struct sockaddr *) &server_addr, &server_addr_len)) < 0){
        perror("Unable to receive message from client: recvfrom() system call failed !\n");
        exit(EXIT_FAILURE);
    }

    printf("Message from the server:\n");
    printf("SERVER: %s\n", buffer);

    close(sockfd);
    return 0;


}