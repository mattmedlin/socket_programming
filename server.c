//Matt Medlin
//Web Server

#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <netinet/in.h>

#define MAX 4096

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in addr;
	
	FILE *web_data;
	web_data = fopen("index.html", "r");
	
	char sendline[MAX];
	fgets(sendline, MAX, web_data);
	
	char header[MAX] = "HTTP/1.1 200 OK\r\n\n";
	strcat(header, sendline);
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("Could not create socket");
		return -1;
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8001);
	addr.sin_addr.s_addr = INADDR_ANY;
	
	int retVal = bind( sockfd, (struct sockaddr*) &addr, sizeof(addr));
	if( retVal == -1 ){
		perror("Could not bind socket");
		return -1;
	}
	
	retVal = listen( sockfd, 5 );
	if( retVal == -1 )
	{
		perror("Could not listen to socket");
		return -1;
	}
	
	int client_sockfd;
	while(1)
	{
		client_sockfd = accept(sockfd, NULL, NULL);
		if( client_sockfd == -1 ){
			perror("Could not accept connection");
			return -1;
		}
		
		send(client_sockfd, header, sizeof(header), 0);
		close(client_sockfd);
	}
	
	return 0;
}

