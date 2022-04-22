#include "utility.h"
#include <openssl/err.h>

BIO *bio_err=0;
static char *pass;
static int password_cb(char *buf, int num, int rwflag, void *userdata);
static void sigpipe_handle(int err);

/*Handle SSL errors using BIO objects*/
int berr_exit(string)
	char *string;
{
	BIO_printf(bio_err,"%s\n",string);
	ERR_print_errors(bio_err);
	return -1;
}

static int password_cb(char *buf, int num, int rwflag, void *userdata)
{
	if(num<strlen(pass)+1)
		return 0;
	strcpy(buf,pass);
	return(strlen(pass));
}

static void sigpipe_handle(int err){}

/*Set up a context object (SSL_CTX)
 * This object is used to create a new connection object for each new SSL connection that are used to do SSL handshakes, reads, writes
 * Each SSL connection will use the same keying material, CA, etc.*/

SSL_CTX *init_ctx(keyfile, password)
	char *keyfile;
	char *password;
{
	const SSL_METHOD *meth;
	SSL_CTX *ctx;

	if(!bio_err){
		/*Global OpenSSL library initialization*/
		SSL_library_init();
		SSL_load_error_strings(); //Allows mapping of OpenSSL errors to string
	
		/*Error write context*/
		bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);
	}

	/*SIGPIPE HANDLER*/
	signal(SIGPIPE, sigpipe_handle);

	/*Create context object*/
	meth = TLS_method(); //general-purpose version-flexible SSL/TLS method
	ctx = SSL_CTX_new(meth);//Create the context object using the meth as the connection method
	
	/*Load keys and certificates*/
	if(!(SSL_CTX_use_certificate_chain_file(ctx,keyfile))) //loads the chain of certificates for use with a SSL seesion using the ctx object
		berr_exit("Can't read certificate file");

	pass = password;
	//As private key is usually encrypted under a password, SSL_CTX_set_default_passwd_cb() is called to obtain the password
	SSL_CTX_set_default_passwd_cb(ctx, password_cb);

	 if(!(SSL_CTX_use_PrivateKey_file(ctx, keyfile, SSL_FILETYPE_PEM)))//Load the private key
		 berr_exit("Failed to read key file");

	 /*Load the trusted CA*/
	 if(!(SSL_CTX_load_verify_locations(ctx,CA_LIST,0)))
		 berr_exit("Failed to read CA list");
	return ctx;
}

void destroy_ctx(ctx)
	SSL_CTX *ctx;
{
	SSL_CTX_free(ctx);
}
int main() {
	       printf("Hello, World!");
	          return 0;
}




