/*
 * Pin allocations for mirto using Uno WIfi Rev2 shield
 * The order of the following pins is service specific, see the service definition for details
 */
#ifndef robot_pins_h
#define robot_pins_h

#if defined (UNO_WIFI_REV2_328MODE)
    // pin defines for DRV8833 boards ('255' elements will be set PWM pin based on motor direction)
    const byte motorPins[]   = {6,9,255, 10,5,255};  // L-in1,L-in2, pwm,  R-in1,R-in2, pwm
    const byte _tonePin = 13;
    const byte encoderPins[] = {7,2,3,4}; // Left:A,B  Right:A,B
    const byte bumpPins[] = {12,11};
    const byte switchPin     = 11;   // switch connected to right bump pin
    const byte irReflectancePins[] = {8,A2,A3,A1}; // first is control, the remainder are used as analog inputs
    const byte potPin = A0;
    const byte servoPins[]    = {255};
    const byte distancePins[] = {A4,A5};
    // the following pins use preprocessor defines to enable conditional compile
    #define neoPixelPin 1
    #define ledPin     LED_BUILTIN
#else
    #error("this code requires Uno WiFi Rev2")
#endif

#define encoderLeftPins  (&encoderPins[0])
#define encoderRightPins (&encoderPins[2])

#endif

