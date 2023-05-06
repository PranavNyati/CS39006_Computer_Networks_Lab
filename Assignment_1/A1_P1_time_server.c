/*
    COMPUTER NETWORKS LAB(CS39006) - SEM 6
    ASSIGNMENT 1 - PROBLEM 1
    NAME : PRANAV NYATI
    ROLL : 20CS30037
*/

/*
			NETWORK PROGRAMMING WITH SOCKETS
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>

#include <time.h>

			/* THE SERVER PROCESS */

int main(){

    int sockfd, newsockfd;     /* Socket descriptor */

    struct sockaddr_in server_addr, client_addr; /* Server and client address  are stored in these sockaddr_in structures */
        
    /* The following system call opens a socket. The first parameter indicates the family of the protocol to be followed. 
    For internet protocols we use AF_INET. For TCP sockets the second parameter is SOCK_STREAM. The third parameter 
    is set to 0 for user applications.
	*/

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Cannot create the socket: socket() system call failed !\n");
        exit(0);
    }

    /* The structure "sockaddr_in" is defined in <netinet/in.h> for the internet family of protocols. This has three 
    main fields. The field "sin_family" specifies the family and is therefore AF_INET for the internet family. 
    The field "sin_addr" specifies the internet address of the server. This field is set to INADDR_ANY for machines 
    having a single IP address. The field "sin_port" specifies the port number of the server.
	*/

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(20000);

    /* With the information provided in server_addr, we associate the server with its port using the bind() system call. */
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        perror("Unable to bind local address: bind() system call failed !\n");
        exit(0);
    }

    /* The listen() system call allows the process to listen on the socket for connections.
    The first argument is the socket file descriptor to listen on. The second argument represents the size of the backlog 
    queue, i.e., the maximum number of concurrent client connection requests that will be queued up while the system is
	executing the "accept" system call below.
    */
   listen(sockfd, 5);

   	/* Here, we illustrate an iterative server - one which handles client connections one by one, i.e., no concurrency. 
       The accept() system call returns a new socket descriptor which is used for communication with the server. 
       After the communication is over, the process comes back to wait again on the original socket descriptor.
	*/

    char buffer[500];		/* We will use this buffer for communication */
    int client_addr_len;

    while(1){

        client_addr_len = sizeof(client_addr);	

        /* The accept() system call accepts a client connection. It blocks the server until a client request comes.

		   The accept() system call fills up the client's details in a struct sockaddr which is passed as a parameter.
		   The length of the structure is noted in clilen. Note that the new socket descriptor returned by the accept()
		   system call is stored in "newsockfd".
		*/

        if ((newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_addr_len)) < 0){
            perror("Unable to accept the client's connection request: accept() system call failed !\n");
            exit(0);
        }

        /* Message to be sent to the client */
        // time_t now =  time(0);
        // struct tm tstruct = *localtime(&now);
        // strftime(buffer, sizeof(buffer), "%x - %I:%M:%S %p", &tstruct);

        /* We initialize the buffer, copy the message to it, and send the message to the client. */

        strcpy(buffer, "Hello, this is the server. I am sending you the current time in my system.");
        if (send(newsockfd, buffer, strlen(buffer) + 1, 0) != strlen(buffer) + 1){
            perror("send() system call sent a different number of bytes than expected !\n");
            exit(0);
        }

        time_t now =  time(0);
        struct tm tstruct = *localtime(&now);
        strftime(buffer, sizeof(buffer), "%x - %I:%M:%S %p", &tstruct);
        
        if (send(newsockfd, buffer, strlen(buffer) + 1, 0) != strlen(buffer) + 1){
            perror("send() system call sent a different number of bytes than expected !\n");
            exit(0);
        }

        /* We now receive a message from the client. For this example we make an assumption that the entire message 
           sent from the client will come together. In general, this need not be true for TCP sockets (unlike UDPi sockets), 
           and this program may not always work (for this example, the chance is very low as the message is very short. 
           But in general, there has to be some mechanism for the receiving side to know when the entire message
		  is received. Look up the return value of recv() to see how you can do this.
		*/

        if (recv(newsockfd, buffer, 500, 0) < 0){
            perror("Unable to receive message from the client: recv() system call failed !\n");
            exit(0);
        }

        printf("Message from the client:\n");
        printf("%s\n", buffer);

        close(newsockfd);

    }
    return 0;
}