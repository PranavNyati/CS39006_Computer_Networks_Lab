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

#define INT_MIN -2147483648

double eval_exprn(char *, int, char*);

			/* THE SERVER PROCESS */

int main(){

    int sockfd, newsockfd;    /* Socket descriptor */
    const char* exit_str = "-1\0";
    struct sockaddr_in server_addr, client_addr; /* Server and client address  are stored in these sockaddr_in structures */

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Cannot create the socket: socket() system call failed !\n");
        exit(0);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(20000);

    /* With the information provided in server_addr, we associate the server with its port using the bind() system call. */
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        perror("Unable to bind local address: bind() system call failed !\n");
        exit(0);
    }

   listen(sockfd, 5);


    int client_addr_len, flag1 = 0;
    int total_bytes_rcv = 0, bytes_recv = 0;

    unsigned int buff_len_max = 5, exprn_len_max = 10, i = 0, current_exprn_len = 0;
    current_exprn_len = exprn_len_max;

    char buffer[buff_len_max];
    char* client_exprn = (char*)malloc(exprn_len_max*sizeof(char));

    while(1){

        client_addr_len = sizeof(client_addr);	


        if ((newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_addr_len)) < 0){
            perror("Unable to accept the client's connection request: accept() system call failed !\n");
            exit(0);
        }

        printf("\nHi, I am the server process, waiting to receive the expression from the client !\n");
        
        while(1){

            total_bytes_rcv = 0;
            i = 0;

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

                for(int j = 0; j < bytes_recv; j++){
                
                    if(buffer[j] != '\0'){
                        client_exprn[i++] = buffer[j];
                    }
                    else if (buffer[j] == '\0'){
                        client_exprn[i++] = '\0';
                        break;
                    }

                    if ( i == current_exprn_len){
                        current_exprn_len = i + exprn_len_max;
                        client_exprn = realloc(client_exprn, current_exprn_len*sizeof(char));
                    }
                }
;
                if(client_exprn[i-1] == '\0'){
                    break;
                }

            }

            if (flag1 == 1){
                close(newsockfd);
                continue;
            }

            if (strcmp(client_exprn, exit_str) == 0){
                break;
            }

            printf("\nReceived expression from a client:\n");
            printf("%s\n", client_exprn);
            printf("\nSize of the expression received : %ld bytes\n", strlen(client_exprn));
            printf("Total no of bytes received from client : %d bytes\n", total_bytes_rcv);

            char *err_string = malloc(150*sizeof(char));

            double result = eval_exprn(client_exprn, strlen(client_exprn), err_string);	/* Evaluate the expression received from the client */
 
            char result_str[150];
            
            if(result == INT_MIN){
                strcpy(result_str, err_string);
            }
            else{
                sprintf(result_str, "%f", result);
            }

            if (send(newsockfd, result_str, strlen(result_str) + 1, 0) != strlen(result_str) + 1){
                perror("send() system call sent a different number of bytes than expected !\n");
                exit(0);
            }
            
            printf("\nResult of the expression sent to the client !\n");
        }
        
        close(newsockfd);   /* Close the socket descriptor used to  communicate with the client */
        printf("\nThe client has closed the connection !\n");
    }

    return 0;
}


double eval_exprn(char *client_exprn, int exprn_len, char* error_str){

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

        else if ((client_exprn[curr_sym] >= '0' && client_exprn[curr_sym] <= '9' ) || client_exprn[curr_sym] == '.'){
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
                if((temp == 0) || (temp < 0.0000000001 && temp > -0.0000000001)){
                    printf("Error: Division by zero! Expression could not be evaluated.\n");
                    strcpy(error_str, "Error => Division by zero! Expression could not be evaluated.\n");
                    return INT_MIN;
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
                strcpy(error_str, "Error => Invalid expression received as left bracket not matched with right bracket !\n");
                return INT_MIN;
            }

            double temp = eval_exprn(client_exprn + curr_sym - temp_exprn_len, temp_exprn_len, error_str);
            if (temp == INT_MIN)
                return INT_MIN;

            if (oprn_flag == -1)
                result = temp;

            else if (oprn_flag == 0)
                result += temp;   

            else if (oprn_flag == 1)
                result -= temp;
            
            else if (oprn_flag == 2)
                result *= temp;
            
            else if (oprn_flag == 3){
                if((temp == 0) || (temp < 0.0000000001 && temp > -0.0000000001)){
                    printf("Error: Division by zero! Expression could not be evaluated.\n");
                    strcpy(error_str, "Error => Division by zero! Expression could not be evaluated.\n");
                    return INT_MIN; 
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
            strcpy(error_str, "Error => Invalid expression received !\n");
            return INT_MIN;
        }
    }
    return result;
}
