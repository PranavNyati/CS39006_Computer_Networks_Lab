/*
    COMPUTER NETWORKS LAB(CS39006) - SEM 6
    ASSIGNMENT 2 - PROBLEM 2
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

            /* THE CLIENT PROCESS */

int main(){

    int sockfd;  /* Socket descriptor */
    struct sockaddr_in server_addr; /* Server address is stored in this sockaddr_in structure */

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Cannot create the socket: socket() system call failed !\n");
        exit(0);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(20000);
    inet_aton("127.0.0.1", &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        perror("Unable to connect to the server: connect() system call failed !\n");
        exit(0);
    }

    printf("Successfully connected to the server.\n\n");

    char buffer[MAX_BUFF_SIZE];
    int bytes_recv = 0, total_bytes_recv = 0, i = 0, curr_cli_buff_len = MAX_BUFF_SIZE;
    char *client_buff = (char *)malloc(MAX_BUFF_SIZE * sizeof(char));
    memset(client_buff, 0, MAX_BUFF_SIZE);

    // ############ RECEIVE LOGIN MESSAGE FROM SERVER IN CHUNKS ############  => DONE

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

        if (client_buff[i - 1] == '\0')
            break;
    }

    printf("Message from the server:\n");
    printf("SERVER: %s\n", client_buff);

    // clearing the receive buffer and the client buffer
    memset(buffer, 0, MAX_BUFF_SIZE);
    memset(client_buff, 0, curr_cli_buff_len);

    // User enters the username to login to the server
    printf("\nEnter a username to login to the server (max username size = 25 chars):\n");
    scanf("%s", buffer);

    while (strlen(buffer) > 25){
        printf("Username size exceeds 25 characters. Please enter the username again.\n");
        scanf("%s", buffer);
    }

    buffer[strlen(buffer)] = '\0';
    printf("length of username = %ld\n", strlen(buffer));

    // ############ SEND USERNAME TO SERVER ############

    if (send(sockfd, buffer, strlen(buffer) + 1, 0) != strlen(buffer) + 1){
        perror("send() system call sent a different number of bytes than expected !\n");
        exit(0);
    }
    printf("\nUsername successfully sent to the server.\n");

    // clearing the receive buffer
    memset(buffer, 0, MAX_BUFF_SIZE);

    // ############ RECEIVE USERNAME MATCH STATUS FROM SERVER IN CHUNKS ############ => DONE

    // realloc the client buffer to the initial size
    client_buff = (char *)realloc(client_buff, MAX_BUFF_SIZE * sizeof(char));
    bytes_recv = 0; total_bytes_recv = 0; i = 0; curr_cli_buff_len = MAX_BUFF_SIZE;

    while(1){
        bytes_recv = recv(sockfd, buffer, MAX_BUFF_SIZE, 0);

        if (bytes_recv < 0){
            perror("Unable to receive data from the server: recv() system call failed !\n");
            exit(0);
        }

        if (bytes_recv == 0){
            printf("Server has closed the connection.\n");
            free(client_buff);
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

        if (client_buff[i - 1] == '\0')
            break;
    }

    // clearing the receiver buffer
    memset(buffer, 0, MAX_BUFF_SIZE);

    if (strcmp(client_buff, "NOT-FOUND\0") == 0){
        printf("\nInvalid username: Username not found in server's database.\n");
        printf("\nClosing the connection with the server ...\n");
        close(sockfd);
        printf("Connection closed.\n");
        free(client_buff);
        exit(0);
    }

    else if(strcmp(client_buff, "FOUND\0") == 0){
        
        printf("\nUsername matched: Successfully logged in to the server!\n\n");

        unsigned int current_len = MAX_BUFF_SIZE;
        const char* exit_str = "exit\0";
        int bytes_sent = 0, terminate_flag = 0;
        
        // clearing the client buffer
        memset(client_buff, 0, curr_cli_buff_len);
        getchar();

        while (1)  // while loop to receive user's shell commands until the user types 'exit'
        {
            printf("\nEnter the shell command to be executed on the server or type 'exit' to close connection with the server:\n");
            
            char shell_cmd[MAX_BUFF_SIZE] = {0};
            unsigned int current_len = MAX_BUFF_SIZE;
            int total_bytes_sent = 0, end_flag = 0;

            // ############ SEND SHELL CMD TO SERVER IN CHUNKS ############  => DONE

            while(fgets(shell_cmd, MAX_BUFF_SIZE, stdin) != NULL){
                size_t len = strlen(shell_cmd);

                if (shell_cmd[len-1] == '\n'){
                    shell_cmd[len-1] = '\0';
                    end_flag = 1;
                }

                if ( (bytes_sent = send(sockfd, shell_cmd, len, 0)) != len){
                    perror("send() system call sent a different number of bytes than expected !\n");
                    exit(0);
                }

                if(strcmp(shell_cmd , exit_str) == 0){     
                    terminate_flag = 1;
                    break;
                }

                total_bytes_sent += bytes_sent;
                if (end_flag == 1)
                    break;

            }

            if (terminate_flag == 1){
                printf("\nClosing the connection with the server ...\n");
                break;
            }

            printf("\nTotal no of bytes sent: %d\n", total_bytes_sent);
            
            // ############ RECEIVE RESULT OF SHELL CMD EXECUTION FROM SERVER IN CHUNKS ############ => DONE

            // realloc the client buffer to the initial size
            client_buff = (char *)realloc(client_buff, MAX_BUFF_SIZE * sizeof(char));
            bytes_recv = 0; total_bytes_recv = 0; i = 0; curr_cli_buff_len = MAX_BUFF_SIZE;

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

                if (client_buff[i - 1] == '\0')
                    break;
            }

            printf("\nResult of the execution of the shell cmd sent by the server:\n");
            printf("%s\n\n", client_buff);

            // clear the client buffer
            memset(client_buff, 0, curr_cli_buff_len);
        }
    }
    
    free(client_buff);
    close(sockfd);
    printf("Connection with the server closed.\n");

    return 0;
}