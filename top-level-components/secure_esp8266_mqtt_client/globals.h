/*
 * globals.h
 *
 *  Created on: 2018-12-30
 *      Author: Warren Taylor
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
