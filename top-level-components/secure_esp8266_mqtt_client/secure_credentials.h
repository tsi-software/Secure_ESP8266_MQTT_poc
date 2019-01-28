/*
    Secure Credentials
*/

#ifndef _SECURE_CREDENTIALS_H_
#define _SECURE_CREDENTIALS_H_


const char *STASSID = "your-ssid";
const char *STAPSK = "your-ssid-password";

const char* MQTT_SERVER = "your-mqtt-server";

const char CA_CERT_PROG[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
... your certificate goes here ...
-----END CERTIFICATE-----
)EOF";

const char CLIENT_CERT_PROG[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
... your certificate goes here ...
-----END CERTIFICATE-----
)EOF";

// KEEP THIS VALUE PRIVATE AND SECURE!!!
const char CLIENT_KEY_PROG[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
... your private key goes here ...
-----END RSA PRIVATE KEY-----
)KEY";


#endif // _SECURE_CREDENTIALS_H_
