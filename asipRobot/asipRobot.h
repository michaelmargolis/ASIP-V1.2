/*
 * robot.h -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * distinct encoder class removed in ASIPv1.2, encoders are supported within motor class
 */

#ifndef robot_h
#define robot_h

#define ASIP_PID  // comment this to disable PID code

#include "asip.h"
#include "RobotMotor.h"  // for H-bridge enums

#ifdef NOT_MOVED_TO_SKETCH  // include the appropriate one if not defined in sketch folder
#if defined (UNO_WIFI_REV2_328MODE) || defined (ARDUINO_SAMD_ZERO) || defined(ARDUINO_UNOWIFIR4) // all use same shield
  #include "UnoWifiRobot_pins.h"  
#elif defined(__MK20DX256__) // Teensy 3.x
  #include "mirto2020Pins.h"
  // #include "TeensyRobot_pins.h"  //uncomment this for 2016 and 2018 boards
#elif defined(TARGET_RP2040) || defined(TARGET_RASPBERRY_PI_PICO) 
  #include "PiPico2040Robot_pins.h"
#else
  #error "this code requires a teensy, Arduino Uno Wifi Rev2 or pico2040"
#endif
#endif

// Service and method defines
// Service IDs must be unique across all services
// Method and event IDs must be unique within a service

// Motor service
const char id_MOTOR_SERVICE = 'M';
// Motor methods (messages to Arduino)
const char tag_SET_MOTOR            = 'm'; // sets motor power  
const char tag_SET_MOTORS           = 'M';
const char tag_SET_MOTOR_RPM        = 'r'; // wheel rpm
const char tag_SET_MOTORS_RPM       = 'R'; // both wheels rpm 
const char tag_SET_ROBOT_SPEED_CM   = 'c'; // speed in Cm per Sec using PID
const char tag_ROTATE_ROBOT_ANGLE   = 'a'; // Robot rotation using given degrees per second and angle 
const char tag_STOP_MOTOR           = 's';  
const char tag_STOP_MOTORS          = 'S';
const char tag_RESET_ENCODERS       = 'E'; // rest total counts to zero


typedef struct {
        int32_t delta;
        int32_t pos;
        int32_t prevPos; // previous encoder reading 
} Encoder_state_t;



// Encoder service
const char id_ENCODER_SERVICE = 'E';
// Encoder methods - use system define, tag_AUTOEVENT_REQUEST ('A') to request autoevents
// Encoder events -  events use system tag: tag_SERVICE_EVENT  ('e')


// Bump detect service
const char id_BUMP_SERVICE = 'B';
// Bump sensor methods - use system define, tag_AUTOEVENT_REQUEST ('A') to request autoevents
// Bump Sensor events -  events use system tag: tag_SERVICE_EVENT  ('e')


// IR Line detect service
const char id_IR_REFLECTANCE_SERVICE = 'R';
// IR Line detect methods - use system define, tag_AUTOEVENT_REQUEST ('A') to request autoevents
// IR Line detect events -  events use system tag: tag_SERVICE_EVENT  ('e')


const int NBR_WHEELS = 2;  // defines the number of wheels (and encoders), note not tested with values other than 2

class robotMotorClass : public asipServiceClass
{  
public:

   robotMotorClass(const char svcId, const char evtId);  
   void begin(byte nbrElements, byte pinCount, pinArray_t pins[]);
   void begin(byte nbrElements, byte motorPinCount, pinArray_t pins[], byte encoderPinCount, const pinArray_t encoderPins[]);
   void setHbridgeType(int type); // enum indicating h-bridge
   int boardDetect();  // enum indicating board
   void reset();
   void refreshEncoderCache(int side);
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device
   void reportValues(Stream *stream);   
   void setMotorPower(byte motor, int power);
   void setMotorPowers(int power0, int power1);
#ifdef ASIP_PID 
   void setMotorRPM(byte motor, int rpm, long duration);
   void setMotorsRPM(int rpm0, int rpm1, long duration);
   void setRobotSpeedCmPerSec(int cmps, long duration);  
   void rotateRobot( int dps, int angle);
#endif   
   void stopMotor(byte motor);
   void stopMotors();
   void resetEncoderTotals();
   void processRequestMsg(Stream *stream);
 //  void reportName(Stream *stream);
 private:
   Encoder_state_t encoder_state[NBR_WHEELS];
 };
   

class bumpSensorClass : public asipServiceClass
{  
public:
   bumpSensorClass(const char svcId);
   void begin(byte nbrElements, byte pinCount, const pinArray_t pins[]);
   void reset();
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device
   void processRequestMsg(Stream *stream);
  // void reportName(Stream *stream);
};

class irLineSensorClass : public asipServiceClass
{  
public:
   irLineSensorClass(const char svcId);
   void begin(byte nbrElements, byte pinCount, const pinArray_t pins[]);
   void reset();
   int16_t getValue(int sequenceId);
   void reportValues(Stream *stream);
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device
   void processRequestMsg(Stream *stream);
  // void reportName(Stream *stream);
};    


#if defined (UNO_WIFI_REV2_328MODE)
// Accelerometer service
////#include "IMU_UnoR2.h"
#warning ("todo fix import of IMU_UnoR2.h"); 
#include "SPI.h"

static PROGMEM const prog_char accelName[]    =  "Accelerometer";

const char id_ACCELEROMETER_SERVICE = 'A';
// methods
const char  tag_ACCELEROMETER_REQUEST = 'R';   // enable auto events
const char  tag_ACCELEROMETER_MEASURE = 'M';   // measure and send a single event 


const int NBR_GYRO_AXIS  = 3;
const int NBR_ACCEL_AXIS = 3;

class AccelerometerClass : public asipServiceClass
{  
public:

   AccelerometerClass(const char svcId);  
   void begin(byte nbrElements);
   void begin(byte nbrElements,serviceBeginCallback_t serviceBeginCallback);   // classes that use I2C instead of specific pins use this begin method
   void reportValue(int sequenceId, Stream * stream) ; // send the value of the given device
   void reportValues(Stream *stream); // send all values separated by commas, preceded by header and terminated with newline
   void processRequestMsg(Stream *stream);
   void reset();
private:
   int readAxis(int sequenceId);
   int16_t axis[NBR_ACCEL_AXIS];
};

#endif

#endif
 
   


