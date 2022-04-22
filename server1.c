#include "server1.h"
#include "utility.h"

int open_socket(){
	int sockfd;
	struct sockaddr_in addr;

	int val = 1;
	
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Could not create socket");
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = INADDR_ANY;
	
	/*When TCP socket is closed, the socket is kept lingering for a while is a state called TIME_WAIT			
	 *This prevent the port number being reused for anoter service until ther aren't any outdated packets from the old connection
	 *Good for client sockets as the number of outgoing ports is huge
	 *Not good for servers which listen to connection as they need to re-acquire the same port number when they restart
	 *SO_REUSEADDR allows the port number to be reused by a new socket immediately */
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	if(bind(sockfd, (struct sockaddr*) &addr, sizeof(addr)) < 0){
		perror("Could not bind socket");
		return -1;
	}

	listen(sockfd, 5);
	return(sockfd);
}


/*Perform a DH key exchange*/
void load_dh_params(ctx,file)
	SSL_CTX *ctx;
	char *file;
{
	DH *ret = 0;
	BIO *bio;

	if((bio=BIO_new_file(file,"r")) == NULL)
		berr_exit("Failed to open DH file");


	ret = PEM_read_bio_DHparams(bio, NULL,NULL,NULL);
	BIO_free(bio);

	if(SSL_CTX_set_tmp_dh(ctx,ret)<0)
		berr_exit("Failed to set DH parameters");
}


/*generates a 2-prime RSA keypair and store it in RSA structure provided in rsa*/
void generate_eph_rsa_key(ctx)
	SSL_CTX *ctx;
{
	EVP_PKEY *pkey;
	pkey = EVP_PKEY_new();
	BIGNUM *bn;
	bn = BN_new();
	BN_set_world(bn,RSA_F4);
	RSA *rsa;
	rsa = RSA_new();
	RSA_generate_key_ex(rsa,2048,bn,NULL);

	EVP_PKEY_assign_RSA(pkey, rsa);
	EVP_PKEY_free(pkey);
}
