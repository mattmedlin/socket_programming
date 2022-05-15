- Modify the server.c into wserver.c
- wserver.c uses symmetric key to encrypt and decrypt data during a connection
- For now, the server operates on port 8080, we can change the port number later
- We have a self-signed certificate (localhost.pem) that will be sent to the client side to prove that our server is legit :))
- The privatekey.pem is used to encrypt the certificate
- When you first run the wserver, and try to connect with it through web browser, the browser will reject the self-signed certificate, warning that it is an insecured connection
- To access the server, you need set the certificate (localhost.pem) as a trusted certificate on your computer (I don't really know how to set it up on Linux but you can find instructions somewhere on Google)
- Refresh the browser and hopefully it works

Client:

compile: gcc -Wall -o client client.c -L/usr/lib -lssl -lcrypto
run: ./client (url)
create a cert: openssl req -x509 -nodes -days 365 -newkey rsa:1024 -keyout mycert.pem -out mycert.pem
