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

#include <time.h>

#define MAX_BUFF_SIZE 50

int main(int argc, char* argv[]){

    int server_port_no;

    if(argc == 2)
        server_port_no = (int) strtol(argv[1], (char **)NULL, 10);

    else {
        printf("Number of cmd line arguments not as expected. Exiting program ...\n");
        exit(0);
    }

    int sockfd, newsockfd;  /* Socket descriptors */
    struct sockaddr_in server_addr, loadb_addr;

    time_t curr_date;
    time(&curr_date);
    srand(time(0));

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Cannot create the socket: socket() system call failed !\n");
        exit(0);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server_port_no);

    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        perror("Unable to bind local address: bind() system call failed !\n");
        exit(0);
    }

    listen(sockfd, 5);

    int loadb_addr_len;
    int total_bytes_recv = 0, bytes_recv = 0, i= 0, curr_serv_buff_len = MAX_BUFF_SIZE;
    char *serv_buff = (char *)malloc(MAX_BUFF_SIZE *sizeof(char));
    char buffer[MAX_BUFF_SIZE];

    while(1){

        loadb_addr_len = sizeof(loadb_addr);

        printf("\nWaiting for a connection request from the load balancer...\n");

        if ( (newsockfd = accept(sockfd, (struct sockaddr *) &loadb_addr, &loadb_addr_len)) < 0){
            perror("Accept failed: accept() system call failed !\n");
            exit(0);
        }
        printf("\nConnection established with the load balancer.\n");

        serv_buff = (char *)realloc(serv_buff, MAX_BUFF_SIZE*sizeof(char));
        bytes_recv = 0; total_bytes_recv = 0; i = 0; curr_serv_buff_len = MAX_BUFF_SIZE;

        while(1){
            bytes_recv = recv(newsockfd, buffer, MAX_BUFF_SIZE, 0);

            if (bytes_recv < 0){
                perror("Unable to receive data from the load balancer: recv() system call failed !\n");
                exit(0);
            }

            if (bytes_recv == 0){
                printf("Load balancer has closed the connection.\n");
                free(serv_buff);
                close(newsockfd);
                exit(0);
            }

            total_bytes_recv += bytes_recv;

            for (int j = 0; j < bytes_recv; j++){
                serv_buff[i++] = buffer[j];
                if (buffer[j] == '\0')
                    break;

                if (i == curr_serv_buff_len){
                    curr_serv_buff_len+= MAX_BUFF_SIZE;
                    serv_buff = (char *)realloc(serv_buff, curr_serv_buff_len*sizeof(char));
                }
            }

            if (serv_buff[i-1] == '\0')
                break;
        }

        // clearing the receiver buffer
        memset(buffer, 0, MAX_BUFF_SIZE);

        // SEND LOAD IF ASKED TO SEND THE LOAD
        if (!strcmp(serv_buff, "Send Load\0")){

            int load = (rand() % 100) + 1;
            snprintf(buffer, MAX_BUFF_SIZE, "%d", load);
            if (send(newsockfd, buffer, strlen(buffer) + 1, 0) != strlen(buffer) + 1){
                perror("send() system call sent a different number of bytes than expected !\n");
                exit(0);
            }
            printf("Load sent: %d\n", load);
        }

        // ELSE SEND THE TIME IF ASKED TO SEND TIME
        else if(!strcmp(serv_buff, "Send Time\0")){
            time_t now = time(0);
            struct tm tstruct = *localtime(&now);
            char time_buff[1000];
            strftime(time_buff, sizeof(time_buff), "%x - %I:%M:%S %p", &tstruct);
            int bytes_sent = 0, total_bytes_sent = 0, end_flag = 0, i = 0;

            while(1){

                for (int j = 0; j < MAX_BUFF_SIZE ; j++){
                    buffer[j] = time_buff[i++];
                    if (buffer[j] == '\0'){
                        end_flag = 1;
                        break;
                    }
                }

                if ( (bytes_sent = send(newsockfd, buffer, MAX_BUFF_SIZE, 0)) != MAX_BUFF_SIZE ){
                    perror("send() system call sent a different number of bytes than expected !\n");
                    exit(0);
                }
                total_bytes_sent += bytes_sent;

                if (end_flag == 1)
                    break;
            }


            printf("\nDate and time info sent to the load balancer.\n");
        }

        // Reset both the buffers
        memset(buffer, 0, MAX_BUFF_SIZE);
        memset(serv_buff, 0, curr_serv_buff_len);

    }

    close(sockfd);
    return 0;
}