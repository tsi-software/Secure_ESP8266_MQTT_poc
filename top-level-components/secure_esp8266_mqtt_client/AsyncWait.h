/*
 * AsyncWait.h
 *
 * Author:  Warren Taylor
 * Created: 2018-12-30
 * Copyright (c) 2018-2019 Warren Taylor.  All right reserved.
 */

#ifndef ASYNC_WAIT_H_
#define ASYNC_WAIT_H_

#if (ARDUINO >= 100)
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif

#include <limits.h>
#include "globals.h"


using MilliSec = unsigned long;


class AsyncWait {
  public:
    AsyncWait() {
      reset();
    }

    void startWaiting(MilliSec currentMilliSec, MilliSec waitDuration) {
      waitStartMillisec = currentMilliSec;
      this->waitDuration = waitDuration;
      isAsyncWaiting = true;
    }

    bool isWaiting(MilliSec currentMilliSec) {
      if (!isAsyncWaiting) {
        return false;
      }

      MilliSec delta = calculateMilliSecDelta(currentMilliSec);
      if (delta >= waitDuration) {
        // We have finished waiting for the given duration.
        reset();
        return false;
      }

      // Still waiting.
      return true;
    }

    void cancel() {
      reset();
    }

  private:
    bool isAsyncWaiting;
    MilliSec waitStartMillisec;
    MilliSec waitDuration;

    void reset() {
      isAsyncWaiting = false;
      waitStartMillisec = 0;
      waitDuration = 0;
    }
    
    /**
     * Calculate the difference between currentMilliSec and waitStartMillisec
     * taking into account the event when 'millis()' wraps around.
     */
    MilliSec calculateMilliSecDelta(MilliSec currentMilliSec)
    {
        if (currentMilliSec < waitStartMillisec) {
            // Handle the wrap-around of millis().
            return ULONG_MAX - waitStartMillisec + currentMilliSec;
        } else {
            return currentMilliSec - waitStartMillisec;
        }
    }
};


#endif /* ASYNC_WAIT_H_ */
