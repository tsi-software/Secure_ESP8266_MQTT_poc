# Secure ESP8266 MQTT Client
*Copyright (c) 2019 Warren Taylor  All right reserved.*

There are many tutorials on how to program the ESP8266 as an MQTT Client using the Arduino libraries. But it's hard to find a single source clearly describing how to program the ESP8266 as a **secure** MQTT Client. As a long time software engineer I know how important security is these days.
This project uses the
[ESP8266 Arduino Core](https://arduino-esp8266.readthedocs.io/)
library and board specifications.

## Security First
At a high level of abstraction secure connections for ESP8266 devices are straight forward when using the arduino-esp8266 **BearSSL::WiFiClientSecure** class.
This is because the **BearSSL::WiFiClientSecure** class extends from the insecure arduino-esp8266 **WiFiClient** class.
The complicated part is properly initializing a BearSSL::WiFiClientSecure object, which is what this project focuses on.
Once the secure initialization is done the remaining code can be implemented mostly the same as the insecure **WiFiClient** class, of which there are many examples.

## ArduinoOTA
The ArduinoOTA library was initially used but later removed from this code because there did not appear to be sufficient resources in the ESP8266 to have a secure connection open by both the MQTT Broker and ArduinoOTA at the same time.
In the near future, Secure OTA will be implemented differently in order to coexist with Secure MQTT.

## Source Files

### secure_esp8266_mqtt_client.ino
Top level source code that:
* Connects to WAP.
* Securely connects to MQTT Broken.
* Subscribes to desired MQTT topics.
* Handles incoming MQTT messages by using callback functions.
* Controls ESP8266 Chip Level Data Pins.
* Configures and manages communications over SPI with connected ATmega168.

### SetupWifi.cpp and SetupWifi.h
The **SetupWifi** class handles the security and encapsulates the arduino-esp8266 **BearSSL::WiFiClientSecure** object.
This class holds the ssid and password of the Wifi Router and implements the code to connect to this router.
This class also holds the ca_cert, client_cert, and client_key used to make a secure connection using the **BearSSL::WiFiClientSecure** class. More on these certs and keys in a moment.
Another detail handled by this class, that is not at first obvious, is accurately setting the ESP8266 clock.
This is needed because security certificates can, and should, have an expiry date.
Expiry dates are important to prevent old and possibly compromised certificates from being reused.


### secure_credentials.h


### AsyncWait.h

### Zones.h
This is application level code and is independent of security.

### globals.h

## Future Features

### ESP8266 OTA

### AVR SPI Slave OTA

## MQTT Client References
<https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html>
<https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/bearssl-client-secure-class.html>
<https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/bearssl-client-secure-class.rst>
<https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/examples>
<https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiClientSecureBearSSL.h>
<https://pubsubclient.knolleary.net/>

# Linux - Secure MQTT Client Certificate setup

# Android - Secure MQTT Client Certificate setup
