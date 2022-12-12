/*
 * Pin allocations for mirto 2016 and 2018 boards
 *    for HUBee boards, include  robot_pins_hubee.h instead of this header
 * The order of the following pins is service specific, see the service definition for details
 */
#ifndef robot_pins_h
#define robot_pins_h

//#define MIRTO_BOARD_DETECT
//#define MIRTO2018 
//#define MIRTO2016 

#if defined(__MK20DX256__) // Teensy 3.x
  #if defined MIRTO_BOARD_DETECT  // auto detect 2016 and 2018 boards
    // following pins may be reconfigured in setup
    // motor pins are: In1, In2, PWM, encoderA, encoderB
    const byte m18switchPin          = 33; 
    byte m18MotorPins[]        = {32,255,20, 9,255,25};  // pins used on Mirto 2018 board
    const byte m18TonePin            = 10;

    const byte m16switchPin          = 5; 
    byte m16MotorPins[]        = {33,20,32, 27,30,25};  // pins used on Mirto 2016 board    
    const byte m16TonePin            = 10;
  #elif defined MIRTO2018    // pcb with DRV8833 H-Bridge
    byte motorPins[]   = {32, 255,20,9,255,25};  // L-in1,no pin,L-PWM, R-in1,no pin,R-PWM
    const byte _tonePin = 10;
    #elif defined MIRTO2016    // pcb with 6612FNG H-Bridge
    byte motorPins[]   = {33,20,32,27,30,25};  // L-in1,L-in2,L-PWM, R-in1,R-in2,R-PWM
    const byte _tonePin = 9;
  #endif
    const byte encoderPins[] = {29,31,26,28}; // Left:A,B  Right:A,B
    const byte bumpPins[] = {6,24};
    const byte irReflectancePins[] = {A0,A1,A2,A3}; // first is control, the remainder are used as analog inputs
    const byte servoPins[]    = {3};
    const byte distancePins[] = {4};
    // the following pins use preprocessor defines to enable conditional compile
    #define neoPixelPin 2
    #define ledPin     13
#else
    #error("this code requires Teensy 3.x")
#endif
// Seperate arrays are a requirement of the third party encoder library 
//#define leftMotorPins    (&motorPins[0])
//#define rightMotorPins   (&motorPins[3])
#define encoderLeftPins  (&encoderPins[0])
#define encoderRightPins (&encoderPins[2])

#endif
