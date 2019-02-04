/*
SetupWifi.h

Author:  Warren Taylor
Created: 2019-01-21
Copyright (c) 2019 Warren Taylor.  All right reserved.
*/

#ifndef _SetupWifi_H_
#define _SetupWifi_H_


#include <ESP8266WiFi.h>
#include "AsyncWait.h"


class SetupWifi {
public:
    SetupWifi(
        const char *ssid_,
        const char *password_,
        const char *ca_cert_PROG_,
        const char *client_cert_PROG_,
        const char *client_key_PROG_
    )
       //TODO: fix the naming convention with the trailing underscore.
     : ssid(ssid_)
     , password(password_)

       //TODO: ?? the following three may not need to be persisted ??
     , ca_cert_PROG(ca_cert_PROG_)
     , client_cert_PROG(client_cert_PROG_)
     , client_key_PROG(client_key_PROG_)

     , x509CaCert(ca_cert_PROG)
     , x509ClientCert(client_cert_PROG)
     , PrivateClientKey(client_key_PROG)
    {
        wifiClient.setTrustAnchors(&x509CaCert);
        wifiClient.setClientRSACert(&x509ClientCert, &PrivateClientKey);
    }

    virtual ~SetupWifi() { }

    WiFiClient & getWiFiClient() {
        return wifiClient;
    }

    bool isReadyForProcessing() {
        return (setClock_status > FINISHED);
    }

    String getMacAddress();
    void setupWifi();
    void loopWifi();

private:
    const char *ssid;
    const char *password;

    //TODO: ?? the following three may not need to be persisted ??
    const char *ca_cert_PROG;
    const char *client_cert_PROG;
    const char *client_key_PROG; // KEEP THIS VALUE PRIVATE AND SECURE!!!

    BearSSL::WiFiClientSecure wifiClient;
    BearSSL::X509List x509CaCert;
    BearSSL::X509List x509ClientCert;
    BearSSL::PrivateKey PrivateClientKey;

    // Note: variables of type 'status' are NEVER assigned a value of 'FINISHED'.
    // 'FINISHED' is only used is comparisons (e.g. if(x_status>FINISHED)... ),
    // rather than always testing for SUCCESS || FAILED.
    enum status { NOT_STARTED, STARTED, FINISHED, SUCCESS, FAILED };
    status setClock_status = NOT_STARTED;
    AsyncWait setClock_AsyncWait;

    void setClock();
    void checkClockStatus();
};


#endif // _SetupWifi_H_
