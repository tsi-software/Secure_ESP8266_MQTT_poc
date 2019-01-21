/*
SetupWifiAndOTA.h

Author:  Warren Taylor
Created: 2019-01-21
Copyright (c) 2019 Warren Taylor
*/

#ifndef _SetupWifiAndOTA_H_
#define _SetupWifiAndOTA_H_


extern const char* mqtt_server;


extern WiFiClient wifiClient;


extern String getMacAddress();
extern void setupWifiAndOTA();
extern void loopWifiAndOTA();


#endif // _SetupWifiAndOTA_H_
