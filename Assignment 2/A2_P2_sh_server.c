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

#include <dirent.h>
#define MAX_BUFF_SIZE 50

// function to check if a username is present in the file containing usernames
int check_username(char *name, char *filename){

    FILE *file1;
    char username[100];
    
    if( (file1 = fopen(filename, "r")) == NULL){
        printf("Error: Unable to open file containing usernames.\n");
        return -1;
    }

    while(fgets(username, 100, file1) != NULL){
        username[strlen(username) - 1] = '\0';
        if (strcmp(username, name) == 0)
            return 1;
    }

    fclose(file1);
    return 0;
}

// function to execute shell command and return the result
int execute_cmd(char *cmd, char *result, int result_len){

    char *path = (char *) malloc(200 * sizeof(char));
    char *args[2] = {NULL, NULL};
    int num = 0;
    args[0] = strtok(cmd, " ");

    if (args[0] == NULL){ // invalid or empty command
        strcpy(result, "$$$$\nInvalid command: Empty command !");
        return -1;
    }

    while (args[num] != NULL){ // checking if the command has more than 1 argument, if yes then invalid command

        if (num >= 2){
            strcpy(result, "####\nError in running the command: Too many arguments !");
            return 0;
        }
        num++;
        args[num] = strtok(NULL, " ");
    }

    // checking the type of command and taking respective action
    
    // if the cmd is 'pwd'
    if (strcmp(args[0], "pwd") == 0){

        if (args[1] != NULL){
            strcpy(result, "####\nError in running the command 'pwd': command 'pwd' does not need any argument !");
            return 0;
        }
        getcwd(result, result_len);
        return 1;
    }

    // if the command is cd
    else if (strcmp(args[0], "cd") == 0){

        int flag;
        {
            if (args[1] == NULL)
                flag = chdir(getenv("HOME"));
            
            else
                flag = chdir(args[1]);
            
            if (flag == 0){
                getcwd(result, result_len);
                return 1;
            }
            else if (flag == -1){
                strcpy(result, "####\nError in running the command 'cd' !");
                return 0;
            }
        }
    }

    // if the command is 'dir'
    else if (strcmp(args[0], "dir") == 0 ){
        
        DIR *dir;
        if (args[1] == NULL)  // if no argument after dir, then list contents of cwd
            dir = opendir(".");
        else // else list contents of the directory specified by the argument
            dir = opendir(args[1]);

        if (dir == NULL){  // if the directory does not exist, then return error
            strcpy(result, "####\nError in running the command 'dir'!");
            return 0;
        }
        // if the directory exists
        else {
            struct dirent *entry;
            int i = 0;
            while ( (entry = readdir(dir)) != NULL){
                if (i == 0){
                    strcpy(result, entry->d_name);
                    i++;
                }
                else{
                    strcat(result, "\n");
                    strcat(result, entry->d_name);
                }
            }
            // strcat(result, "\n");
            closedir(dir);
            return 1;
        }
    }

    else{  // if the command is not any of the above, then return error
        strcpy(result, "$$$$\nInvalid command !");
        return -1;
    }
}

			/* THE SERVER PROCESS */

int main(){

    int sockfd, newsockfd;     /* Socket descriptors */
    struct sockaddr_in server_addr, client_addr;   /* Server and client address  are stored in these sockaddr_in structures */
    char user_file[] = "users.txt";

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Cannot create the socket: socket() system call failed !\n");
        exit(0);  
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(20000);

    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        perror("Unable to bind local address: bind() system call failed !\n");
        exit(0);
    }
    printf("Server is running on port 20000.\n");

    listen(sockfd, 5);

    printf("\nWaiting for clients to connect to the server ...\n");

    int client_addr_len;
    char buffer[MAX_BUFF_SIZE];

    while(1){

        client_addr_len = sizeof(client_addr);

        if ((newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_addr_len)) < 0){
            perror("Unable to accept the client's connection request: accept() system call failed !\n");
            exit(0);
        }

        printf("\nAccepted a new client's connection.\n");


        if(fork() == 0){  // value of fork = 0 stands for the child process

        	/* This child process will now communicate with the
		    client through the send() and recv() system calls.
			*/
            close(sockfd);

            // ############ SEND LOGIN PROMPT TO CLIENT ############
            
            strcpy(buffer, "LOGIN:");
            if ( send(newsockfd, buffer, strlen(buffer) + 1, 0) != strlen(buffer) + 1){
                perror("send() system call sent a different number of bytes than expected !\n");
                exit(0);
            }

            // reset the receive buffer
            memset(buffer, 0, MAX_BUFF_SIZE);

            int bytes_recv = 0, total_bytes_recv = 0, i = 0, curr_serv_buff_len = MAX_BUFF_SIZE;
            char *serv_buff = (char *) malloc(MAX_BUFF_SIZE * sizeof(char));
            memset(serv_buff, 0, MAX_BUFF_SIZE);

            // ############ RECEIVE USERNAME IN CHUNKS ############  => DONE

            while(1){
                bytes_recv = recv(newsockfd, buffer, MAX_BUFF_SIZE, 0);

                if (bytes_recv < 0){
                    perror("Unable to receive data from the client: recv() system call failed !\n");
                    exit(0);
                }

                if (bytes_recv == 0){
                    printf("A client has closed the connection.\n");
                    close(newsockfd);
                    exit(0);
                }

                total_bytes_recv += bytes_recv;

                for (int j = 0; j < bytes_recv; j++){
                    serv_buff[i++] = buffer[j];
                    if (buffer[j] == '\0')
                        break;

                    if (i == curr_serv_buff_len){
                        curr_serv_buff_len += MAX_BUFF_SIZE;
                        serv_buff = (char *) realloc(serv_buff, curr_serv_buff_len * sizeof(char));
                    }  
                }

                if (serv_buff[i-1] == '\0')
                    break;
            }

            printf("\nUsername received: %s\n", serv_buff);
            // printf("length of username = %ld\n", strlen(serv_buff));
            
            // reset the receive buffer
            memset(buffer, 0, MAX_BUFF_SIZE);

            int res = check_username(serv_buff, user_file);  // function call to check username validity

            if (res == -1)
                exit(1); // exit the child process if file containing usernames cannot be opened
            
            else if (res == 0)  // if username is not found, send 'NOT-FOUND' to the client side
                strcpy(buffer, "NOT-FOUND");
            
            else if (res == 1)  // if username is found, send 'FOUND' to the client side
                strcpy(buffer, "FOUND");

            if ( send(newsockfd, buffer, strlen(buffer) + 1, 0) != strlen(buffer) + 1){
                perror("send() system call sent a different number of bytes than expected !\n");
                exit(0);
            }

            printf("\nUser login status sent to the client.\n");

            // reset the receive and server buffers
            memset(buffer, 0, MAX_BUFF_SIZE);
            memset(serv_buff, 0, curr_serv_buff_len);

            int curr_cmd_len;
            char *client_cmd;
            const char* exit_str = "exit\0";
            total_bytes_recv = 0, i = 0; 

            // while loop to receive shell commands from the client until the client sends 'exit'
            while(1){
                
                client_cmd = (char *)malloc(MAX_BUFF_SIZE*sizeof(char));
                memset(client_cmd, 0, MAX_BUFF_SIZE);
                total_bytes_recv = 0;
                i = 0;
                curr_cmd_len = MAX_BUFF_SIZE;

                // ########## RECEIVE SHELL COMMAND IN CHUNKS ##########  => DONE

                // receiving a shell command from the client
                while(1){
                    bytes_recv = recv(newsockfd, buffer, MAX_BUFF_SIZE, 0);

                    if (bytes_recv < 0){
                        perror("Unable to receive data from the client: recv() system call failed !\n");
                        exit(0);
                    }

                    if (bytes_recv == 0){
                        printf("A client has closed the connection.\n");
                        close(newsockfd);
                        exit(0);
                    }

                    total_bytes_recv += bytes_recv;

                    for (int j = 0; j < bytes_recv; j++){                   
                        client_cmd[i++] = buffer[j];
                        if (buffer[j] == '\0')
                            break;   

                        if (i == curr_cmd_len){
                            curr_cmd_len += MAX_BUFF_SIZE;
                            client_cmd = realloc(client_cmd, curr_cmd_len*sizeof(char));
                        }  
                    }

                    if (client_cmd[i-1] == '\0')
                        break;
                }

                if (strcmp(client_cmd, exit_str) == 0){
                    printf("A client has closed the connection.\n");
                    close(newsockfd);
                    exit(0);
                }

                printf("\nCommand received from a client:\n");
                printf("%s\n", client_cmd);
                // printf("\nLength of the shell command received : %ld bytes\n", strlen(client_cmd));

                // reset the receive buffer
                memset(buffer, 0, MAX_BUFF_SIZE);

                int result_len = 10000;
                char *result = (char *)malloc(result_len*sizeof(char));
                
                int res = execute_cmd(client_cmd, result, result_len); // function call to execute the shell command

                //  ############### SEND TO BE DONE IN CHUNKS ##################### => DONE

                int bytes_sent = 0, total_bytes_sent = 0, end_flag = 0;
                int curr_result_len = 0;
                i = 0;

                while(1){

                    for (int j = 0; j < MAX_BUFF_SIZE ; j++){
                        buffer[j] = result[i++];
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
                
                printf("\nResult of the execution of the command sent to the client.\n");
                                
            }

            close(newsockfd); 
            printf("Connection with a client has been closed.\n");
            exit(0);
        }
       
        close(newsockfd);  // parent process closes the new socket descriptor as parent process is not going to communicate with the client
    }
    return 0;
}