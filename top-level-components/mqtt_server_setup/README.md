# MQTT Broker secure setup
*Copyright (c) 2019-2023 Warren Taylor.*

Here are the basics of how to install, configure, and secure the “Mosquitto” MQTT Broker on an already properly configured and running installation of [OpenWRT](https://openwrt.org/). However, the MQTT Broker can run on any computer on a local area network (LAN). You could even run it on a Raspberry PI if performance isn’t a strict requirement. So the following instruction should be adaptable to most modern operating systems.

## Disclaimer
The following documentation in no way guarantees a secure system.
*See [LICENSE](../../LICENSE).*

## Install Mosquitto Broker and Client
With ssh (or similar) log into your OpenWRT router.
```bash
ssh root@your-router-hostname
opkg install mosquitto-ssl mosquitto-client-ssl libmosquitto-ssl
```

## Create "mosquitto" user if it does not already exist
```bash
useradd -M mosquitto
usermod -L mosquitto
```

## Create a directory to securely hold your certificates
*(Keys and Certificates can be generate on any computer as long as the required files are securely copied to the server and all keys are securely stored.)*
```bash
mkdir /root/mosquitto
chmod go-rwx /root/mosquitto
chown mosquitto:mosquitto /root/mosquitto
cd /root/mosquitto
```

## Creating the MQTT Keys and Certificates
The documentation below talks a lot about Keys, Certificates, Certificate Authorities, etc...
Rather than trying to rewrite the very good documentation others have already put a great deal of effort into, I refer you to some of their work:
* [SSL and SSL Certificates Explained For Beginners](http://www.steves-internet-guide.com/ssl-certificates-explained/)
* [Introduction to MQTT Security Mechanisms](http://www.steves-internet-guide.com/mqtt-security-mechanisms/)
* [Mosquitto SSL Configuration -MQTT TLS Security](http://www.steves-internet-guide.com/mosquitto-tls/)
* [Securing a Mosquitto Server](https://dzone.com/articles/mqtt-security-securing-a-mosquitto-server)

**NEVER** use the same key and certificate to secure more than one device.
If one device becomes compromised then all devices secured with the same key and certificate are also compromised.<br>
When generating your credentials it is important to use different subject parameters for your CA, server and client certificates.<br>
**Every time you are prompted for the CN (Common Name), enter your same server hostname.**
If you don't know your exact hostname then run something like:
```bash
uci show system
```

### Create an X509 CA key and certificate for self-signing
*(Determine and securely store a PEM Pass Phrase, which is used to protect your CA Key)*
```bash
openssl req -new -x509 -days 365 -extensions v3_ca -keyout mosq_ca.key -out mosq_ca.crt -subj "/C=CA/ST=BC/L=your-city/O=ca.your-domain.com/OU=ca/CN=your-hostname/emailAddress=your@email.com"
```
Subject Parameters:
* C - Country
* ST - State
* L - City
* O - Organization
* OU - Organization Unit
* CN - Common Name (eg: the main domain the certificate should cover)
* emailAddress - main administrative point of contact for the certificate

To verify:
```bash
openssl x509 -in mosq_ca.crt -noout -text
```

### Generate the MQTT Server private key
```bash
openssl genrsa -out mosq_serv.key 2048
```
### Generate the MQTT Server self-signed certificate
```bash
openssl req -new -key mosq_serv.key -out mosq_serv.csr -subj "/C=your-country/ST=your-state/L=your-city/O=server.your-domain.com/OU=server/CN=your-hostname/emailAddress=your@email.com"
```
### Generate the CA signed certificate to use in the MQTT Mosquitto Server
```bash
openssl x509 -req -in mosq_serv.csr -CA mosq_ca.crt -CAkey mosq_ca.key -CAcreateserial -out mosq_serv.crt -days 365
```

### Generate the MQTT Client private key
```bash
openssl genrsa -out mosq_client.key 2048
```

### Generate the MQTT Client self-signed certificate
```bash
openssl req -new -key mosq_client.key -out mosq_client.csr -subj "/C=your-country/ST=your-state/L=your-city/O=client.your-domain.com/OU=client/CN=your-hostname/emailAddress=your@email.com"
```

### Generate the CA signed certificate to use in the MQTT Client
```bash
openssl x509 -req -in mosq_client.csr -CA mosq_ca.crt -CAkey mosq_ca.key -CAcreateserial -out mosq_client.crt -days 365
```

### Increase the security of the files just created
```bash
chmod go-rwx /root/mosquitto/*
chown mosquitto:mosquitto /root/mosquitto/*
```

## Mosquitto server config
**/etc/mosquitto/mosquitto.conf**
```
port 8883
cafile /etc/mosquitto/mosq_ca.crt
keyfile /etc/mosquitto/mosq_serv.key
certfile /etc/mosquitto/mosq_serv.crt
protocol mqtt
tls_version tlsv1.2
require_certificate true
```

### Copy the necessary certificates and key
```bash
cp /root/mosquitto/mosq_ca.crt   /etc/mosquitto/mosq_ca.crt
cp /root/mosquitto/mosq_serv.key /etc/mosquitto/mosq_serv.key
cp /root/mosquitto/mosq_serv.crt /etc/mosquitto/mosq_serv.crt
```

## Restart the Mosquitto Broker
```bash
service mosquitto stop
service mosquitto start
```

## Run a quick test
The following command should turn on zone 2 for 2 minutes (i.e. 120 seconds).
Try running it now, even if the MQTT client has not yet been setup, in order to check for any errors that may occur on the server side.
```bash
mosquitto_pub -h your-hostname -p 8883 \
    --cafile mosq_ca.crt --cert mosq_serv.crt --key mosq_serv.key \
    --debug --topic irrigation/zone/on -m "2 120"
```

### Error: The connection was refused.
If you are using a newer version of the Mosquitto Broker
you may get the following error messages:
```
Client (null) sending CONNECT
Client (null) received CONNACK (5)
Connection error: Connection Refused: not authorised.
Error: The connection was refused.
```
The quick solution is to update **mosquitto.conf** with the following line
and then Restart the Mosquitto Broker:
```
allow_anonymous true
```
However, this solution is a "double edge sword".
Sure, it gets things running but you may now be less secure than you require.
A more complicated configuration could include:
```
...
allow_anonymous false
use_identity_as_username true
...
```
Or even require user name and password in addition to using security certificates and keys.

There are many more Mosquitto Security related configuration settings but
I cannot tell you what to do because everyones security requirements are different
and these are getting into complicated configurations that are beyond the scope of this project.

## MQTT Server References
* <https://manpages.debian.org/stretch/mosquitto/mosquitto-tls.7.en.html>
* <https://manpages.debian.org/stretch/mosquitto/mosquitto_passwd.1.en.html>
* <https://www.openssl.org/docs/man1.1.0/apps/>
* <https://github.com/eclipse/mosquitto>
