/*
 * asipServos.h -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */


#ifndef asipServo_h
#define asipServo_h

#if defined (UNO_WIFI_REV2_328MODE)
    #warning("servos not yet supported on Uno Wifi Rev 2 board")
#else
  #if defined (ARDUINO_ARCH_ESP32)
    #include <ESP32Servo.h>
  #else  
    #include <Servo.h> 
  #endif
    #include "asip.h"    
#endif

// Service and method defines
// Service IDs must be unique across all services
// Method and event IDs must be unique within a service

//IDs already used by the core mirtle services:  I,M,E,B,R

// Servo service
const char id_SERVO_SERVICE = 'S';
// methods
const char tag_SERVO_WRITE = 'W';

   
class asipServoClass : public asipServiceClass
{  
public:
   asipServoClass(const char svcId, const char evtId);
   void begin(byte nbrElements, const pinArray_t pins[], Servo* servos);
   void reset();
   void reportValues(Stream * stream);
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device   
   void processRequestMsg(Stream *stream); 
   //void reportName(Stream *stream);   
private: 
   void write(byte servoId, byte angle);
   Servo *myServoPtr;
   void remapPins(Stream *stream);
};   

extern asipServoClass asipServos;

#endif
 
   


