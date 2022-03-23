//Matt Medlin
//Web Client

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

#define SERVER_PORT 80
#define MAX 4096
#define SA struct sockaddr

int main(int argc, char **argv)
{
	int sockfd, n;
	int sendbytes;
	struct sockaddr_in addr;
	char sendline[MAX];
	char recvline[MAX];
	
	if (argc != 2)
	{
		printf("Usage: ./client (address)\n");
		return 0;
	}
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("Could not create socket");
		return -1;
	}
	
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
	
	int retVal = inet_pton(AF_INET, argv[1], &addr.sin_addr);
	if (retVal == -1)
	{
		perror("Error: IP address not valid\n");
		return -1;
	}
	
	retVal = connect(sockfd, (SA *) &addr, sizeof(addr));
	if (retVal == -1)
	{
		perror("Could not connect to server");
		return -1;
	}
	
	sprintf(sendline, "GET / HTTP/1.1\r\n\r\n");
	sendbytes = strlen(sendline);
	
	retVal = write(sockfd, sendline, sendbytes);
	if (retVal == -1)
	{
		perror("Could not write");
		return -1;
	}
	
	
	while((n = read(sockfd, recvline, MAX-1)) > 0)
	{
		printf("%s", recvline);
		memset(recvline, 0, MAX);
	}
	
	if (n < 0)
	{
		perror("Could not read");
		return -1;
	}
	
	exit(0);
}
