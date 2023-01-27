/*
    COMPUTER NETWORKS LAB(CS39006) - SEM 6
    ASSIGNMENT 3 - PROBLEM 1
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

#define MAX_BUFF_SIZE 50

int main(int argc, char *argv[]){
    
    int lb_port_no;

    if(argc == 2)
        lb_port_no = (int) strtol(argv[1], (char **)NULL, 10);

    else {
        printf("Number of cmd line arguments not as expected. Exiting program ...\n");
        exit(0);
    }

    int sockfd;  /* Socket descriptor */
    struct sockaddr_in server_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0){
        perror("Cannot create a socket: socket() system call failed !\n");
        exit(0);
    }

    server_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &server_addr.sin_addr);
    server_addr.sin_port = htons(lb_port_no);

    // ################# CONNECT TO THE LOAD BALANCER  ################
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("Unable to connect to the specified server: connect() system call failed !\n");
        exit(0);
    }
    printf("Successfully connected to the load balancer.\n\n");

    // ############ RECEIVE SERVER DATE AND TIME IN CHUNKS VIA LOAD BALANCER ############
   
    char buffer[MAX_BUFF_SIZE];
    int bytes_recv = 0, total_bytes_recv = 0, i = 0, curr_cli_buff_len = MAX_BUFF_SIZE;
    char *client_buff = (char *)malloc(MAX_BUFF_SIZE * sizeof(char));
    memset(client_buff, 0, MAX_BUFF_SIZE);

    while(1){
        bytes_recv = recv(sockfd, buffer, MAX_BUFF_SIZE, 0);

        if (bytes_recv < 0){
            perror("Unable to receive data from the server: recv() system call failed !\n");
            exit(0);
        }

        if (bytes_recv == 0){
            printf("Server has closed the connection.\n");
            close(sockfd);
            exit(0);
        }
        total_bytes_recv += bytes_recv;

        for (int j = 0; j < bytes_recv; j++){
            client_buff[i++] = buffer[j];
            if (buffer[j] == '\0')
                break;

            if (i == curr_cli_buff_len){
                curr_cli_buff_len += MAX_BUFF_SIZE;
                client_buff = (char *)realloc(client_buff, curr_cli_buff_len * sizeof(char));
            }
        }

        if (client_buff[i-1] == '\0')
            break;
    }

    printf("Message received from the server:\n");
    printf("%s\n", client_buff);

    close(sockfd);
    return 0;
}