/*
 * Pin definitions services that require constant values declared at compile time
 * all other pin definitions are included from mirtohardware.h in the sketch directory 
 */

#pragma once


//#define MIRTO_BOARD_DETECT
//#define MIRTO2018 
// #define GREEN_PROTO_PCB // only define this if using the green prototype PCB
#define MIRTO2016 

#if defined(__MK20DX256__) // Teensy 3.x
    const uint8_t encoderPins[] = {29,31,26,28}; // Left:A,B  Right:A,B
#elif defined(UNO_WIFI_REV2_328MODE) || (ARDUINO_SAMD_ZERO) // zero uses same pins as uno
    const uint8_t encoderPins[] = {3,7, 2,4};
#elif defined (TARGET_RP2040) || defined(TARGET_RASPBERRY_PI_PICO)
    #ifdef GREEN_PROTO_PCB   
        const uint8_t encoderPins[] = {21,22,8,9}; // use this for green proto PCB
    #else
        const uint8_t encoderPins[] = {0,1,4,5};  // use this for black production PCB
    #endif    
#elif defined (ARDUINO_ARCH_ESP32)   
     pinArray_t encoderPins[] = {NO_PIN,NO_PIN,NO_PIN,NO_PIN}; // NO ENCODERS   
#else     
    #error("Hardware not suppported")
#endif
// Separate arrays are a requirement of the third party encoder library 
#define encoderLeftPins  (&encoderPins[0])
#define encoderRightPins (&encoderPins[2])

