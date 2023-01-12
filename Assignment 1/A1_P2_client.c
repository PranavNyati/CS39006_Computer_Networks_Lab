/*
    COMPUTER NETWORKS LAB(CS39006) - SEM 6
    ASSIGNMENT 1 - PROBLEM 2
    NAME : PRANAV NYATI
    ROLL : 20CS30037
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

            /* THE CLIENT PROCESS */

int main(){

    int sockfd;  /* Socket descriptor */

    struct sockaddr_in server_addr;  

    /* Opening a socket is exactly similar to the server process */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Cannot create the socket: socket() system call failed !\n");
        exit(0);
	}

   server_addr.sin_family = AF_INET;
   inet_aton("127.0.0.1", &server_addr.sin_addr);
   server_addr.sin_port = htons(20000);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("Unable to connect to the specified server: connect() system call failed !\n");
        exit(0);
    }

    const char* exit_str = "-1\0";
    int bytes_sent = 0, terminate_flag = 0;

    while(1){

        printf("\nEnter the arithmetic expression to be sent to the server(end with a newline character)\n \tOR \nEnter -1 to terminate the client process.\n");
        
        unsigned int len_max = 11;
        unsigned int current_len = 0;

        char *user_input = malloc(len_max*sizeof(char));
        current_len = len_max;
        
        int total_bytes_sent = 0, end_flag = 0;

        fflush(stdin);
        while(fgets(user_input, len_max, stdin) != NULL){


            size_t len = strlen(user_input);
            
            if (user_input[len-1] == '\n'){
                user_input[len-1] = '\0';
                end_flag = 1;
            }

            if ((bytes_sent = send(sockfd, user_input, len , 0)) != len ){
                perror("send() system call sent a different number of bytes than expected !\n");
                exit(0);
            }

            if(strcmp(user_input, exit_str) == 0){     
                terminate_flag = 1;
                break;
            }

            total_bytes_sent += bytes_sent;

            if (end_flag == 1){
                break;
            }

        }

        if (terminate_flag == 1){
            break;
        }

        printf("\nTotal no of bytes sent: %d\n", total_bytes_sent);

        char result_str[150];
        int bytes_recv = 0;

        printf("\nThe result of the expression as computed by the server:\n");
    
        if ( (bytes_recv = recv(sockfd, result_str, 150, 0)) < 0){
            perror("Unable to receive data from the server: recv() system call failed !\n");
            exit(0);
        }

        printf("Server response: %s\n", result_str);

    }

    close(sockfd);
    printf("Client process terminated.\n");

    return 0;
}


