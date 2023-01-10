/*
    COMPUTER NETWORKS LAB(CS39006) - SEM 6
    ASSIGNMENT 1 - PROBLEM 1
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

    int sockfd;   /* Socket descriptor */
    
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

    char buffer[500];

    for (int i = 0; i < 500; i++) buffer[i] = '\0';    // initialising the buffer

    /* Receive the data sent by thr server to this client using the recv() system call */
    recv(sockfd, buffer, 500, 0);
    printf("Message from the server:\n\n");
	printf("%s\n", buffer);

    recv(sockfd, buffer, 500, 0);
    printf("%s\n", buffer);

	/* Message by the client to the server */
	strcpy(buffer,"I have received your message (the server date and time).\n");
	send(sockfd, buffer, strlen(buffer) + 1, 0);
		
	close(sockfd);

    return 0;
}