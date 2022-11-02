#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
	
#define MAXLINE 1024	// Char size
#define INTERVAL 1 		// Seconds

/* Global variables */
int sockfd;						// Socket file descriptor
char buffer[MAXLINE];			// String buffer
struct sockaddr_in servaddr;	// Socket server address
struct timeval tv;				// time out data

int main(int argc, char *argv[]) {	

	if (argc != 4) {
		printf("Usage: client [IP SERVER ADRESS] [PORT] [NUMBER OF PACKETS TO RECEIVE]\n");
		exit(EXIT_FAILURE);
	}

	uint16_t server_port	= htons(atoi(argv[2])); // Server port
	in_addr_t server_ip 	= inet_addr(argv[1]);	// Address
	int total_packets		= atoi(argv[3]);		// Packets to receive
	
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		printf("socket file descriptor error: %d %s\n", sockfd, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	memset(&servaddr, 0, sizeof(servaddr));	// Clear data of socket server address 
		
	// Define server data
	servaddr.sin_family = AF_INET;			// IPv4
	servaddr.sin_addr.s_addr = server_ip;	// Assign server IP
	servaddr.sin_port = server_port;		// Assign server port

	// Assign time out
	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0){
		printf("set time out error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	socklen_t len;	// Size of socket struct address
	int n;			// Number of bytes received

	// Send message to server
	sendto(sockfd, (const char *)"Client connected", 16,
		MSG_CONFIRM, (const struct sockaddr *) &servaddr,
			sizeof(servaddr));
	
	int i;
	int num;
	int pkt_losses = 0;
	for(i = 0; i < total_packets; i++){		
		sleep(INTERVAL);
		n = 0;
		n = recvfrom(sockfd, (int *)&num, sizeof(int),
					MSG_WAITALL, (struct sockaddr *) &servaddr,
					&len);
		if (n < 0) {
			pkt_losses++;
		}else
			printf("[Packet received] %d | %dBytes\n", num, n);
	}	
	close(sockfd);

	// Calc packet loss
	printf("=============================\n");	
	printf("Packet received:\t%d\n", total_packets-pkt_losses);
	printf("Packet loss:\t%d\n", pkt_losses);
	printf("Packet loss ratio:\t%.2f%%\n", (float)(pkt_losses*100)/total_packets);

	return 0;
}