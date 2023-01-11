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

#include <math.h>


//double eval_exprn(char *client_exprn, int exprn_len);
double eval_exprn(char *, int );

			/* THE SERVER PROCESS */

int main(){

    int sockfd, newsockfd;    /* Socket descriptor */

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

    int client_addr_len, flag1 = 0;
    int total_bytes_rcv = 0, bytes_recv = 0;

    unsigned int buff_len_max = 5, exprn_len_max = 10, i = 0, current_exprn_len = 0;
    current_exprn_len = exprn_len_max;

    //char* buffer = (char*)malloc(buff_len_max*sizeof(char));

    char buffer[buff_len_max];
    char* client_exprn = (char*)malloc(exprn_len_max*sizeof(char));

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

        total_bytes_rcv = 0;
        i = 0;

        printf("Hi, I am the server process, waiting to receive the expression from the client !\n");
        int itr = 0;
        while(1){
            
            if ( (bytes_recv = recv(newsockfd, buffer, buff_len_max, 0)) <= 0 ){	/* Receive the expression from the client */
                if (bytes_recv < 0){
                    perror("Unable to receive the expression from the client: recv() system call failed !\n");
                    exit(0);
                }
                else if (bytes_recv == 0){
                    printf("The client has closed the connection !\n");
                    flag1 = 1;
                    break;
                }
            }

            total_bytes_rcv += bytes_recv;
            printf("Received %d bytes from client in iteration itr = %d\n", bytes_recv, itr++);

            for(int j = 0; j < bytes_recv; j++){
                
            // printf("buffer[%d] = %c\n", j, buffer[j]);
            
                if(buffer[j] != '\0'){
                    client_exprn[i++] = buffer[j];
                }
                else if (buffer[j] == '\0'){
                    client_exprn[i++] = '\0';
                    break;
                }

                if ( i == current_exprn_len){
                    current_exprn_len = i + exprn_len_max;
                    //current_exprn_len = 1 + current_exprn_len;
                    client_exprn = realloc(client_exprn, current_exprn_len*sizeof(char));
                }
            }

            // printf("Debug 1\n");
            if(client_exprn[i-1] == '\0'){
                break;
            }
            // printf("Debug 2\n");

        }

        if (flag1 == 1){
            close(newsockfd);
            continue;
        }

        printf("Received expression from client:\n");
        printf("%s\n", client_exprn);
        printf("Size of the expression received : %ld bytes\n", strlen(client_exprn));
        printf("Total no of bytes received from client : %d bytes\n", total_bytes_rcv);

        double result = eval_exprn(client_exprn, strlen(client_exprn) );	/* Evaluate the expression received from the client */
        printf("Result of the expression received from the client = %lf\n", result);

        close(newsockfd);   /* Close the socket descriptor used to  communicate with the client */

    }

    //close(newsockfd);   /* Close the socket descriptor used to  communicate with the client */

    return 0;
}


double eval_exprn(char *client_exprn, int exprn_len){

    // function to evaluate the arithmetic expression of decimal numbers received from the client and compute its value
    int curr_sym = 0, expo_ten_decimal = -1;
    double result = 0.0;
    
    int oprn_flag = -1;  // set 0 for addn, 1 for subs, 2 for mult, 3 for div

    while(curr_sym < exprn_len){

        if (client_exprn[curr_sym] == ' '){
            curr_sym++;
        }

        else if (client_exprn[curr_sym] == '+'){
            curr_sym++;
            oprn_flag = 0;
        }

        else if (client_exprn[curr_sym] == '-'){
            curr_sym++;
            oprn_flag = 1;
        }

        else if (client_exprn[curr_sym] == '*'){
            curr_sym++;
            oprn_flag = 2;
        }

        else if (client_exprn[curr_sym] == '/'){
            curr_sym++;
            oprn_flag = 3;
        }

        else if (client_exprn[curr_sym] >= '0' && client_exprn[curr_sym] <= '9'){
            double temp = 0.0;
            while( (curr_sym < exprn_len) && (client_exprn[curr_sym] >= '0' && client_exprn[curr_sym] <= '9') ){
                temp = temp*10 + (client_exprn[curr_sym] - '0');
                curr_sym++;
            }

            if ((curr_sym < exprn_len) && (client_exprn[curr_sym] == '.')){
                curr_sym++;
                expo_ten_decimal = -1;
                while( (curr_sym < exprn_len) && (client_exprn[curr_sym] >= '0' && client_exprn[curr_sym] <= '9') ){
                    temp = temp + (client_exprn[curr_sym] - '0')*pow((double)10, (double)expo_ten_decimal);
                    curr_sym++;
                    expo_ten_decimal--;
                }
            }

            if (oprn_flag == -1)
                result = temp;

            else if (oprn_flag == 0)
                result += temp;   

            else if (oprn_flag == 1)
                result -= temp;
            
            else if (oprn_flag == 2)
                result *= temp;
            
            else if (oprn_flag == 3){
                if(temp == 0){
                    printf("Error: Division by zero! Expression could not be evaluated.\n");
                    exit(0);
                }
                else{
                    result /= temp;
                }
            }
        }

        else if (client_exprn[curr_sym] == '('){
            
            curr_sym++;
            int temp_exprn_len = 0;
            while(curr_sym < exprn_len && client_exprn[curr_sym] != ')' ){
                temp_exprn_len++;
                curr_sym++;
            }

            if (curr_sym == exprn_len){
                printf("Error: Invalid expression received as left bracket not matched with right bracket !\n");
                exit(0);
            }

            double temp = eval_exprn(client_exprn + curr_sym - temp_exprn_len, temp_exprn_len);

            if (oprn_flag == -1)
                result = temp;

            else if (oprn_flag == 0)
                result += temp;   

            else if (oprn_flag == 1)
                result -= temp;
            
            else if (oprn_flag == 2)
                result *= temp;
            
            else if (oprn_flag == 3){
                if(temp = 0 || temp < 0.0000000001 && temp > -0.0000000001){
                    printf("Error: Division by zero! Expression could not be evaluated.\n");
                    exit(0);
                }
                else{
                    result /= temp;
                }
            }
        }

        else if(client_exprn[curr_sym] == ')')
            curr_sym++;

        else{
            printf("Error: Invalid expression received !\n");
            exit(0);
        }
    }
    return result;
}
