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

    /* Recall that we specified INADDR_ANY when we specified the server
	   address in the server. Since the client can run on a different
	   machine, we must specify the IP address of the server. 

	   In this program, we assume that the server is running on the
	   same machine as the client. 127.0.0.1 is a special address
	   for "localhost" (this machine)

    */
	   
	/* IF YOUR SERVER RUNS ON SOME OTHER MACHINE, YOU MUST CHANGE 
           THE IP ADDRESS SPECIFIED BELOW TO THE IP ADDRESS OF THE 
           MACHINE WHERE YOU ARE RUNNING THE SERVER. 
    */

   server_addr.sin_family = AF_INET;
   inet_aton("127.0.0.1", &server_addr.sin_addr);
   server_addr.sin_port = htons(20000);

   	/* With the information specified in serv_addr, the connect()
	   system call establishes a connection with the server process.
	*/

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("Unable to connect to the specified server: connect() system call failed !\n");
        exit(0);
    }

    /* After connection, the client can send or receive messages.
	   However, please note that recv() will block when the
	   server is not sending and vice versa. Similarly send() will
	   block when the server is not receiving and vice versa. For
	   non-blocking modes, refer to the online man pages.
	*/

    const char* exit_str = "-1\0";
    int bytes_sent = 0;

    while(1){

        printf("\nEnter the arithmetic expression to be sent to the server(end with a newline character)\n \tOR \nEnter -1 to terminate the client process.\n");
        
        unsigned int len_max = 10;
        unsigned int current_len = 0;

        char *user_input = malloc(len_max*sizeof(char));
        current_len = len_max;
        

        if (user_input != NULL){

            int ch = EOF;
            unsigned int i = 0;

            // accept user input until a newline character is encountered or EOF is encountered
            while ((ch = getchar()) != '\n' && ch != EOF){
                user_input[i++] = (char)ch;
                    
                // if the user input exceeds the allocated memory, reallocate memory
                if (i == current_len){
                    current_len = i + len_max;
                    //current_len = current_len + 1;
                    user_input = realloc(user_input, current_len*sizeof(char));
                }
            }
            user_input[i] = '\0';

            if(strcmp(user_input, exit_str) == 0){
                // printf("Client process terminated.\n");
                break;
            }

            printf("User input taken successfully: %s\n", user_input);
            printf("Size of user input : %ld bytes\n", strlen(user_input));

            if ((bytes_sent = send(sockfd, user_input, strlen(user_input) + 1, 0)) != strlen(user_input) + 1){
                perror("send() system call sent a different number of bytes than expected !\n");
                exit(0);
            }

            printf("Bytes sent : %d bytes\n", bytes_sent);
        }

    
    }

    close(sockfd);
    printf("Client process terminated.\n");

    return 0;
    
}


