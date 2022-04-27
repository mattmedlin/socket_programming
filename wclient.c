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
#include <resolv.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define SERVER_PORT 443
#define MAX_bytes 4096
#define SA struct sockaddr

SSL_CTX* InitCTX(void)
{
	const SSL_METHOD *method;
	SSL_CTX *ctx;

	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
	method = TLS_client_method();
	ctx = SSL_CTX_new(method);
	if (ctx == NULL)
	{
		ERR_print_errors_fp(stderr);
		abort();
	}
	return ctx;
}

void ShowCerts(SSL* ssl)
{
	X509 *cert;
	char *line;

	cert = SSL_get_peer_certificate(ssl);
	if ( cert != NULL)
	{
		printf("Server certificates:\n");
		line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
		printf("Subject: %s\n", line);
		free(line);
		line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
		printf("Issuer: %s\n", line);
		free(line);
		X509_free(cert);
	}
	else
	{
		printf("Info: No client certificates configured.\n");
	}
}

int main(int argc, char **argv)
{
	SSL_CTX *ctx;
	SSL *ssl;
	char buf[MAX_bytes];
	int bytes;

	int sockfd, n;
	int sendbytes;
	struct sockaddr_in addr;
	char sendline[MAX_bytes];
	char recvline[MAX_bytes];

	memset(buf,'\0',sizeof(buf));

	if (argc != 2)
	{
		printf("Usage: ./client (address)\n");
		return 0;
	}

	SSL_library_init();
	ctx = InitCTX();

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
	if (retVal == -1){
		perror("Error: IP address not valid\n");
		return -1;
	}


	retVal = connect(sockfd, (SA *) &addr, sizeof(addr));
	if (retVal == -1){
		perror("Could not connect to server");
		return -1;
	}

	ssl = SSL_new(ctx);
	SSL_set_fd(ssl, sockfd);

	retVal = SSL_connect(ssl);
	if (retVal == -1){
		ERR_print_errors_fp(stderr);
	}

	sprintf(sendline, "GET / HTTP/1.1\r\n\r\n");
	sendbytes = strlen(sendline);

	SSL_write(ssl,sendline,sendbytes);

	while(1){
		int bytes_received = SSL_read(ssl,buf,sizeof(buf));
		if(bytes_received < 1){
			printf("Done reading.Close\n");
			break;
		}

		printf("%s",buf);
	}

	SSL_shutdown(ssl);
	close(sockfd);
	SSL_free(ssl);
	SSL_CTX_free(ctx);

	return 0;
}
