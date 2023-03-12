/*
 * asipLidar.cpp -  Lidar implimentation for Arduino Services Interface Protocol (ASIP)
 * 
 * This services uses a dedicated serial port with hardware UART and requires a 32 bit board 
 *
 * Copyright (C) 2023 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#pragma once
#include "asip.h"


// Service and method defines
// Service IDs must be unique across all services
// Method and event IDs must be unique within a service

//IDs in use by mirtle services:  I(I/O),M(Motor),E(Encoder),B(Bump sensor),R(IR line sensors)
// L(LCD) T(Tone) P(neoPixels) D(distance),S(servo) A(Accelerometer), G(Gyroscope), H(heading)
// ID used:  ABDEGHILMPRST

// Lidar service
const char id_LIDAR_SERVICE = 'N';
// methods
// enable auto events - use system define, tag_AUTOEVENT_REQUEST ('A') to request autoevents
// events use system tag: SERVICE_EVENT  ('e')


class asipLidarClass : public asipServiceClass
{  
public:

   asipLidarClass(const char svcId);  
   void begin( HardwareSerial *lidarSerial, const pinArray_t pins[]);
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device
   void reportValues(Stream *stream);
   void processRequestMsg(Stream *stream);
   void reset();
   void service(); // lidar has a seprate service method
   void setStream(Stream *stream);
private:  

 };   

   


