/*
 * asipDistance.cpp -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version. 
 *
 * Nov 2016- added begin option supporting seperate triger and echo pins 
 */
 
 
#include "asipDistance.h"


asipDistanceClass::asipDistanceClass(const char svcId) : asipServiceClass(svcId)
{
   svcName = PSTR("Distance");
   i2cBus = NULL;
}

// each sensor uses 1 pin
void asipDistanceClass::begin(byte nbrElements, const pinArray_t pins[])
{
  SeperateTrigEchoPins = false;
  asipServiceClass::begin(nbrElements,pins);
}

// each sensor uses 2 pins
void asipDistanceClass::begin(byte nbrElements, const byte nbrPins, const pinArray_t pins[])
{
  if(nbrPins == 1) {
    SeperateTrigEchoPins = false;
  }  
  else {
     SeperateTrigEchoPins = true;
  }
  asipServiceClass::begin(nbrElements,nbrPins,pins);
}

// sensor uses I2C on given pins at given I2C address
// only one I2C sensor is supported in this version (nbrElements should be 1)
// to support more than 1 I2C sensor, code needed to accept array of sensor addresses
// pinArray[0] is the SDA pin, pinarray[1] is SCL
void asipDistanceClass::begin(byte nbrElements, const byte nbrPins, const pinArray_t pins[], TwoWire &I2CBus, const byte addr )
{
  //Serial.printf("nbr pins=%d, sda=%d, scl=%d, addr = %x\n", nbrPins,pins[0], pins[1], addr);
 
  i2cBus = &I2CBus;
  i2cAddr = addr;
 
  asipServiceClass::begin(nbrElements,nbrPins,pins);
#if defined (TARGET_RP2040) or defined (ARDUINO_ARCH_ESP32)
  i2cBus->setSDA(pins[0]);
  i2cBus->setSCL(pins[1]);
#endif
  i2cBus->begin();
  delay(20);
}

// this function rewrites the pins used by this service   
void asipDistanceClass::remapPins(Stream *stream)
{

   int count = stream->parseInt();
   if( count == pinCount && count <= nbrElements) {
       for(int i=0; i < count; i++){
       // todo 
       }          
   }
   else {
     // error message
   }
}

/*
void asipDistanceClass::reportName(Stream * stream)
{
  stream->println(F(ASIP_SERVICE_NAME));
}
*/

 void asipDistanceClass::reset()
 {
  
 }
 
 void asipDistanceClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device
{
  if( sequenceId < nbrElements) {
       stream->print(getDistance(sequenceId));
  }
}

void asipDistanceClass::processRequestMsg(Stream *stream)
{
   int request = stream->read();
   if( request == tag_AUTOEVENT_REQUEST) {
      setAutoreport(stream);
   }
   else if(request == tag_DISTANCE_MEASURE){ 
      reportValues(stream);  // send a single measurement
   }
      
   else {
     reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
   }
}

int asipDistanceClass::getDistance(int sequenceId)
{
    if(i2cBus != NULL) {
        return readI2CSensor(sequenceId);
    }
    else{
        return readPulsedSensor(sequenceId);
    } 
    else
        return 0;
}

int asipDistanceClass::readPulsedSensor(int sequenceId)
{
  const long MAX_DISTANCE = 100;  
  const long MAX_DURATION =   (MAX_DISTANCE * 58);

  // The sensor is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
   byte trigPin, echoPin;
  if(SeperateTrigEchoPins){
     int index = 2*sequenceId;
     trigPin = pins[index];  
     echoPin = pins[index+1];  
  }
  else {
    trigPin = echoPin = pins[sequenceId];    
  }
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(4);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  pinMode(echoPin, INPUT); 
  
  // limit pulseIn duration to a max of 275cm (just under 16ms) 
  // if pulse does not arrive in this time then ping sensor may not be connected
  // if you need to increase this then you must change the distanceSensorDataRequest message body size
  long duration = pulseIn(echoPin, HIGH, MAX_DURATION); 
  // convert the time into a distance
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  int cm = (duration / 29) / 2;
  return cm;    
}

int asipDistanceClass::readI2CSensor(int sequenceId)
{
  // returns distance in cm or -1 if no sensor
  i2cBus->beginTransmission(i2cAddr);
  i2cBus->write(1);
  uint8_t error = i2cBus->endTransmission(true);
  delay(15);
  //Read 3 bytes from the slave
  size_t BYTES_TO_READ = 3;
  uint8_t bytesReceived = i2cBus->requestFrom(i2cAddr, BYTES_TO_READ);
  if (bytesReceived == BYTES_TO_READ) {  //If received request nbr bytes
    uint8_t temp[BYTES_TO_READ];
    i2cBus->readBytes(temp, BYTES_TO_READ);
    uint32_t val = temp[2] + 256 * temp[1] + 256 * 256 * temp[0];
    return (val / 1000);
  }

  else {
    return -1;
  } 
}