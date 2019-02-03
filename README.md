# Secure ESP8266 MQTT proof of concept
There are many in depth and well documented ESP8266 MQTT tutorials out there. This in not one of them. This is a brief proof of concept where “security first” is the focus, rather than an afterthought. The goal of this project is to have a documented and working secure MQTT Broker and Client, which can be used as a reference for later projects.
If you’re a first timer this project probably isn’t for you.

## Features
- MQTT Publish/Subscribe Messaging secured using TLS 1.2
- OTA (Over-the-air) Client firmware updates.
- As of this writing esp8266 Secure Encrypted OTA is still in beta.

# Top Level Components
- Secure MQTT Server setup
- Secure ESP8266 MQTT Client
- AVR SPI Slave

# High Level Design 

1) Secure MQTT Broker
The MQTT Broker named Mosquitto is installed on the OpenWRT Wifi router. Running the MQTT Broker on the Wifi router is an experiment, which has so far worked just fine. However, the MQTT Broker could be run on any other computer on the same local area network (LAN). You could even run it on a Raspberry PI if performance isn’t a strict requirement.

2) Secure MQTT Client
Again, there are many tutorials on how to program the ESP8266, everyone’s favourite IoT chip, as an MQTT Client. But I did not come across any describing how to program the ESP8266 as a **secure** MQTT Client. As a long time software engineer it is my job to know how important security is these days.

3) AVR SPI Slave
The original plan was to connect peripherals directly to the ESP8266 data pins but I ran out of pins very quickly. Since I had an unused ATmega168 lying around I though “what the heck” and connected the two chips via SPI with the ESP8266 as the master and the ATmega as the slave. In additions to giving me a lot more data pins this scheme also gave me 5v tolerance. I’m also very happy I went this route because it has led to a number of additional useful ideas, which I will come to later.
