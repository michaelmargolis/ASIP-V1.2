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
#include "asipLidar.h"
#include "ld06Lidar.h"

#define ASIP_SERVICE_NAME  "LIDAR"

LD06 ld06;

static PROGMEM const prog_char lidarName[]     =  "LIDAR";

asipLidarClass::asipLidarClass(const char svcId) : asipServiceClass(svcId){svcName = lidarName;}

void asipLidarClass::begin( HardwareSerial *lidarSerial, const pinArray_t pins[])
{
  asipServiceClass::begin(1,2,pins); // one lidar using two pins
  ld06.begin(lidarSerial,pins[0], pins[1], &Serial);
}

void asipLidarClass::reportValue(int sequenceId, Stream * stream)
{
   // values are directly reported from ld06 instance
}

void asipLidarClass::reportValues(Stream *stream)
{
  
}

void asipLidarClass::processRequestMsg(Stream *stream)
{
   int request = stream->read();
   if(request == tag_AUTOEVENT_REQUEST) {
     int val = stream->parseInt();
     if(val < 1) {
         ld06.outputEnabled = false;
     }
     else{
       ld06.outputEnabled = true;
     }
   }
   else {
     reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
   }
}

void asipLidarClass::reset()
{
    
}


void asipLidarClass::service()
{
  ld06.service();
}

void asipLidarClass::setStream(Stream *stream)
{
     ld06.setOutStream(stream);
}