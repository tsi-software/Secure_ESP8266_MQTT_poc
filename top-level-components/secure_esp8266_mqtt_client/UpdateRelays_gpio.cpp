/*
 * UpdateRelays_gpio.cpp
 *
 * Author:  Warren Taylor
 * Created: 2020-08-19
 * Copyright (c) 2018-2020 Warren Taylor.  All right reserved.
 */

#include "UpdateRelays.h"

//------------------------------------------------------------------------------
#if defined(UPDATE_RELAYS_USE_GPIO)



static const unsigned short NUM_OF_ZONES = 8;

// FAIL !!!
static uint8_t zonePins[ NUM_OF_ZONES ] = {
    5,  // Zone 0
    4,
    14,
    12,
    13,
    10,
    9,
    5   // Zone 7
};


extern void updateRelaysSetup(void)
{
    for (unsigned short ndx = 0; ndx < NUM_OF_ZONES; ++ndx) {
        // Default to INPUT - High Impedence.
        pinMode(zonePins[ndx], INPUT);
    }
}


void updateRelays(const Zones &zones)
{
    for (unsigned short ndx = 0; ndx < NUM_OF_ZONES; ++ndx) {
        const uint8_t pin = zonePins[ndx];

        // A low output value energizes the relay.
        // A high value (or high impedence) turns off the relay.
        // This is inverted logic.
        if (zones.isZoneOn(ndx)) {
            pinMode(pin, OUTPUT);
            digitalWrite(pin, LOW);
        } else {
            // INPUT - High Impedence.
            pinMode(pin, INPUT);
        }
    }
}


#endif // defined(UPDATE_RELAYS_USE_GPIO)
//------------------------------------------------------------------------------
