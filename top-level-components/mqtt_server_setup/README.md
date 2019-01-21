# Secure MQTT Server setup

## Creating the MQTT Server Certificate
Create a directory to store all Mosquitto certificates.
It is important to use different certificate subject parameters for your CA, server and clients.
**When prompted for the CN (Common Name), enter your server's hostname.**
Create 'mosquitto' user if it does not already exist.
* sudo useradd -M mosquitto
​* sudo usermod -L mosquitto

### Create an X509 CA certificate.
```bash
openssl req -new -x509 -days 365 -extensions v3_ca -keyout mosq_ca.key -out mosq_ca.crt -subj "/C=CA/ST=BC/L=**your-city**/O=**ca.your-domain.com**/OU=ca/CN=**your-hostname**/emailAddress=**your@email.com**"
```
> * C — Country
> * ST — State
> * L — City
> * O — Organization
> * OU — Organization Unit
> * CN — Common Name (eg: the main domain the certificate should cover)
> * emailAddress — main administrative point of contact for the certificate
*Also, enter and remember your PEM Pass Phrase*

### Create the MQTT server certificate and private key.
```bash
openssl genrsa -out mosq_serv.key 2048
```
### Create a self-signed server certificate.
```bash
openssl req -new -key mosq_serv.key -out mosq_serv.csr -subj "/C=CA/ST=BC/L=**your-city**/O=**server.your-domain.com**/OU=server/CN=**your-hostname**/emailAddress=**your@email.com**"
```
### Create the certificate to use in the MQTT Mosquitto Server.
```bash
openssl x509 -req -in mosq_serv.csr -CA mosq_ca.crt -CAkey mosq_ca.key -CAcreateserial -out mosq_serv.crt -days 365
```

### Generate a client key.
```bash
openssl genrsa -out mosq_client.key 2048
```

### Generate a certificate signing request to send to the CA.
```bash
openssl req -out mosq_client.csr -key mosq_client.key -new -subj "/C=CA/ST=BC/L=**your-city**/O=**client.your-domain.com**/OU=client/CN=**your-hostname**/emailAddress=**your@email.com**"
```

### Sign the CSR with your CA key, or send it to the CA:
```bash
openssl x509 -req -in mosq_client.csr -CA mosq_ca.crt -CAkey mosq_ca.key -CAcreateserial -out mosq_client.crt -days 365
```

## MQTT Server References
* <https://manpages.debian.org/stretch/mosquitto/mosquitto-tls.7.en.html>
* <https://manpages.debian.org/stretch/mosquitto/mosquitto_passwd.1.en.html>
* <https://www.openssl.org/docs/man1.1.0/apps/>
* <https://github.com/eclipse/mosquitto>
