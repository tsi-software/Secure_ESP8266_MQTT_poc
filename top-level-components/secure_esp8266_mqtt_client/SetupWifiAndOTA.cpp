/*
SetupWifiAndOTA.cxx

Author:  Warren Taylor
Created: 2019-01-21
Copyright (c) 2019 Warren Taylor
*/

#include <ESP8266WiFi.h>
//#include <ESP8266mDNS.h>
//#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <time.h>
#include "globals.h"
#include "SetupWifiAndOTA.h"


// Set time via NTP, as required for x.509 validation
void SetupWifiAndOTA::setClock() {

    //-------------------------------------------------------------------------
    //TODO: set clock to Zulu and set timezone and daylightsavings elsewhere.
    //      only use timezone for display purposes and data entry.
    //-------------------------------------------------------------------------

    //void configTime(int timezone, int daylightOffset_sec, const char* server1, const char* server2, const char* server3)
    //https://github.com/esp8266/Arduino/blob/master/cores/esp8266/time.c
    configTime(-8 * 3600, 0, "pool.ntp.org", "time.nist.gov", "time.windows.com");
    setClock_status = STARTED;
    DEBUG_LOG("Waiting for NTP time sync: ");
    setClock_AsyncWait.startWaiting(millis(), 1000); // Log every 1 second.
    // Asynchronously wait for network response via checkClockStatus().
}


// Check Clock Status and update 'setClock_status' accordingly.
void SetupWifiAndOTA::checkClockStatus() {
    time_t now = time(nullptr);
    if(now < 8 * 3600 * 2) {
        // The NTP request has not yet completed.
        if (!setClock_AsyncWait.isWaiting(millis())) {
            DEBUG_LOG(".");
            setClock_AsyncWait.startWaiting(millis(), 1000); // Log every 1 second.
        }
        return;
    }

    // The NTP request has now completed.
    setClock_status = SUCCESS;

    #ifdef DEBUG
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    DEBUG_LOGLN("");
    DEBUG_LOG("Current time: ");
    DEBUG_LOGLN(asctime(&timeinfo));
    #endif //DEBUG
}


String SetupWifiAndOTA::getMacAddress() {
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
void SetupWifiAndOTA::setupWifiAndOTA() {
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

    setClock();

    //TODO: improve security of OTA firmware updates.
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


// Loop
void SetupWifiAndOTA::loopWifiAndOTA() {
    // This must be first.
    ArduinoOTA.handle();

    // Prevent ALL other actions here until the clock as been set by NTP.
    if (setClock_status < FINISHED) {
        checkClockStatus();
        return;
    }

    //MilliSec currentMilliSec = millis();
}

