/*
 * UpdateRelays_spi.cpp
 *
 * Author:  Warren Taylor
 * Created: 2020-08-19
 * Copyright (c) 2018-2020 Warren Taylor.  All right reserved.
 */

#include "UpdateRelays.h"

//------------------------------------------------------------------------------
#if defined(UPDATE_RELAYS_USE_SPI)
#include <SPI.h>


// SPI setup.
const int slaveSelectPin = 16;
static SPISettings spiZoneSettings(2000000, MSBFIRST, SPI_MODE0);

// For Future Use.
const int programResetPin = 4;
const int spiRxRequestPin = 5;


extern void updateRelaysSetup(void)
{
    // Set programResetPin to high impedance and default its value to HIGH.
    pinMode(programResetPin, INPUT);
    digitalWrite(programResetPin, HIGH);

    // SPI Master setup.
    // set the Slave Select Pins as outputs:
    pinMode(slaveSelectPin, OUTPUT);
    digitalWrite(slaveSelectPin, HIGH);
    SPI.begin();
}


void updateRelays(const Zones &zones)
{
    uint8_t bitmap = zones.asBitMap8();

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


#endif // defined(UPDATE_RELAYS_USE_SPI)
//------------------------------------------------------------------------------
