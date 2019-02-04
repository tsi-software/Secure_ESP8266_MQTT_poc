/*
SetupWifi.cpp

Author:  Warren Taylor
Created: 2019-01-21
Copyright (c) 2019 Warren Taylor.  All right reserved.
*/

#include <ESP8266WiFi.h>
#include <time.h>
#include "globals.h"
#include "SetupWifi.h"


// Set time via NTP, as required for x.509 validation
void SetupWifi::setClock() {

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
void SetupWifi::checkClockStatus() {
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


String SetupWifi::getMacAddress() {
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


// Connect to WiFi network.
void SetupWifi::setupWifi() {
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

    DEBUG_LOGLN("Ready.");
    //DEBUG_LOG("IP address: ");
    //DEBUG_LOGLN(WiFi.localIP());
}


// Loop
void SetupWifi::loopWifi() {
    // Prevent ALL other actions here until the clock as been set by NTP.
    if (setClock_status < FINISHED) {
        checkClockStatus();
        return;
    }

    //MilliSec currentMilliSec = millis();
}

