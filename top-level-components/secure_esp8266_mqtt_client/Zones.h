/*
 * Zones.h
 *
 * Author:  Warren Taylor
 * Created: 2018-12-30
 * Copyright (c) 2018-2019 Warren Taylor.  All right reserved.
 */

#ifndef ZONES_H_
#define ZONES_H_

#if (ARDUINO >= 100)
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif

#include "AsyncWait.h"
#include "globals.h"


class ZoneStatus {
  public:
    ZoneStatus(unsigned zoneIndex) : zoneIndex(zoneIndex) {
      zoneName = String("Zone-") + String(zoneIndex);
      //turnOff(); //DON'T CALL THIS HERE!
    }

    bool isZoneOn() const { return isOn; }

    //---------------------------------
    // Return:
    //  true  if this Zone Status changed,
    //  false if nothing changed.
    bool Loop(MilliSec currentMilliSec) {
      if (shouldBeTurnedOff(currentMilliSec)) {
        DEBUG_LOG("Loop currentMilliSec=");
        DEBUG_LOG(currentMilliSec);
        DEBUG_LOG(", ");
        DEBUG_LOGLN(getStatus());
        DEBUG_LOGLN(zoneName + " should be turned off.");
        return turnOff();
      }
      return false; // Nothing changed.
    }


    //---------------------------------
    // 'duration' is in seconds.
    //
    // Return:
    //  true  if this Zone Status changed,
    //  false if nothing changed.
    bool turnOn(unsigned duration, MilliSec currentMilliSec) {
      if (duration == 0) {
        // Do nothing.
        return false;
      }

      //TODO: do we need this line as a paranoid safety?
      //waitToTurnOff.cancel();

      bool oldValue = isOn;

      // Turn off the zone after the given duration.
      // ...convert seconds to milliseconds.
      waitToTurnOff.startWaiting(currentMilliSec, duration * 1000);
      isOn = true;

      if (oldValue != isOn) {
        DEBUG_LOG(zoneName);
        DEBUG_LOG(" TURN ON for ");
        DEBUG_LOG(duration);
        DEBUG_LOGLN(" seconds.");
        return true;
      }
      return false;
    }


    //---------------------------------
    // Return:
    //  true  if this Zone Status changed,
    //  false if nothing changed.
    bool turnOff() {
      bool oldValue = isOn;
      isOn = false;

      if (oldValue != isOn) {
        DEBUG_LOGLN(zoneName + " TURN OFF.");
        //TODO: determine if this line is needed?
        //waitToTurnOff.cancel();
        return true;
      }
      return false;
    }


    //---------------------------------
    String getStatus() const {
      String statusStr(zoneName);
      if (isOn) {
        statusStr += " on";
      } else {
        statusStr += " off";
      }
      return statusStr;
    }

  protected:
    const unsigned zoneIndex;

  private:
    String zoneName;
    bool isOn = false;
    AsyncWait waitToTurnOff;

    bool shouldBeTurnedOff(MilliSec currentMilliSec) {
      if (!isOn) {
        // The Zone is already off. Leave it unchanged.
        return false;
      }

      // The zone is currently on.
      if (waitToTurnOff.isWaiting(currentMilliSec)) {
        // The Zone is On and is still waiting to be turned off.
        // Leave it unchanged.
        return false;
      }

      // The Zone is On and has finished waiting.
      // It should now be turned off.
      waitToTurnOff.cancel();
      return true;
    }

    //void publishTurnedOn() {
    //}

    //void publishTurnedOff() {
    //}
};


class Zones {
  public:
    Zones(ZoneStatus *zones, unsigned numberOfZones)
      : zones(zones), numberOfZones(numberOfZones)
    { }

    void Setup() {
    }


    //---------------------------------
    // Return:
    //  true  if a Zone Status changed,
    //  false if nothing changed.
    bool Loop(MilliSec currentMilliSec) {
      bool changed = false;
      for (unsigned loopIndex = 0; loopIndex < numberOfZones; ++loopIndex) {
        // Warning: "||" is a short-circuit operator - do not reverse the arguments.
        changed = zones[loopIndex].Loop(currentMilliSec) || changed;
      }
      return changed;
    }

    uint8_t asBitMap8() {
      uint8_t result = 0;
      for (unsigned loopIndex = 0; loopIndex < numberOfZones; ++loopIndex) {
        if (zones[loopIndex].isZoneOn()) {
          result |= (1 << loopIndex);
        }
      }
      return result;
    }

    //---------------------------------
    // 'duration' is in seconds.
    //
    // Return:
    //  true  if a Zone Status changed,
    //  false if nothing changed.
    bool turnOn(unsigned zoneIndex, unsigned duration, MilliSec currentMilliSec, bool turnOffAllOthers = true) {
      DEBUG_LOG("Zones.turnOn()");
      DEBUG_LOG("  zoneIndex=");
      DEBUG_LOG(zoneIndex);
      DEBUG_LOG(", duration=");
      DEBUG_LOG(duration);
      DEBUG_LOG(", currentMilliSec=");
      DEBUG_LOG(currentMilliSec);
      DEBUG_LOGLN("");
      if (duration == 0 || !isValidZoneIndex(zoneIndex)) {
        return false;
      }

      bool changed = false;

      if (turnOffAllOthers) {
        //DEBUG_LOGLN("turnOffAllOthers - begin.");
        for (unsigned loopIndex = 0; loopIndex < numberOfZones; ++loopIndex) {
          if (loopIndex != zoneIndex) {
            // Warning: "||" is a short-circuit operator - do not reverse the arguments.
            changed = getZone(loopIndex).turnOff() || changed;
          }
        }
        //DEBUG_LOGLN("turnOffAllOthers - end.");
      }

      // Warning: "||" is a short-circuit operator - do not reverse the arguments.
      changed = getZone(zoneIndex).turnOn(duration, currentMilliSec) || changed;

      return changed;
    }


    //---------------------------------
    // Return:
    //  true  if a Zone Status changed,
    //  false if nothing changed.
    bool turnOff(unsigned zoneIndex) {
      if (!isValidZoneIndex(zoneIndex)) {
        return false;
      }
      return getZone(zoneIndex).turnOff();
    }

  private:
    const unsigned numberOfZones;
    ZoneStatus *zones;
    /**
    static const unsigned numberOfZones = NUM_OF_ZONES;
    ZoneStatus zones[numberOfZones] {
      //TODO: implement the following with meta-programming.
      ZoneStatus(0), ZoneStatus(1), ZoneStatus(2), ZoneStatus(3),
      ZoneStatus(4), ZoneStatus(5), ZoneStatus(6), ZoneStatus(7)
    };
    **/

    bool isValidZoneIndex(unsigned zoneIndex) {
      //TODO: log a warning if invalid.
      return (zoneIndex < numberOfZones);
    }

    // Assume zoneIndex has been tested for validity before being called.
    ZoneStatus & getZone(unsigned zoneIndex) { return zones[zoneIndex]; }
};


#endif /* ZONES_H_ */
