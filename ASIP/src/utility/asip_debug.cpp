/*
 asip_debug.cpp 
 
 Debug printing is enabled by defining ASIP_DEBUG in debug.h file,
 comment that define to disable debugging.
 
 Output is sent to a stream using software serial, the pin is set in asip.cpp using the txPin define.
 Baud rate is set with the ASIP_DEBUG_BAUD, 57600 is the highest rate reliably supported by 8MHz boards
 printf is used to format print statements, the default max line length is 80 characters.

*/
 
 
#include <SoftwareSerial.h>
#include "asip_debug.h"

#ifndef ARDUINO_ARCH_ESP32
#include "Arduino.h"
#include <avr/pgmspace.h>
#endif

// Buffer for printf
#if defined(ASIP_DEBUG) && !defined(__MK20DX256__) 
char _buf[ASIP_DEBUG_BUFFER_SIZE];   // increase buffer size for non-teensy if strings will be 80 chars or more
#endif
