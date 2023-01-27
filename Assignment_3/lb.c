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
#include <poll.h>

#include <time.h>

#define MAX_BUFF_SIZE 50

int main(int argc, char* argv[]){

    int clilen, loadb_port_no, serv1_port_no, serv2_port_no;

    if(argc == 4){
        loadb_port_no = (int) strtol(argv[1], (char **)NULL, 10);
        serv1_port_no = (int) strtol(argv[2], (char **)NULL, 10);
        serv2_port_no = (int) strtol(argv[3], (char **)NULL, 10);
    }

    else {
        printf("Number of cmd line arguments not as expected. Exiting program ...\n");
        exit(0);
    }

    time_t curr_date;
    time(&curr_date);
    srand(time(0));

    int sockfd, sockfd_1, newsockfd; /*Socket descriptors */
    struct sockaddr_in load_bal_addr, client_addr, server1_addr, server2_addr, chosen_serv;
    struct pollfd fdset[1];
    int nfds = 1, total_time = 5000, timeout = 5000;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Cannot create the socket: socket() system call failed !\n");
        exit(0);
    }

    // specifying the fields of sockaddr_in struct of the load-balancer as server for the clients
    load_bal_addr.sin_family = AF_INET;
    load_bal_addr.sin_addr.s_addr = INADDR_ANY;
    load_bal_addr.sin_port = htons(loadb_port_no);

    // specifying the fields of sockaddr_in struct of the server1 which is like a server for load balancer
    server1_addr.sin_family = AF_INET;
    server1_addr.sin_port = htons(serv1_port_no);
    inet_aton("127.0.0.1", &server1_addr.sin_addr);

    // specifying the fields of sockaddr_in struct of the server2 which is also like a server for load balancer
    server2_addr.sin_family = AF_INET;
    server2_addr.sin_port = htons(serv2_port_no);
    inet_aton("127.0.0.1", &server2_addr.sin_addr);

    // specifying the info of the two clients using fdset
    memset(fdset, 0, sizeof(fdset));
    fdset[0].fd = sockfd;
    fdset[0].events = POLLIN;

    if (bind(sockfd, (struct sockaddr *) &load_bal_addr, sizeof(load_bal_addr)) < 0){
        perror("Unable to bind local address: bind() system call failed !\n");
        exit(0);       
    }

    printf("The load balancer is running on port %d.\n", loadb_port_no);

    int load_serv[2];
    load_serv[0] = (rand() % 100) + 1;
    load_serv[1] = (rand() % 100) + 1;

    listen(sockfd, 5);

    char buffer[MAX_BUFF_SIZE];
    int bytes_recv = 0, total_bytes_recv = 0, i = 0, curr_lb_buff_len = MAX_BUFF_SIZE;
    char *lb_buff = (char *)malloc(MAX_BUFF_SIZE* sizeof(char));
    memset(lb_buff, 0, MAX_BUFF_SIZE);

    while(1){

        // get current system time
        time_t t1, t2;
        time(&t1);
        int ret = poll(fdset, nfds, timeout);

        time(&t2);

        if (ret < 0){
            perror("poll() system call failed!\n");
            exit(EXIT_FAILURE);
        }

        // Timeout occured
        else if (ret==0){
            printf("\nTimeout occured! No connection request received from a client yet ....\n\n");

            if ((sockfd_1 = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            perror("Cannot create the socket: socket() system call failed !\n");
            exit(0);
             }

            // Open a connection to S1 to get its load
            if (connect(sockfd_1, (struct sockaddr *) &server1_addr, sizeof(server1_addr)) < 0){
                perror("Unable to connect to server 1: connect system call failed !\n");
                exit(0);
            }

            // ask S1 to send its load 
            strcpy(buffer, "Send Load");
            if ( send(sockfd_1, buffer, strlen(buffer) + 1, 0) != strlen(buffer) + 1){
                perror("send() system call sent a different number of bytes than expected !\n");
                exit(0);
            }

            // reset the buffer and other counters
            memset(buffer, 0, MAX_BUFF_SIZE);
            lb_buff = (char *)realloc(lb_buff, MAX_BUFF_SIZE);
            bytes_recv = 0; total_bytes_recv = 0; i = 0; curr_lb_buff_len = MAX_BUFF_SIZE;

            // receive server1 load in chunks
            while(1){
                bytes_recv = recv(sockfd_1, buffer, MAX_BUFF_SIZE, 0);

                if(bytes_recv < 0){
                    perror("Unable to receive data from the server: recv() call failed !\n");
                    exit(0);
                }

                if (bytes_recv == 0){
                    printf("The server S1 has closed the connection.\n");
                    close(newsockfd);
                    exit(0);
                }

                total_bytes_recv += bytes_recv;

                for(int j = 0; j < bytes_recv; j++){
                    lb_buff[i++] = buffer[j];
                    if (buffer[j] == '\0')
                        break;

                    if (i == curr_lb_buff_len){
                        curr_lb_buff_len += MAX_BUFF_SIZE;
                        lb_buff = (char *)realloc(lb_buff, curr_lb_buff_len*sizeof(char));
                    }
                }

                if(lb_buff[i-1] == '\0')
                    break;
            }

            load_serv[0] = (int) strtol(lb_buff, (char **)NULL, 10);
            printf("Load received from server S1 (%s, %d):  %d\n", inet_ntoa(server1_addr.sin_addr), htons(server1_addr.sin_port), load_serv[0]);

            // reset the buffer
            memset(buffer, 0, MAX_BUFF_SIZE);
            memset(lb_buff, 0, curr_lb_buff_len);
            close(sockfd_1);

            // reinitialise the socket to connect to S2
            if ((sockfd_1 = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            perror("Cannot create the socket: socket() system call failed !\n");
            exit(0);
            }

            // Open a connection to S2 to get its load
            if (connect(sockfd_1, (struct sockaddr *) &server2_addr, sizeof(server2_addr)) < 0){
                perror("Unable to connect to server 2: connect system call failed !\n");
                exit(0);
            }
            
            // ask S2 to send its load 
            strcpy(buffer, "Send Load");
            if ( send(sockfd_1, buffer, strlen(buffer) + 1, 0) != strlen(buffer) + 1){
                perror("send() system call sent a different number of bytes than expected !\n");
                exit(0);
            }

            // reset the buffer and other counters
            memset(buffer, 0, MAX_BUFF_SIZE);
            lb_buff = (char *)realloc(lb_buff, MAX_BUFF_SIZE);
            bytes_recv = 0; total_bytes_recv = 0; i = 0; curr_lb_buff_len = MAX_BUFF_SIZE;

            // receive server2 load in chunks
            while(1){
                bytes_recv = recv(sockfd_1, buffer, MAX_BUFF_SIZE, 0);

                if(bytes_recv < 0){
                    perror("Unable to receive data from the server: recv() call failed !\n");
                    exit(0);
                }

                if (bytes_recv == 0){
                    printf("The server S2 has closed the connection.\n");
                    close(newsockfd);
                    exit(0);
                }

                total_bytes_recv += bytes_recv;

                for(int j = 0; j < bytes_recv; j++){
                    lb_buff[i++] = buffer[j];
                    if (buffer[j] == '\0')
                        break;

                    if (i == curr_lb_buff_len){
                        curr_lb_buff_len += MAX_BUFF_SIZE;
                        lb_buff = (char *)realloc(lb_buff, curr_lb_buff_len*sizeof(char));
                    }
                }

                if(lb_buff[i-1] == '\0')
                    break;
            }

            load_serv[1] = (int) strtol(lb_buff, (char **)NULL, 10);
            printf("Load received from server S2 (%s, %d):  %d\n", inet_ntoa(server2_addr.sin_addr), htons(server2_addr.sin_port), load_serv[1]);
            
            // reset the buffer
            memset(buffer, 0, MAX_BUFF_SIZE);
            memset(lb_buff, 0, curr_lb_buff_len);
            close(sockfd_1);

            timeout = total_time;
        }

        else if (fdset[0].revents & POLLIN){

            clilen = sizeof(client_addr);
            if ( (newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &clilen)) < 0){
                perror("Unable to accept the client's connection request: accept() system call failed !\n");
                exit(0);
            }

            printf("\nAccepted a new client's connection.\n");

            if (fork() == 0){

                close(sockfd);

                printf("Current load of S1: %d\n", load_serv[0]);
                printf("Current load of S2: %d\n", load_serv[1]);

                chosen_serv = (load_serv[0] <= load_serv[1]) ? server1_addr : server2_addr;
                printf("Chosen server: S%d\n", (load_serv[0] <= load_serv[1]) ? 1 : 2);

                if ((sockfd_1 = socket(AF_INET, SOCK_STREAM, 0)) < 0){
                    perror("Cannot create the socket: socket() system call failed !\n");
                    exit(0);
                }

                if (connect(sockfd_1, (struct sockaddr *) &chosen_serv, sizeof(chosen_serv)) < 0){
                    perror("Unable to connect to server : connect system call failed !\n");
                    exit(0);
                }

                printf("Successfully connected to server S%d !\n\n", (load_serv[0] <= load_serv[1]) ? 1 : 2);

                // reset the buffer
                memset(buffer, 0, MAX_BUFF_SIZE);
                strcpy(buffer, "Send Time");

                printf("Sending client request to chosen server S%d (%s, %d) .....\n", ((load_serv[0] <= load_serv[1]) ? 1 : 2), inet_ntoa(chosen_serv.sin_addr), htons(chosen_serv.sin_port));

                if ( send(sockfd_1, buffer, strlen(buffer) + 1, 0) != strlen(buffer) + 1){
                    perror("send() system call sent a different number of bytes than expected !\n");
                    exit(0);
                }

                // ### RECEIVE TIME FROM SERVER IN CHUNKS ### 
                memset(buffer, 0, MAX_BUFF_SIZE);
                lb_buff = (char *)realloc(lb_buff, MAX_BUFF_SIZE);
                bytes_recv = 0; total_bytes_recv = 0; i = 0; curr_lb_buff_len = MAX_BUFF_SIZE;
           
                while(1){
                    bytes_recv = recv(sockfd_1, buffer, MAX_BUFF_SIZE, 0);

                    if(bytes_recv < 0){
                        perror("Unable to receive data from the server: recv() call failed !\n");
                        exit(0);
                    }

                    if (bytes_recv == 0){
                        printf("The server has closed the connection.\n");
                        close(newsockfd);
                        exit(0);
                    }

                    total_bytes_recv += bytes_recv;

                    for(int j = 0; j < bytes_recv; j++){
                        lb_buff[i++] = buffer[j];
                        if (buffer[j] == '\0')
                            break;

                        if (i == curr_lb_buff_len){
                            curr_lb_buff_len += MAX_BUFF_SIZE;
                            lb_buff = (char *)realloc(lb_buff, curr_lb_buff_len*sizeof(char));
                        }
                    }

                    if(lb_buff[i-1] == '\0')
                        break;
                    
                }     

                printf("Time received from server S%d (%s, %d):  %s\n", ((load_serv[0] <= load_serv[1]) ? 1 : 2), inet_ntoa(chosen_serv.sin_addr), htons(chosen_serv.sin_port), lb_buff);

                // ####### SEND TIME TO CLIENT #######
                int bytes_sent = 0, total_bytes_sent = 0, end_flag = 0;
                i = 0;

                while(1){

                    for (int j = 0; j < MAX_BUFF_SIZE ; j++){
                        buffer[j] = lb_buff[i++];
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


                printf("Time sent to client.\n");

                close(sockfd_1);
                close(newsockfd);
                exit(0);
            }

            close(newsockfd);

            timeout -= difftime(t2, t1)*1000;
            if (timeout < 0.0000)
                timeout = 5000;
        }

        else if (fdset[0].revents != POLLIN){
            printf(" Error! revents = %d\n", fdset[0].revents);
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}