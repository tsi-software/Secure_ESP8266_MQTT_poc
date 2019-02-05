/*
 * globals.h
 *
 * Author:  Warren Taylor
 * Created: 2018-12-30
 * Copyright (c) 2018-2019 Warren Taylor.  All right reserved.
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_


#ifndef DEBUG
#define DEBUG
#endif


#ifdef DEBUG
#  define DEBUG_LOG(msg)     Serial.print(msg)
#  define DEBUG_LOGF(fmt,p1) Serial.printf((fmt),(p1))
#  define DEBUG_LOGLN(msg)   Serial.println(msg)
#else
#  define DEBUG_LOG(msg)
#  define DEBUG_LOGF(fmt,p1)
#  define DEBUG_LOGLN(msg)
#endif


#endif /* GLOBALS_H_ */
