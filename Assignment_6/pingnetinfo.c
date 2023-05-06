// NETWORKS LAB ASSIGNMENT-6
// PRANAV NYATI (20CS30037)
// SHREYAS JENA (20CS30049)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <poll.h>
#include <errno.h>


#define BUF_SIZE 2000

int MAX_TTL = 30;

int ttl = 1;
int num_probes = 3;
int time_gap = 3;
int data_size = 1000;
double MAX_TIME = 9999999.00;

int microsleep(long microsec)
{
    struct timespec ts;
    int res;

    if (microsec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = microsec / 1000000;
    ts.tv_nsec = (microsec % 1000000) * 1000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

// convert hostname to IP address
char *convert_hostname_to_ip(char *host)
{
    struct hostent *hent;
    struct in_addr **addr_list;
    int i;

    if ((hent = gethostbyname(host)) == NULL)
    {
        herror("gethostbyname");
        return NULL;
    }

    addr_list = (struct in_addr**)hent->h_addr_list;

    for (i = 0; addr_list[i] != NULL; i++)
        return inet_ntoa(*addr_list[i]);

    return NULL;
}

// function to compute checksum
uint16_t checksum(uint16_t *addr, int len) 
{
    int nleft = len;
    uint32_t sum = 0;
    uint16_t *w = addr;
    uint16_t answer = 0;

    // Calculate the sum of each 16-bit word
    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }

    // Add in the carry from the last 16-bit word, if any
    if (nleft == 1) {
        *(uint8_t*)(&answer) = *(uint8_t*)w;
        sum += answer;
    }

    // Add in the carries from the sum
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    // Take the one's complement of the sum
    answer = ~sum;

    return answer;
}

void print_packet_info(char *buf, FILE *fp, int bytes_recv, struct sockaddr_in addr){


    fprintf(fp, "============================================================\n");
    fprintf(fp, "PACKET RECEIVED FROM: %s\n", inet_ntoa(addr.sin_addr));
    fprintf(fp, "BYTES RECEIVED: %d bytes\n", bytes_recv);
    fprintf(fp, "------------------------------------------------------------\n");
    fprintf(fp, "IP HEADER:\n");
    fprintf(fp, "------------------------------------------------------------\n");
    // print the IP header in the FILE
    struct ip *ip_header = (struct ip*)buf;
    fprintf(fp, "SRC IP: %s\n", inet_ntoa(*(struct in_addr *)&ip_header->ip_src.s_addr));
    fprintf(fp, "DST IP: %s\n", inet_ntoa(*(struct in_addr *)&ip_header->ip_dst.s_addr));
    fprintf(fp, "TTL: %d\n", ip_header->ip_ttl);
    fprintf(fp, "PROTOCOL: %d\n", ip_header->ip_p);
    fprintf(fp, "HEADER LENGTH: %d bytes\n", ip_header->ip_hl*4);
    fprintf(fp, "TOTAL LENGTH: %d bytes\n", ntohs(ip_header->ip_len));

    fprintf(fp, "------------------------------------------------------------\n");
    fprintf(fp, "ICMP HEADER:\n");
    fprintf(fp, "------------------------------------------------------------\n");
    struct icmphdr *icmp_header = (struct icmphdr*)(buf + ip_header->ip_hl*4);
    fprintf(fp, "TYPE: %d\n", icmp_header->type);
    fprintf(fp, "CODE: %d\n", icmp_header->code);
    fprintf(fp, "CHECKSUM: %d\n\n", icmp_header->checksum);
    fprintf(fp, "============================================================\n");

    fflush(fp);
}

int find_next_node(int sockfd, struct sockaddr_in *dest_addr, char *node_ip, int* timeout_flag, FILE *fp)
{
    srand(time(NULL));

    int n = 5;

    char first_ip[50];
    memset(first_ip, 0, 50);

    printf("------------------------------------------------------------\n");
    printf("DISCOVERING NODE NO %d IN THE PATH ...\n\n", ttl);
    
    struct pollfd fdset[1];
    int nfds = 1;
    int timeout = 2 * 1000;

    // specifying the info of the two clients using fdset
    memset(fdset, 0, sizeof(fdset));
    fdset[0].fd = sockfd;
    fdset[0].events = POLLIN;

    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
    
    while (n--)
    {

        struct ip *iph;
        struct icmphdr *icmph;
        char buf[BUF_SIZE];

        // Create a zero-data packet (only header, for checking latency)
        icmph = (struct icmphdr*)buf;

        icmph->type = ICMP_ECHO;
        icmph->code = 0;
        icmph->checksum = 0;
        icmph->checksum = checksum((uint16_t*)icmph, sizeof(struct icmphdr));

        // Receive the packet
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);

        struct timespec tstart={0,0}, tend={0,0};        
        double time_diff = 0.0;

        if (sendto(sockfd, buf, sizeof(struct icmphdr), 0, (struct sockaddr*)dest_addr, sizeof(struct sockaddr)) == -1)
        {
            perror("Error while sending packet!");
            exit(EXIT_FAILURE);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
        }
        // Clear the buffer
        memset(buf, 0, BUF_SIZE);

        clock_gettime(CLOCK_MONOTONIC, &tstart);

        // poll with timeout on the socket for receiving the packet
        int ret = poll(fdset, nfds, timeout);

        if (ret < 0)
        {
            printf("Error while polling!\n");
            exit(EXIT_FAILURE);
        }

        if (ret == 0)
        {
            if (n > 0){
                printf("P %d: \t*\t", 4-n);
                fflush(stdout);
                continue;
            }

            else{
                printf("P %d: \t*\t", 4-n);
                fflush(stdout);
                printf("\n\nLATENCY and BANDWIDTH can't be estimated for the incoming and outgoing link from this node!\n");
                printf("Continuing with the next hop ...");
                printf("\n\nTTL : %d, Node IP : COULD NOT FOUND\n\n", ttl);

                memset(node_ip, 0, 50);
                *timeout_flag = 1;
                return 0;
            }
        }

        else if (fdset[0].revents & POLLIN){
            
            int bytes_recv = 0;

            if ( (bytes_recv = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr*)&addr, &addr_len)) == -1){
                perror("Error while receiving packet!");
                exit(EXIT_FAILURE);
            }
            
            // function call to print the IP header 
            print_packet_info(buf, fp, bytes_recv, addr);

            clock_gettime(CLOCK_MONOTONIC, &tend);

            // Find time difference in microseconds 
            time_diff = ((double)(tend.tv_sec*1.0e6 + 1.0e-3*tend.tv_nsec)) - ((double)tstart.tv_sec*1.0e6 + 1.0e-3*tstart.tv_nsec);
            printf("P %d: %0.2f ms\t", 4-n, time_diff/1000.0);
            fflush(stdout);
            
        }
        
        // Get the IP header from the packet
        struct ip *ip_header = (struct ip*)buf;

        // memset(node_ip, 0, 50);
        memset(node_ip, 0, 50);
        strcpy(node_ip, inet_ntoa(*(struct in_addr *)&ip_header->ip_src.s_addr));

        if (n == 4){
            strcpy(first_ip, node_ip);
            // printf("IP of the node at %d hops for the 1st ICMP packet: %s\n", ttl, first_ip);
        }

        else {
            if (strcmp(first_ip, node_ip) != 0){
                // printf("IP of the node at %d hops for the first %d ICMP packets: %s\n", ttl, 5-n, first_ip);
                // printf("IP of the node at %d hops for the %d ICMP packet: %s\n", ttl, 6-n, node_ip);
                // printf("The node at %d hops from src has changed between the first %d ICMP packets and the %d ICMP packet\n", ttl, 5-n, 6-n);
                // printf("Hence, Finalising the node with IP %s as the node at %d hops from src for Latency and Bandwidth calculations.\n", first_ip, ttl);
                // printf("Not sending the remaining %d ICMP packets with TTL = %d, as the Route has changed!!\n", n-1, ttl);

                strcpy(node_ip, first_ip);   // Copy the first IP to node_ip to be used for latency and bandwidth calculations

                break;
            }

        }   
        // sleep for remaining time if difftime < 1 second, else if difftime > 1 second, don't sleep
        if (time_diff < 2*1.0e6 + 5){  // packet was received within 1 second
            microsleep(2*1.0e6 -time_diff);  // sleep for remaining time in microseconds (1 second = 1000000 microseconds            
        }
    
    }   

    // printf("Destination IP before leaving latency fn : %s\n\n\n", dest);
    printf("\n\nTTL : %d, Node IP : %s\n\n", ttl, node_ip);
    
    return 0;
}


int compute_node_latency_bandwidth(char *node_ip, int sockfd, double *curr_min_RTT, double *curr_min_RTT_data, int *lat_timeout_flag, int *band_timeout_flag, FILE *fp){

    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &MAX_TTL, sizeof(MAX_TTL));
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(node_ip);

    struct pollfd fdset[1];
    int nfds = 1;
    int timeout = time_gap * 1000;  // in milliseconds


    // ######### SENDING ICMP packets without DATA in-order to estimate LATENCY #######

    // specifying the info of the two clients using fdset
    memset(fdset, 0, sizeof(fdset));
    fdset[0].fd = sockfd;
    fdset[0].events = POLLIN;

    *curr_min_RTT = 0.0;
    double curr_RTT = 0;
    int flag1 = 0;
    double min = MAX_TIME;

    printf("Sending %d ICMP packets without DATA ...\n", num_probes);
    printf("Packet size: %ld bytes\n\n", sizeof(struct ip) + sizeof(struct icmphdr));


    for (int i = 0; i < num_probes; i++){

        struct ip *iph;
        struct icmphdr *icmph;
        char buf[BUF_SIZE];

        // Create a zero-data packet (only header, for checking latency)
        icmph = (struct icmphdr*)buf;
        icmph->type = ICMP_ECHO;
        icmph->code = 0;
        icmph->checksum = 0;
        icmph->checksum = checksum((uint16_t*)icmph, sizeof(struct icmphdr));

        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);

        struct timespec tstart={0,0}, tend={0,0};       
        double time_diff = 0.0;
        // printf("Packet size: %ld bytes\n", sizeof(struct ip) + sizeof(struct icmphdr));

        if (sendto(sockfd, buf, sizeof(struct icmphdr), 0, (struct sockaddr*)&dest_addr, sizeof(struct sockaddr)) == -1)
        {
            perror("Error while sending packet!");
            exit(EXIT_FAILURE);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
        }
        memset(buf, 0, BUF_SIZE);

        clock_gettime(CLOCK_MONOTONIC, &tstart); 

        // poll with timeout on the socket for receiving the packet
        int ret = poll(fdset, nfds, timeout);

        if (ret < 0)
        {
            printf("Error while polling!\n");
            exit(EXIT_FAILURE);
        }

        if (ret == 0)
        {
            if (i < num_probes - 1){
                printf("Timeout occured before receiving the packet no %d from node no %d\n", i, ttl);
                printf("Sending the next packet ...\n\n");
                continue;
            }

            else if (i == num_probes - 1){
                printf("Timeout occured before receiving the packet no %d from node no %d\n", i, ttl);

                if (min == MAX_TIME){
                    printf("RTT of the node with IP %s cannot be estimated, and hence LATENCY and BANDWIDTH for the previous and the next link in the route CANNOT BE ESTIMATED!\n", node_ip);
                    *lat_timeout_flag = 1;
                    memset(node_ip, 0, 50);  
                    flag1 = 1;
                    break;
                }
            
            }
        }

        else if (fdset[0].revents & POLLIN){
            
            int bytes_recv = 0;

            if ((bytes_recv = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr*)&addr, &addr_len)) == -1){
                perror("Error while receiving packet!");
                exit(EXIT_FAILURE);
            }
            
            print_packet_info(buf, fp, bytes_recv, addr);

            clock_gettime(CLOCK_MONOTONIC, &tend);

            // Find time difference in microseconds
            time_diff = ((double)(tend.tv_sec*1.0e6 + 1.0e-3*tend.tv_nsec)) - ((double)tstart.tv_sec*1.0e6 + 1.0e-3*tstart.tv_nsec);

            curr_RTT = time_diff;

            if (bytes_recv == sizeof(struct ip) + sizeof(struct icmphdr)){
                printf("Bytes received in packet no %d: %d (as expected)\t\tRTT: %0.2f ms\n", i, bytes_recv, curr_RTT/1000.0);
                // printf("RTT of the intermediate node %s for packet no %d : %0.2f ms\n", node_ip, i, curr_RTT/1000.0);
            
                if (curr_RTT < min + 5){
                    min = curr_RTT;
                }
            }

            else{
                printf("Bytes received in packet no %d: %d (not as expected)\n", i, bytes_recv);
                printf("Will not consider this packet for RTT estimation.\n");
            }
        }

        if (time_diff < time_gap*1.0e6- 5){
            // printf("Sleeping for %0.2f seconds ...\n\n", time_gap - time_diff);
            
            microsleep(time_gap*1.0e6 - time_diff);
        }

    }

    if (flag1 == 1){
        *curr_min_RTT = MAX_TIME;
        return 0;
    }

    *curr_min_RTT = min;
    printf("\nMINIMUM RTT of the node %s for NON-DATA packet: %0.2f ms\n", node_ip, min/1000.0);
    
    // ######### SENDING ICMP PACKETS WITH DATA TO ESTIMATE BANDWIDTH

    memset(fdset, 0, sizeof(fdset));
    fdset[0].fd = sockfd;
    fdset[0].events = POLLIN;
      
    *curr_min_RTT_data = 0.0;
    double curr_RTT_data = 0;
    int flag2 = 0;
    double min_with_data = MAX_TIME;

    printf("\nSending %d ICMP packets with DATA ...\n", num_probes);
    printf("Packet size: %ld bytes\n\n", sizeof(struct ip) + sizeof(struct icmphdr) + data_size);


    for (int i = 0; i < num_probes; i++){

        struct ip *iph;
        struct icmphdr *icmph;
        char buf[BUF_SIZE];

        icmph = (struct icmphdr*)buf;
        icmph->type = ICMP_ECHO;
        icmph->code = 0;
        icmph->checksum = 0;

        // add data of size 500 bytes to the packet
        for (int j = 0; j < 500; j++){
            buf[sizeof(struct icmphdr) + j] = 'a';
        }
        icmph->checksum = checksum((uint16_t*)buf, sizeof(struct icmphdr) + data_size);


        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);

        struct timespec tstart={0,0}, tend={0,0};       
        double time_diff = 0.0;

        // printf("Packet size: %ld bytes\n", sizeof(struct ip) + sizeof(struct icmphdr) + data_size);

        if (sendto(sockfd, buf, sizeof(struct icmphdr) + data_size, 0, (struct sockaddr*)&dest_addr, sizeof(struct sockaddr)) == -1)
        {
            perror("Error while sending packet!");
            exit(EXIT_FAILURE);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
        }
        memset(buf, 0, BUF_SIZE);
        // printf("Data Packet no %d sent to node no %d\n", i, ttl);

        // start timer
        clock_gettime(CLOCK_MONOTONIC, &tstart); 

        // poll with timeout on the socket for receiving the packet
        int ret = poll(fdset, nfds, timeout);

        if (ret < 0)
        {
            printf("Error while polling!\n");
            exit(EXIT_FAILURE);
        }

        if (ret == 0){
            if (i < num_probes - 1){
                printf("Timeout occured before receiving the DATA packet no %d from node no %d\n", i, ttl);
                printf("Sending the next DATA packet ...\n\n");
                continue;
            }

            else if (i == num_probes - 1){
                printf("Timeout occured before receiving the DATA packet no %d from node no %d\n", i, ttl);

                if (min_with_data == MAX_TIME){
                    printf("RTT of the node with IP %s cannot be estimated, and hence BANDWIDTH for the previous and the next link in the route CANNOT BE ESTIMATED!\n", node_ip);
                    *band_timeout_flag = 1;
                    memset(node_ip, 0, 50);  
                    flag2 = 1;
                    break;
                }
            
            }
        }

        else if (fdset[0].revents & POLLIN){

            int bytes_recv = 0;

            if ((bytes_recv = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr*)&addr, &addr_len)) == -1){
                perror("Error while receiving packet!");
                exit(EXIT_FAILURE);
            }

            print_packet_info(buf, fp, bytes_recv, addr);


            clock_gettime(CLOCK_MONOTONIC, &tend);

            // Find time difference in microseconds
            time_diff = ((double)(tend.tv_sec*1.0e6 + 1.0e-3*tend.tv_nsec)) - ((double)tstart.tv_sec*1.0e6 + 1.0e-3*tstart.tv_nsec);
            curr_RTT_data = time_diff;

            if (bytes_recv == sizeof(struct ip) + sizeof(struct icmphdr) + data_size){
                printf("Bytes received in data packet no %d: %d (as expected)\t\tRTT: %0.2f ms\n", i, bytes_recv, curr_RTT_data/1000.0);
                // printf("No of bytes received in data packet no %d: %d is as expected.\n", i, bytes_recv);
                // printf("RTT of the intermediate node %s for data packet no %d : %0.2f ms\n", node_ip, i, curr_RTT_data/1000.0);
            
                if (curr_RTT_data < min_with_data + 5){
                    min_with_data = curr_RTT_data;
                }
            }

            else{
                printf("Bytes received in packet no %d: %d (not as expected)\n", i, bytes_recv);
                printf("Will not consider this packet for RTT estimation.\n");
            }
        }

        if (time_diff < time_gap*1.0e6- 5){
            
            microsleep(time_gap*1.0e6 - time_diff);
        }
    }

    if (flag2 == 1){
        *curr_min_RTT_data = MAX_TIME;
        return 0;
    }

    *curr_min_RTT_data = min_with_data;
    printf("\nMINIMUM RTT of the node %s for DATA packet: %0.2f ms\n", node_ip, min_with_data/1000.0);
    
    return 0;
}


int traceroute(int sockfd, char *src_ip, char *dest_ip)
{
    char node_ip[50], prev_node_ip[50], src[50], dest[50];
    // printf("Source IP in traceroute : %s\n", src_ip);
    // printf("Destination IP in traceroute : %s\n", dest_ip);
    strcpy(src, src_ip);
    strcpy(dest, dest_ip);
    strcpy(prev_node_ip, src_ip);

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(dest);

    double prev_node_min_RTT = 0.0;
    double curr_node_min_RTT = 0.0;
    double prev_node_min_RTT_with_data = 0.0;
    double curr_node_min_RTT_with_data = 0.0;

    FILE *fp = fopen("traceroute_packets.txt", "w");
    if (fp == NULL){
        printf("Error while opening the file!\n");
        exit(EXIT_FAILURE);
    }

    while (1){

        strcpy(dest_ip, dest);
        int timeout_flag = 0, lat_timeout_flag = 0, band_timeout_flag = 0;
        int ret = find_next_node(sockfd, &dest_addr, node_ip, &timeout_flag, fp);

        if (timeout_flag == 1){
            ttl++;

            if (strcmp(node_ip, dest) == 0){
                printf("Destination reached!\n");
                return 0;
            }
            continue;
        }

        // calculate latency
        ret = compute_node_latency_bandwidth(node_ip, sockfd, &curr_node_min_RTT, &curr_node_min_RTT_with_data, &lat_timeout_flag, &band_timeout_flag, fp);
        
        if (curr_node_min_RTT == MAX_TIME){
            printf("\nMINIMUM LATENCY of node b/w %s and %s: CANNOT BE ESTIMATED!\n", prev_node_ip, node_ip);
            printf("MINIMUM BANDWIDTH of link b/w %s and %s: CANNOT BE ESTIMATED!\n\n", prev_node_ip, node_ip);
            prev_node_min_RTT = curr_node_min_RTT;
        }
        
        else if (prev_node_min_RTT == MAX_TIME){
            printf("\nMINIMUM LATENCY of node b/w %s and %s: CANNOT BE ESTIMATED!\n", prev_node_ip, node_ip);
            printf("MINIMUM BANDWIDTH of link b/w %s and %s: CANNOT BE ESTIMATED!\n\n", prev_node_ip, node_ip);
            prev_node_min_RTT = curr_node_min_RTT;
        }

        // in case of no timeout
        else {

            double final_latency = (curr_node_min_RTT - prev_node_min_RTT)/2.0;
            printf("\nMIMIMUN LATENCY of link between node %s and %s: %0.2f ms\n", prev_node_ip, node_ip, final_latency/1000.0);
            prev_node_min_RTT = curr_node_min_RTT;


            if (curr_node_min_RTT_with_data == MAX_TIME){
                printf("\nMINIMUM BANDWIDTH of link b/w %s and %s: CANNOT BE ESTIMATED!\n\n", prev_node_ip, node_ip);
                prev_node_min_RTT_with_data = curr_node_min_RTT_with_data;
            }

            else if (prev_node_min_RTT_with_data == MAX_TIME){
                printf("\nMINIMUM BANDWIDTH of link b/w %s and %s: CANNOT BE ESTIMATED!\n\n", prev_node_ip, node_ip);
                prev_node_min_RTT_with_data = curr_node_min_RTT_with_data;
            }

            else {
                
                double final_latency_with_data = (curr_node_min_RTT_with_data - prev_node_min_RTT_with_data)/2.0;
                double final_bw = (data_size * 8.0)/((final_latency_with_data - final_latency));
                printf("\nMINIMUM BANDWIDTH of link b/w %s and %s: %0.2f Mbps\n\n", prev_node_ip, node_ip, final_bw);
                prev_node_min_RTT_with_data = curr_node_min_RTT_with_data;
            }
        }

        strcpy(prev_node_ip, node_ip);
        ttl++;

        if (strcmp(node_ip, dest) == 0)
        {
            printf("Destination reached!\n");
            fclose(fp);
            return 0;
        }

        // break if MAX_TTL is reached
        if (ttl > MAX_TTL)
        {
            printf("Destination not reached within MAX NO OF HOPS = %d hops\n", MAX_TTL);
            printf("Terminting Traceroute ...\n");
            fclose(fp);
            return 0;
        }

    }
    fclose(fp);
    return -1;
}

int main(int argc, char *argv[])
{
    // Input arguments
    char *site = argv[1];
    num_probes = atoi(argv[2]);
    time_gap = atoi(argv[3]);

    int sockfd;
    char hostbuffer[256];
    int hostname = gethostname(hostbuffer, sizeof(hostbuffer));

    if (hostname == -1)
    {
        perror("Error while getting hostname!");
        exit(EXIT_FAILURE);
    }

    printf("Local host name : %s\n", hostbuffer);

    char src_ip[50];
    strcpy(src_ip, convert_hostname_to_ip(hostbuffer));
    printf("Source IP : %s\n", src_ip);

    // Convert sitename to IP address if it is given
    char dest_ip[50];

    if (inet_aton(site, NULL) == 0)
        strcpy(dest_ip, convert_hostname_to_ip(site));
    else
        strcpy(dest_ip, site);

    printf("Destination IP : %s\n\n", dest_ip);

    // Create a raw socket
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
    {
        perror("Error while creating raw socket!");
        exit(EXIT_FAILURE);
    }


    if (traceroute(sockfd, src_ip, dest_ip) == -1)
    {
        perror("Error while tracing route!");
        exit(EXIT_FAILURE);
    }

    close(sockfd);
    return 0;

}