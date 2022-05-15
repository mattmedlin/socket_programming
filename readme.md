Server:
compile: gcc -Wall -g -L/opt/homebrew/opt/openssl@3/lib -I/opt/homebrew/opt/openssl@3/include -lssl -lcrypto -o wserver wserver.c 
-L/pathToopenssl/openssl@3/lib -I/pathToopenssl/openssl@3/include -lssl -lcrypto -o wserver wserver.c 
run: ./wserver
On a web browser, search for https://localhost:443

- On the first run, the browser will reject the self-signed certificate, warning that it is an insecured connection
- To access the server,set the certificate (server.pem) as a trusted certificate on your computer. Or just click Advance, procceding with the insecure connection.
- Refresh the browser 

Client:

compile: gcc -Wall -o client client.c -L/usr/lib -lssl -lcrypto
run: ./client (url)
create a cert: openssl req -x509 -nodes -days 365 -newkey rsa:1024 -keyout mycert.pem -out mycert.pem
