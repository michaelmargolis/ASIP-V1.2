/*
 * Boards.h -  Arduino Services Interface Protocol (ASIP)
 *  Pin information derived from pins_arduino.h 
 * 
 * Copyright (C) 2014 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

/* 
 * added Pico 2040 support 12 Dec 2022
 */

#ifndef asip_boards_h
#define asip_boards_h

/* macros providing pin information */

// TODO see if this can be gleaned from the distributed pins_arduino.h
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
#define TOTAL_PINCOUNT           20 
#define TOTAL_ANALOG_PINS       6
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINCOUNT)
#define IS_PIN_ANALOG(P)        ((P) >= 14 && (P) < 14 + NUM_ANALOG_INPUTS)
#define PIN_TO_ANALOG(P)        (P-14)
#define ANALOG_PIN_TO_DIGITAL   (P+14)
#define SERIAL_RX_PIN            0
#define SERIAL_TX_PIN            1
#define DIGITAL_PIN_TO_PORT(p)   digitalPinToPort(p)
#define DIGITAL_PIN_TO_MASK(p)   digitalPinToBitMask(p)
#define ARDUINO_PINOUT_OPTIMIZE

#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) 
#define TOTAL_PINCOUNT           70 
#define TOTAL_ANALOG_PINS       16
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINCOUNT)
#define IS_PIN_ANALOG(p)        ((p) >= 54 && (p) < TOTAL_PINCOUNT)
#define PIN_TO_ANALOG(P)        (P-54)
#define ANALOG_PIN_TO_DIGITAL   (P+54)
#define SERIAL_RX_PIN            0
#define SERIAL_TX_PIN            1
#define SERIAL1_RX_PIN           19
#define SERIAL1_TX_PIN           18
#define SERIAL2_RX_PIN           17
#define SERIAL2_TX_PIN           16
#define SERIAL3_RX_PIN           15
#define SERIAL3_TX_PIN           14
#define DIGITAL_PIN_TO_PORT(p)   digitalPinToPort(p)
#define DIGITAL_PIN_TO_MASK(p)   digitalPinToBitMask(p)
#define ARDUINO_PINOUT_OPTIMIZE


#elif defined(__AVR_ATmega32U4__)
#define TOTAL_PINCOUNT          30
#define TOTAL_ANALOG_PINS       12 
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINCOUNT)
#define IS_PIN_ANALOG(p)        ((p) >= 18 && (p) < TOTAL_PINCOUNT)
#define PIN_TO_ANALOG(P)        (P-18)
#define ANALOG_PIN_TO_DIGITAL   (P+18)
#define SERIAL_RX_PIN            -1  //negative pin numbers if comms is over USB
#define SERIAL_TX_PIN            -1
#define SERIAL1_RX_PIN            0
#define SERIAL1_TX_PIN            1
#define DIGITAL_PIN_TO_PORT(p)   digitalPinToPort(p)
#define DIGITAL_PIN_TO_MASK(p)   digitalPinToBitMask(p)
#define ARDUINO_PINOUT_OPTIMIZE

#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
#define TOTAL_PINCOUNT           32 
#define TOTAL_ANALOG_PINS       8
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINCOUNT)
#define IS_PIN_ANALOG(p)        ((p) >= 24 && (p) < TOTAL_PINCOUNT)
#define PIN_TO_ANALOG(P)        (31-P)
#define ANALOG_PIN_TO_DIGITAL   (31-P)
#define SERIAL_RX_PIN            8
#define SERIAL_TX_PIN            9
#define SERIAL1_RX_PIN           10
#define SERIAL1_TX_PIN           11
#define DIGITAL_PIN_TO_PORT(p)   digitalPinToPort(p)
#define DIGITAL_PIN_TO_MASK(p)   digitalPinToBitMask(p)
#define ARDUINO_PINOUT_OPTIMIZE


// Teensy 2.0
#elif defined(__AVR_ATmega32U4__) && defined(CORE_TEENSY)
#define TOTAL_PINCOUNT          25 // 11 digital + 12 analog
#define TOTAL_ANALOG_PINS       12
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 11 && (p) <= 22)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         IS_PIN_DIGITAL(p)
#define IS_PIN_I2C(p)           ((p) == 5 || (p) == 6)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        (((p)<22)?21-(p):11)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)


// Teensy 3.0 and 3.1
#elif defined(__MK20DX128__) || defined(__MK20DX256__)
#define TOTAL_PINCOUNT          38 // 24 digital + 10 analog-digital + 4 analog
#define TOTAL_ANALOG_PINS       14
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) <= 33)
// note pins 35-38 are analog only pins
#define IS_PIN_ANALOG(p)        (((p) >= 14 && (p) <= 23) || ((p) >= 26 && (p) <= 28) || ((p) >= 35 && (p) <= 38))
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         IS_PIN_DIGITAL(p)
#define IS_PIN_I2C(p)           ((p) == 18 || (p) == 19)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        (((p)<=23)?(p)-14:(p)-24)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p) 
#define SERIAL_RX_PIN            -1  //negative pin numbers if comms is over USB
#define SERIAL_TX_PIN            -1 
#define SERIAL1_RX_PIN           0
#define SERIAL1_TX_PIN           1
#define SERIAL2_RX_PIN           9
#define SERIAL2_TX_PIN           10
#define SERIAL3_RX_PIN           7
#define SERIAL3_TX_PIN           8
#define DIGITAL_PIN_TO_PORT(p)   (p/8) 
#define DIGITAL_PIN_TO_MASK(p)   (1<<(p%8))     

// Teensy LC
#elif defined(__MKL26Z64__)
#define TOTAL_PINCOUNT          27 
#define TOTAL_ANALOG_PINS       13
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) <= 26)
#define IS_PIN_ANALOG(p)        ((p) >= 14 && (p) <= 23) 
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         IS_PIN_DIGITAL(p)
#define IS_PIN_I2C(p)           ((p) == 18 || (p) == 19)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        (((p)<=23)?(p)-14:(p)-24)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p) 
#define SERIAL_RX_PIN            -1  //negative pin numbers if primary comms is over USB
#define SERIAL_TX_PIN            -1 
#define SERIAL1_RX_PIN           0
#define SERIAL1_TX_PIN           1
#define DIGITAL_PIN_TO_PORT(p)   (p/8) 
#define DIGITAL_PIN_TO_MASK(p)   (1<<(p%8))     



// Teensy++ 1.0 and 2.0
#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
#define TOTAL_PINCOUNT          46 // 38 digital + 8 analog
#define TOTAL_ANALOG_PINS       8
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 38 && (p) < TOTAL_PINS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         IS_PIN_DIGITAL(p)
#define IS_PIN_I2C(p)           ((p) == 0 || (p) == 1)
#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 38)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)

#elif defined (__arm__) && defined (__SAM3X8E__) // Arduino Due compatible
#define TOTAL_PINCOUNT          65 
#define TOTAL_ANALOG_PINS       12
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINS)
#define IS_PIN_ANALOG(p)        ((p) >= 54 && (p) < TOTAL_PINS)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         IS_PIN_DIGITAL(p)
#define IS_PIN_I2C(p)           ((p) == 20 || (p) == 21)
//#define IS_PIN_SPI(p)           ((p) == SS || (p) == MOSI || (p) == MISO || (p) == SCK)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(p)        ((p) - 54)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)
#define SERIAL_RX_PIN           0
#define SERIAL_TX_PIN           1

// Uno WiFi Rev 2 using ATmega4809
#elif defined(UNO_WIFI_REV2_328MODE)
#define TOTAL_PINCOUNT          20 
#define TOTAL_ANALOG_PINS       5
#define IS_PIN_DIGITAL(p)       ((p) >= 0 && (p) < TOTAL_PINCOUNT)
#define IS_PIN_ANALOG(P)        ((P) >= 14 && (P) < 14 + NUM_ANALOG_INPUTS)
#define PIN_TO_DIGITAL(p)       (p)
#define PIN_TO_ANALOG(P)        (P-14)
#define ANALOG_PIN_TO_DIGITAL   (P+14)
#define SERIAL1_RX_PIN           0
#define SERIAL1_TX_PIN           1
#define SERIAL2_RX_PIN           255
#define SERIAL2_TX_PIN           255
#define DIGITAL_PIN_TO_PORT(p)   digitalPinToPort(p)
#define DIGITAL_PIN_TO_MASK(p)   digitalPinToBitMask(p)
#define SERIAL_RX_PIN           0
#define SERIAL_TX_PIN           1

#elif defined(TARGET_RP2040) || defined(TARGET_RASPBERRY_PI_PICO)
 #define TOTAL_PINCOUNT          28
 #define TOTAL_ANALOG_PINS       3
 #define VERSION_BLINK_PIN       LED_BUILTIN
 #define IS_PIN_DIGITAL(p)       (((p) >= 0 && (p) < 23) || (p) == LED_BUILTIN)
 #define IS_PIN_ANALOG(p)        ((p) >= 26 && (p) < 26 + TOTAL_ANALOG_PINS)
 #define IS_PIN_PWM(p)           IS_PIN_DIGITAL(p)
 #define IS_PIN_SERVO(p)         (IS_PIN_DIGITAL(p) && (p) != LED_BUILTIN)
 #define SERIAL_RX_PIN            -1  //negative pin numbers if primary comms is over USB
 #define SERIAL_TX_PIN            -1 
 // From the data sheet I2C-0 defaults to GP 4 (SDA) & 5 (SCL) (physical pins 6 & 7)
 // However, v2.3.1 of mbed_rp2040 defines WIRE_HOWMANY to 1 and uses the non-default GPs 6 & 7:
 //#define WIRE_HOWMANY	(1)
 //#define PIN_WIRE_SDA            (6u)
 //#define PIN_WIRE_SCL            (7u)
 #define IS_PIN_I2C(p)           ((p) == PIN_WIRE_SDA || (p) == PIN_WIRE_SCL)
 // SPI-0 defaults to GP 16 (RX / MISO), 17 (CSn), 18 (SCK) & 19 (TX / MOSI) (physical pins 21, 22, 24, 25)
 #define IS_PIN_SPI(p)           ((p) == PIN_SPI_SCK || (p) == PIN_SPI_MOSI || (p) == PIN_SPI_MISO || (p) == PIN_SPI_SS)
 // UART-0 defaults to GP 0 (TX) & 1 (RX)
 #define IS_PIN_SERIAL(p)        ((p) == 0 || (p) == 1 || (p) == 4 || (p) == 5 || (p) == 8 || (p) == 9 || (p) == 12 || (p) == 13 || (p) == 16 || (p) == 17)
 #define PIN_TO_DIGITAL(p)       (p)
 #define PIN_TO_ANALOG(p)        ((p) - 26)
 #define PIN_TO_PWM(p)           (p)
 #define PIN_TO_SERVO(p)         (p) 
 #define DIGITAL_PIN_TO_PORT(p)   (p/8) 
 #define DIGITAL_PIN_TO_MASK(p)   (1<<(p%8))     


// ESP32
// GPIO 6-11 are used for FLASH I/O, therefore they're unavailable here
#elif defined(ESP32)
#define TOTAL_PINCOUNT          34 // Input only pins 34-39 not used by asipio
#define TOTAL_ANALOG_PINS       16
#define TOTAL_PINS              TOTAL_PINCOUNT 
#define digitalPinHasSPI(p)     ((p) == 12 || (p) == 13 || (p) == 14 || (p) == 15)
#define PIN_SPI_MOSI            13
#define PIN_SPI_MISO            12
#define PIN_SPI_SCK             14
#define digitalPinHasSerial(p)  ((p) == 16 || (p) == 17 || (p) == 1 || (p) == 3)
// Pins 1 and 3 are used for the USB Serial communication. If we enable them here, the initial pin reset causes the serial communication
// to not work after boot. 
#define IS_PIN_DIGITAL(p)       ((p) == 0 || (p) == 2 || (p) == 4 || (p) == 5 || ((p) >= 12 && (p) < 24) || ((p) >= 25 && (p) < 28) || ((p) >= 32 && (p) <= 39))
#define IS_PIN_ANALOG(p)        ((p) == 0 || (p) == 2 || (p) == 4 || (p) == 5 || ((p) >= 12 && (p) < 16) || ((p >= 25 && (p) < 28) || ((p) >= 32 && (p) < 37) || (p) == 39))
#define IS_PIN_PWM(p)           (IS_PIN_DIGITAL(p))
#define IS_PIN_SERVO(p)         IS_PIN_DIGITAL(p)
#define IS_PIN_I2C(p)           ((p == 21) || (p == 22))
#define IS_PIN_SPI(p)           (IS_PIN_DIGITAL(p) && digitalPinHasSPI(p))
#define IS_PIN_INTERRUPT(p)     (digitalPinToInterrupt(p) > NOT_AN_INTERRUPT)
#define IS_PIN_SERIAL(p)        (digitalPinHasSerial(p))
#define PIN_TO_DIGITAL(p)       (p)
#define DIGITAL_PIN_TO_PORT(p)   (p/8) 
#define DIGITAL_PIN_TO_MASK(p)   (1<<(p%8))    
#define PIN_TO_ANALOG(p)        digitalPinToAnalogChannel(p) // defined in esp32-hal-gpio.h
// ESP32 supports PWM on almost all pins, but only 16 pins can use pwm at once.
#define PIN_TO_PWM(p)           (p)
#define PIN_TO_SERVO(p)         (p)
#define DEFAULT_PWM_RESOLUTION  13
#define DEFAULT_ADC_RESOLUTION  12
#define LARGE_MEM_DEVICE        320

#else
#error "Analog pin macros not defined in board.h for this chip"
#endif

#if not defined (IS_PIN_DIGITAL)
#define IS_PIN_DIGITAL(P) (P < TOTAL_PINCOUNT)      // assumes all pins can be used as digital pins
#endif
#if not defined (PIN_TO_DIGITAL)
#define PIN_TO_DIGITAL(P) (P) 
#endif

#if not defined (digitalPinHasPWM)
//#define digitalPinHasPWM digitalPinToTimer
#endif

#if not defined (IS_PIN_PWM)
#define IS_PIN_PWM(P)  (digitalPinHasPWM(P))
#endif

#if not defined (PIN_TO_PWM)
#define PIN_TO_PWM(P)  (P)
#endif

// board name macros
#if defined(__AVR_ATmega168__)    
#define CHIP_NAME "ATmega168" 
#elif defined(__AVR_ATmega328__)
#define CHIP_NAME "ATmega328"
#elif defined(__AVR_ATmega328P__)
#define CHIP_NAME "ATmega328P"
#elif defined(__AVR_ATmega32U4__)
#define CHIP_NAME "ATmega32U4"
#elif defined(__AVR_ATmega1280__)
#define CHIP_NAME "ATmega1280"
#elif defined(__AVR_ATmega2560__)
#define CHIP_NAME "ATmega2560"
#elif defined(__AVR_ATmega644P__)
#define CHIP_NAME "ATmega644P"
#elif defined(__MK20DX128__)
#define CHIP_NAME "MK20DX128"
#elif defined(__MK20DX256__)
#define CHIP_NAME "Teensy3.1"
#elif defined (_VARIANT_ARDUINO_DUE_X_)
#define CHIP_NAME "Arduino DUE"
#elif defined (__arm__) && defined (__SAM3X8E__)
#define CHIP_NAME "SAM3X8E"
#elif defined (UNO_WIFI_REV2_328MODE)
#define CHIP_NAME "Uno Wifi Rev2"
#elif defined(TARGET_RP2040) || defined(TARGET_RASPBERRY_PI_PICO)
#define CHIP_NAME "Pi Pico 2040"
#elif defined(ARDUINO_ARCH_ESP32)
#define CHIP_NAME "ESP32"
#else
#define CHIP_NAME "Unrecognized chip"
#endif

#endif 


