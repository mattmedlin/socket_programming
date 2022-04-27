#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define MAX 4096
int main(){
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
	if(!ctx){
		perror("SSL_CTX_new() failed.\n");
		return -1;
	}


	if(!SSL_CTX_use_certificate_file(ctx, "server.pem" , SSL_FILETYPE_PEM) || !SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM)){
		perror("SSL_CTX_use_certificate_file failed.\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}

	int sockfd;
	struct sockaddr_in addr;

	FILE *web_data;
	web_data = fopen("index.html", "r");

	char sendline[MAX];
	fgets(sendline, MAX, web_data);

	char header[MAX] = "HTTP/1.1 200 OK\r\n\n";
	strcat(header, sendline);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1){
		perror("Could not create socket");
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(443);
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

		SSL *ssl = SSL_new(ctx);
		if (!ssl){
			perror("SSL_new() failed.\n");
			return -1;
		}

		SSL_set_fd(ssl, client_sockfd);
		if(SSL_accept(ssl) <= 0){
			perror("Could not accept.\n");
			ERR_print_errors_fp(stderr);

			SSL_shutdown(ssl);
			close(client_sockfd);
			SSL_free(ssl);

			continue;
		}

		char request[1024];
		int recieved = SSL_read(ssl,request,1024);

		int sent = SSL_write(ssl, header, strlen(header));

		SSL_shutdown(ssl);
		close(client_sockfd);
		SSL_free(ssl);
		
	}

	close(sockfd);
	SSL_CTX_free(ctx);

	return 0;
}
