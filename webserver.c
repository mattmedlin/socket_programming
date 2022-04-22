#include "utility.h"
#include "server1.h"
#include <sys/time.h>
#include <sys/ioctl.h>

#define MAX 4096
static int http_control(ssl,s)
	SSL *ssl;
	int s;
{
	char buf[BUF_SIZE];
	int r,len;

	char sendline[MAX];
	FILE *web_data;
	web_data = fopen("index.html","r");
	fgets(sendline,MAX,web_data);

	BIO *io, *ssl; //Use BIO APIs to read the client's request one line at a time

	/*Perform buffered reads and writes on the SSL connection by using the BIO_* functions on the new io project*/
	io = BIO_new(BIO_f_buffer());
	ssl_bio = BIO_new(BIO_f_ssl());
	BIO_set_ssl(ssl_bio,ssl, BIO_CLOSE);
	BIO_push(io,ssl_bio);

	/*Read the HTTP request sent from client*/
	while(1){
		r = BIO_gets(io,buf,BUF_SIZE-1); //takes an arbitary size bufferm and reads a line from the SSL connection into that buffer.

		switch(SSL_get_error(ssl,r)){
			case SSL_ERROR_NONE:
				len = r;
				break;
			default:
				berr_exit("SSL read problem");
		}

		/*Find the end of HTTP headers*/
		if(!strcmp(buf,"\r\n" || !strcmp(buf, "\n"))
				break;
		}

		/*Return html file*/
		if((r=BIO_puts(io,"HTTP/1.1 200 OK\r\n\n")) <= 0){
			perror("Write error");
			return -1;
		}
		if((r=BIO_puts(io,"Server: EKRServer\r\n\r\n"))<=0){
			perror("Write error");
			return -1;
		}
		if((r=BIO_puts(io,sendLine))<=0){
			perror("Write error");
			return -1;
		}

		if((r=BIO_flush(io))<0){
			perror("Write error");
			return -1;
		}

		r= SSL_shutdown(ssl);
	
		`if(!r){
			shutdown(s,1);
			r=SSL_shutdown(ssl);
		}

		switch(r){
			case 1:
				break; //Success
			case 0:
			case -1:
			default:
				berr_exit("Teardown failed");
		}

		SSL_free(ssl);
		close(s);

		return 0;
}

int main(argc, argv)
	int argc;
	char **argv;
{
	int sockfd, s;
	BIO *sbio;
	SSL_CTX *ctx;
	SSL *ssl;
	int r;

	pid_t pid;

	/*Build SSL context*/
	ctx=init_ctx(KEYFILE,PASSWORD);
	load_dh_params(ctx,DHFILE);

	sockfd = open_socket();

	while(1){
		if((s=accept(sockfd,0,0))<0){
			perror("Could not accept connection");
			return -1;
		}
		if((pid = fork())){
			close(s);
		}
		else{
			sbio=BIO_new_socket(s, BIO_NOCLOSE);
			ssl = SSL_new(ctx);
			SSL_set_bio(ssl,sbio,sbio);

			if((r=SSL_accept(ssl)<=1))
				berr_exit("SSL accept error");

			http_control(ssl,s);
			exit(0);
		}
	}
	destroy_ctx(ctx);
	exit(0);
}
		
	

