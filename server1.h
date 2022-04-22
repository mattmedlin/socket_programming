#ifndef _server1_h
#define _server_h

#define KEYFILE "server.pem"
#define PASSWORD "plzwork"
#define DHFILE "dhparam.pem"

int open_socket(void);
void load_dh_params(SSL_CTX *ctx, char *file);
void generate_eph_rsa_key(SSL_CTX *ctx);

#endif
