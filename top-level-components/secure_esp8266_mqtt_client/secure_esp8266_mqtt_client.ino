/****
Secure ESP8266 MQTT Client
version 00.01

----------
 Features:
----------
MQTT
OTA firmware revision
8 Relay Module controller

Warren Taylor
2018-12-30

--------
 Notes:
--------
  screen /dev/ttyUSB0 115200
  clean   ctrl-A C
  quit -- ctrl-A k
       or ctrl-A \

  openssl genrsa -out private.key 2048
  openssl rsa -in private.key -outform PEM -pubout -out public.key

  ~/bin/arduino-1.8.8/arduino  ~/github/tsi-software/Secure_ESP8266_MQTT_poc/top-level-components/secure_esp8266_mqtt_client/secure_esp8266_mqtt_client.ino
  ~/bin/arduino-1.8.8/arduino  --verify  ~/github/tsi-software/Secure_ESP8266_MQTT_poc/top-level-components/secure_esp8266_mqtt_client/secure_esp8266_mqtt_client.ino
  ~/bin/arduino-1.8.8/arduino  --upload  ~/github/tsi-software/Secure_ESP8266_MQTT_poc/top-level-components/secure_esp8266_mqtt_client/secure_esp8266_mqtt_client.ino
*/

#define MAJOR_VER "00"
#define MINOR_VER "01"

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <SPI.h>
#include "AsyncWait.h"
#include "globals.h"
#include "Zones.h"

//TODO: implement secure credintials as a runtime config file
//      rather than a header file.
#include "/home/wtaylor/private/Secure_ESP8266_MQTT/secure_credentials.h"
//#include "secure_credentials.h"

const char* ssid = STASSID;
const char* password = STAPSK;
const char* mqtt_server = MQTTSRV;
//IPAddress broker(192,168,1,1); // IP address of your MQTT broker

const char *ID = "sec_mqtt_client_" MAJOR_VER "_" MINOR_VER;  // Name of our device, must be unique
const String TOPIC_ZONE_ON("irrigation/zone/on");
const String TOPIC_ZONE_OFF("irrigation/zone/off");
const String TOPIC_ZONE_STATUS("irrigation/zone/status");


// TODO: Use Metaprogramming to automate the zone indexing.
static ZoneStatus zoneArray[] {
  ZoneStatus(0), ZoneStatus(1), ZoneStatus(2), ZoneStatus(3),
  ZoneStatus(4), ZoneStatus(5), ZoneStatus(6), ZoneStatus(7)
};

static Zones zones( zoneArray, sizeof(zoneArray)/sizeof(zoneArray[0]) );
static WiFiClient wifiClient;
static PubSubClient pubsubClient(wifiClient);

// SPI setup.
const int slaveSelectPin = 16;
static SPISettings spiZoneSettings(2000000, MSBFIRST, SPI_MODE0);

void updateRelays(uint8_t bitmap) {
  // A low output value energizes the relay.
  // A high value turns off the relay.
  // So invert each bit.
  bitmap ^= 0xFF;

  DEBUG_LOG("updateRelays: ");
  DEBUG_LOGLN(String(bitmap, BIN));

  SPI.beginTransaction(spiZoneSettings);
  digitalWrite(slaveSelectPin, LOW);
  SPI.transfer(bitmap);
  digitalWrite(slaveSelectPin, HIGH);
  SPI.endTransaction();
}


// Handle incomming messages from the broker
void callback(char* topic, byte* payload, unsigned int length) {
  String topicStr;
  String payloadStr;

  for (int i = 0; topic[i]; i++) {
    topicStr += topic[i];
  }

  for (int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }

  DEBUG_LOGLN("");
  DEBUG_LOG("Message arrived - [");
  DEBUG_LOG(topicStr);
  DEBUG_LOG("] ");
  DEBUG_LOGLN(payloadStr);

  if (topicStr == TOPIC_ZONE_ON) {
    callbackZoneOn(topicStr, payloadStr);
  } else if (topicStr == TOPIC_ZONE_OFF) {
    callbackZoneOff(topicStr, payloadStr);
  } else if (topicStr == TOPIC_ZONE_STATUS) {
    callbackZoneStatus(topicStr, payloadStr);
  }
}


void callbackZoneOn(const String &topicStr, const String &payloadStr) {
  MilliSec currentMilliSec = millis();

  // Payload in "[Zone-Index] [Duration-in-seconds]"
  String buffer(payloadStr);
  String zoneIndexStr;
  String durationStr;
  buffer.trim();
  int delimIndex = buffer.indexOf(' ');
  if (delimIndex < 0) {
    // Delimeter not found! Leave 'durationStr' empty.
    zoneIndexStr = buffer;
  } else {
    zoneIndexStr = buffer.substring(0, delimIndex);
    durationStr = buffer.substring(delimIndex + 1);
  }

  if (zoneIndexStr.length() <= 0 || durationStr.length() <= 0) {
    // Invalid Payload:
    DEBUG_LOGLN("Invalid Payload! zoneIndexStr=" + zoneIndexStr + ", durationStr=" + durationStr);
    return;
  }

  unsigned zoneIndex = (unsigned)zoneIndexStr.toInt();
  int duration = durationStr.toInt();
  if (duration < 0)  { duration = 0; }

  bool changed = false;
  if (duration > 0) {
    changed = zones.turnOn(zoneIndex, (unsigned)duration, currentMilliSec);
  } else {
    changed = zones.turnOff(zoneIndex);
  }
  if (changed) {
    updateRelays(zones.asBitMap8());
  }
}


void callbackZoneOff(const String &topicStr, const String &payloadStr) {
  MilliSec currentMilliSec = millis();
  //TODO: if needed.
}


void callbackZoneStatus(const String &topicStr, const String &payloadStr) {
  MilliSec currentMilliSec = millis();
  //TODO: if needed.
}


/****
void process_response(String &response, byte pin, const char *state_topic) {
  if(response == "on")
  {
    digitalWrite(pin, HIGH);
    pubsubClient.publish(state_topic,"on");
    //DEBUG_LOG(pin);
    //DEBUG_LOG("=HIGH, ");
    //DEBUG_LOG(state_topic);
    //DEBUG_LOGLN("=on");
  }
  else if(response == "off")  // Turn the light off
  {
    digitalWrite(pin, LOW);
    pubsubClient.publish(state_topic,"off");
    //DEBUG_LOG(pin);
    //DEBUG_LOG("=LOW, ");
    //DEBUG_LOG(state_topic);
    //DEBUG_LOGLN("=off");
  }
}
****/


String getMacAddress() {
  byte mac[6];
  String macStr;

  WiFi.macAddress(mac);
  macStr = String(mac[0], HEX) + ":"
         + String(mac[1], HEX) + ":"
         + String(mac[2], HEX) + ":"
         + String(mac[3], HEX) + ":"
         + String(mac[4], HEX) + ":"
         + String(mac[5], HEX);
  
  return macStr;
}


// Connect to WiFi network and check for OTA firmware update.
void setupWifiAndOTA() {
  DEBUG_LOGLN("");
  DEBUG_LOG("MAC ");
  DEBUG_LOGLN(getMacAddress());
  DEBUG_LOG("Connecting to ");
  DEBUG_LOGLN(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    DEBUG_LOGLN("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  DEBUG_LOGLN("");
  DEBUG_LOGLN("WiFi connected");
  DEBUG_LOG("IP ");
  DEBUG_LOG(WiFi.localIP());
  //DEBUG_LOG(", MAC ");
  //DEBUG_LOG(getMacAddress());
  DEBUG_LOGLN("");
  randomSeed(micros());

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    DEBUG_LOGLN("OTA: Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    DEBUG_LOGLN("\nOTA: End");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    DEBUG_LOGF("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    DEBUG_LOGLN("");
    DEBUG_LOGLN("------------------------------------------------------");
    DEBUG_LOGF("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      DEBUG_LOGLN("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      DEBUG_LOGLN("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      DEBUG_LOGLN("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      DEBUG_LOGLN("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      DEBUG_LOGLN("End Failed");
    }
    DEBUG_LOGLN("------------------------------------------------------");
    DEBUG_LOGLN("");
  });
  
  ArduinoOTA.begin();
  DEBUG_LOGLN("Ready.");
  //DEBUG_LOG("IP address: ");
  //DEBUG_LOGLN(WiFi.localIP());
}


// Reconnect to the MQTT client.
void reconnectToMQTT(MilliSec currentMilliSec) {
  if (pubsubClient.connected()) {
    // We are connected so nothing further needs to be done.
    return;
  }

  static AsyncWait waitToRetry;
  if (waitToRetry.isWaiting(currentMilliSec)) {
    return;
  }

  DEBUG_LOG("Attempting MQTT connection...");
  // Attempt to connect
  if (pubsubClient.connect(ID)) {
    DEBUG_LOGLN("connected");

    pubsubClient.subscribe(TOPIC_ZONE_ON.c_str());
    DEBUG_LOG("Subcribed to: ");
    DEBUG_LOGLN(TOPIC_ZONE_ON);

    pubsubClient.subscribe(TOPIC_ZONE_OFF.c_str());
    DEBUG_LOG("Subcribed to: ");
    DEBUG_LOGLN(TOPIC_ZONE_OFF);
    
    pubsubClient.subscribe(TOPIC_ZONE_STATUS.c_str());
    DEBUG_LOG("Subcribed to: ");
    DEBUG_LOGLN(TOPIC_ZONE_STATUS);
    
  } else {
    DEBUG_LOGLN(" try again in 5 seconds.");
    waitToRetry.startWaiting(currentMilliSec, 5000);
  }
}


void setup() {
  #ifdef DEBUG
  Serial.begin(115200); // Start serial communication at 115200 baud
  #endif

  setupWifiAndOTA();
  //pubsubClient.setServer(broker, 1883);
  pubsubClient.setServer(mqtt_server, 1883);
  pubsubClient.setCallback(callback); // Initialize the callback routine
  zones.Setup();

  // SPI Master setup.
  // set the Slave Select Pins as outputs:
  pinMode(slaveSelectPin, OUTPUT);
  digitalWrite(slaveSelectPin, HIGH);
  SPI.begin();
}


#ifdef DEBUG
void startupTest(MilliSec currentMilliSec) {
  static AsyncWait startupTestWait;
  static uint8_t startupTestValue;
  static bool firstTime = true;
  static const unsigned turnOnSeconds = 1; // 1 second.
  static const unsigned nextIterationDuration = 1250; // 1,250 milliseconds.

  bool changed = false;

  if (firstTime) {
    firstTime = false;
    startupTestValue = 0;
    changed = zones.turnOn(startupTestValue, turnOnSeconds, currentMilliSec);
    startupTestWait.startWaiting(currentMilliSec, nextIterationDuration);
  }

  if (startupTestValue >= 0 && startupTestValue <= 7) {
    if (!startupTestWait.isWaiting(currentMilliSec)) {
      ++startupTestValue;
      changed = zones.turnOn(startupTestValue, turnOnSeconds, currentMilliSec);
      startupTestWait.startWaiting(currentMilliSec, nextIterationDuration);
    }
  }

  if (changed) {
    updateRelays(zones.asBitMap8());
  }
}
#endif // DEBUG


void loop() {
  ArduinoOTA.handle();

  /***/
  if (!pubsubClient.connected()) {
    // Reconnect if connection is lost.
    MilliSec currentMilliSec = millis();
    reconnectToMQTT(currentMilliSec);
  }
  pubsubClient.loop();
  /***/

  { // App code.
    MilliSec currentMilliSec = millis();

    #ifdef DEBUG
    startupTest(currentMilliSec);
    #endif

    bool zonesChanged = zones.Loop(currentMilliSec);
    if (zonesChanged) {
      updateRelays(zones.asBitMap8());
    }
  }

}
