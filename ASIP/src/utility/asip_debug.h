/*
 asip_debug.h 
 
 Debug printing is enabled by defining ASIP_DEBUG in this file and
 using debug_printf statements in your code 
 More extensive debug is enabled by also defining ASIP_VERBOSE_DEBUG and 
 using verbose_printf statements and defining ASIP_VERBOSE_DEBUG in this file.

debug_printf and verbose_printf use standard printf arguments with syntax 
dependant on the compiler used.
By default, line length is limited to 80 chars on 8 bit MCUs
 
 Output can be sent to the same stream as ASIP, on another stream that can be hardware or 
 software serial.
  to use software Serial, define ASIP_DEBUG_SOFT_SERIAL and set the pin in asip.cpp using the txPin define.
 Baud rate is set with the ASIP_DEBUG_BAUD, 57600 is the highest rate reliably supported by 8MHz boards
*/


#ifndef asip_debug_h
#define asip_debug_h

#include <inttypes.h>
#include <Stream.h>
#include <asip.h>

extern  Stream *debugStream;   // debug output

const char DEBUG_MSG_INDICATOR = '!';  // debug text within info messages are preceded with this tag

//#define ASIP_DEBUG
#define debugStream  (&Serial)
//#define ASIP_VERBOSE_DEBUG
#define ASIP_DEBUG_BAUD 57600  // 57600 is highest rate for 8MHz boards 
#define ASIP_DEBUG_BUFFER_SIZE 140 // max length of a debug string

#ifdef ASIP_DEBUG
//#define ASIP_DEBUG_SOFT_SERIAL

#if defined(HAS_SERIAL_PRINTF) // defined in boards.h
#define debug_printf                    \
    debugStream->write(DEBUG_MSG_INDICATOR); \
    debugStream->printf
#else  // 8 bit MCUs assume no built-in  printf support
extern char _buf[];  // for printf
#define debug_printf(...)                         \
    debugStream->write(DEBUG_MSG_INDICATOR); \
    do {                            \
        sprintf(_buf, __VA_ARGS__); debugStream->print(_buf); \
    } while (0) 
#endif // check for serial printf support 
#else // #ASIP_DEBUG not defined
#define debug_printf(...) 
#endif // PRINTF_DEBUG

#if defined (ASIP_VERBOSE_DEBUG) && defined(ASIP_DEBUG)
#define verbose_printf debug_printf 
#else 
#define verbose_printf(...)
#endif

/******************************************************************************
* Definitions
******************************************************************************/

#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif


#endif  // SendOnlySoftwareSerial_h
