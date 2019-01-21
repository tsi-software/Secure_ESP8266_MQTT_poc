/*
SetupWifiAndOTA.h

Author:  Warren Taylor
Created: 2019-01-21
Copyright (c) 2019 Warren Taylor
*/

#ifndef _SetupWifiAndOTA_H_
#define _SetupWifiAndOTA_H_


class SetupWifiAndOTA {
public:
    SetupWifiAndOTA() : ssid("STASSID"), password("STAPSK") { }
    virtual ~SetupWifiAndOTA() { }

    String getMacAddress();
    void setupWifiAndOTA();
    void loopWifiAndOTA();

    WiFiClient & getWiFiClient() {
        return wifiClient;
    }

private:
    const char* ssid;
    const char* password;

    WiFiClient wifiClient;

    void setClock();
};


#endif // _SetupWifiAndOTA_H_
