/*
 * UpdateRelays.h
 *
 * Author:  Warren Taylor
 * Created: 2020-08-19
 * Copyright (c) 2018-2020 Warren Taylor.  All right reserved.
 */

#ifndef UPDATERELAYS_H_
#define UPDATERELAYS_H_

#if (ARDUINO >= 100)
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif

#include "globals.h"
#include "Zones.h"


/*
NOTE:
Define ONLY ONE of UPDATE_RELAYS_USE_SPI or UPDATE_RELAYS_USE_GPIO depending on
whether you want to control the relays via SPI or GPIO, respectively.
*/
#define UPDATE_RELAYS_USE_SPI
//#define UPDATE_RELAYS_USE_GPIO

#if defined(UPDATE_RELAYS_USE_SPI) && defined(UPDATE_RELAYS_USE_GPIO)
#error Both UPDATE_RELAYS_USE_SPI and UPDATE_RELAYS_USE_GPIO cannot be defined at the same time!
#endif


extern void updateRelaysSetup(void);
extern void updateRelays(const Zones &zones);


#endif /* UPDATERELAYS_H_ */
