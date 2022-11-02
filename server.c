#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/*
sudo tc qdisc add dev wlp3s0 root netem loss 10%
sudo tc qdisc delete dev wlp3s0 root netem
*/
	
#define MAXLINE 1024	// Char size
#define INTERVAL 1 		// Seconds

/* Global variables */
int sockfd;						// Socket file descriptor
char buffer[MAXLINE];			// String buffer
struct sockaddr_in 	servaddr, 	// Socket server address
					cliaddr;	// Socket client address
	
int main(int argc, char *argv[]) {	

	if (argc != 3) {
		printf("Usage: server [PORT] [NUMBER OF PACKETS TO SEND]\n");
		exit(EXIT_FAILURE);
	}

	uint16_t port 		= htons(atoi(argv[1]));
	int total_packets 	= atoi(argv[2]);	

	// Creating server socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		printf("socket creation failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("-> Socket criado\n");

	memset(&servaddr, 0, sizeof(servaddr));	// clear server socket struct
	memset(&cliaddr, 0, sizeof(cliaddr));	// clear client socket struct
		
	// Assign server data
	servaddr.sin_family = AF_INET; 			// IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;	// IP Address(choose for primary network interface)
	servaddr.sin_port = port;				// Port
		
	// Bind - assign local address to a socket
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 ){
		printf("bind failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
		
	socklen_t 	len;	// size socket
	int 		n;		// number of bytes received
	
	len = sizeof(cliaddr);

	printf("-> Waiting for connection...\n");

	// Espera pelo primeiro cliente a se conectar
	n = recvfrom(sockfd, (char *)buffer, MAXLINE,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr,
				&len);
	buffer[n] = '\0';
	printf("%s\n", buffer);	
	printf("IP Adress: %s\n", inet_ntoa(cliaddr.sin_addr));	// Endereço IP do cliente
	printf("Port: %d\n", ntohs(cliaddr.sin_port));			// Porta de conexão do cliente

	// Envia uma quantidade de pacotes a cada x segundos
	int i;
	for (i = 0; i < total_packets; i++){
		sleep(INTERVAL);
		sendto(sockfd, (const int *)&i, sizeof(int),
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
				len);
		printf("Packet %d sent\n", i);
	}	
	return 0;
}