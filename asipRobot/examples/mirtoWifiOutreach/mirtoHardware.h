/* mirtoHardware.h
 * hardware defines for supported mirto variants
 */

#ifndef mirtoHardware_h
#define mirtoHardware_h

#define NO_PIN 255

// check for the hardware platform
#if defined(UNO_WIFI_REV2_328MODE)
    #define HAS_WIFI_ONBOARD  // comment this if board does not have wifi
    #define _UNO_WIFI_R2_
    #define _U8G_LCD_ // use u8g2lib
    #define LCD_FONT lcd_MEDIUM_FONT
    #define MAX_LCD_LINES 5
    #define MAX_LCD_CHARS_PER_LINE  21 
    #define IS_LCD_FLIPPED false
    #include <U8g2lib.h> 
    // U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, SCL, SDA); // 128x64 oled
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
   // U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0); 
    // pin defines for DRV8833 boards ('255' elements will be set PWM pin based on motor direction)
    const byte motorPins[]   = {6,9,NO_PIN, 10,5,NO_PIN};  // L-in1,L-in2, pwm,  R-in1,R-in2, pwm
    const byte tonePin = 13;
     // these are defined in Robot_pins.h  const byte encoderPins[] = {7,2,3,4}; // Left:A,B  Right:A,B
    const byte bumpPins[] = {12,11};
    const byte switchPin     = 11;   // switch connected to right bump pin
    const byte irReflectancePins[] = {8,A3,A2,A3}; // first is control, the remainder are used as analog inputs
    const byte potPin = A0;
    //#define NO_SERVO // definign this prevents loading servo code
    const byte servoPins[]    = {NO_PIN};
    const byte distancePins[] = {A4,A5};
    // the following pins use preprocessor defines to enable conditional compile
    #define neoPixelPin 1
    #define ledPin     LED_BUILTIN
    
#elif defined(__MK20DX256__)  // Teensy 3.x
  #define _TEENSY_
  #define MIRTO2016 // PCB selection
  //#define MIRTO2018 // PCB selection
  #if defined MIRTO2018    // pcb with DRV8833 H-Bridge
    byte motorPins[]   = {32, NO_PIN,20,9,NO_PIN,25};  // L-in1,no pin,L-PWM, R-in1,no pin,R-PWM
    const byte tonePin = 10;
    const byte switchPin = 33;
  #elif defined MIRTO2016    // pcb with 6612FNG H-Bridge
    byte motorPins[]   = {33,20,32,27,30,25};  // L-in1,L-in2,L-PWM, R-in1,R-in2,R-PWM
    const byte tonePin = 9; 
    const byte switchPin = 5;
  #else
    #error "Missing define for selecting Mirto PCB"   
  #endif 
     // these are defined in Robot_pins.h  const byte encoderPins[] = {29,31,26,28}; // Left:A,B  Right:A,B
    const byte bumpPins[] = {6,24};
    const byte irReflectancePins[] = {A0,A1,A2,A3}; // first is control, the remainder are used as analog inputs
    const byte servoPins[]    = {3};
    const byte distancePins[] = {4};
    // the following pins use preprocessor defines to enable conditional compile
    #define neoPixelPin 2
    #define ledPin     13
    
    #define _U8G_LCD_ // use u8g2lib
    #include <U8g2lib.h> 
    U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, SCL, SDA); // 128x32 oled
    #define LCD_FONT lcd_MEDIUM_FONT
    #define MAX_LCD_LINES 2
    #define MAX_LCD_CHARS_PER_LINE  21 
    #define IS_LCD_FLIPPED false

#elif defined(TARGET_RP2040) || defined(TARGET_RASPBERRY_PI_PICO)
    #define HAS_WIFI_ONBOARD  // comment this if board does not have wifi
    #define _PICO2040_
    //#define _U8G_LCD_ // use u8g2lib for mono  OLED
    #define _TFT_LCD_   // use TFT_eSPI for color LCD

    #ifdef _U8G_LCD_ 
    #include <U8g2lib.h> 
    U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, SCL0, SDA0); // 128x32 oled
    
    #elif  defined _TFT_LCD_
    #include <TFT_eSPI.h>
    #include <SPI.h>
    TFT_eSPI tft = TFT_eSPI(135, 240);  // Invoke custom library    
    #define peripheralPinsToReserve 5 // five lcd pins to be reserved
    pinArray_t peripheralPins[] = {1,2,3,4,5}; // pins used for LCD
    #endif
    
    #define LCD_FONT lcd_MEDIUM_FONT
    #define MAX_LCD_LINES 5
    #define MAX_LCD_CHARS_PER_LINE  21
    #define IS_LCD_FLIPPED true

    #define DISTANCE_I2C_BUS Wire
    const byte DISTANCE_ADDR = 0x57; // i2c address for RCWL-1601
    const byte SCL0 = 17;  //  i2c header for optional sensors
    const byte SDA0 = 16;
    const byte SCL1 = 20;  // i2c Stemma conn for RCWL-1601 distance sensor
    const byte SDA1 = 19;
 
    // pin defines for DRV8833 boards (NO_PIN elements will be set PWM pin based on motor direction)
    pinArray_t motorPins[]   = {7,6,NO_PIN, 11,10,NO_PIN};  // L-in1,L-in2, pwm,  R-in1,R-in2, pwm
    const byte tonePin = 0;
    // pinArray_t encoderPins[] = {21,22, 8,9}; // these are defined in Robot_pins.h
    pinArray_t bumpPins[] = {14,15};  
    const byte switchPin  = 12  ;
    pinArray_t irReflectancePins[] = {NO_PIN, 27, 26, 28}; // IR always on
    pinArray_t servoPins[]    = {20};
    pinArray_t distancePins[] = {SDA0, SCL0}; // uses Wire on Stemma connector
    // the following pins use preprocessor defines to enable conditional compile
    #define neoPixelPin 13
    #define ledPin     LED_BUILTIN

#elif defined (ARDUINO_ARCH_ESP32)
    #define HAS_WIFI_ONBOARD  // comment this if board does not have wifi
    #define _ESP32_
    #define _TFT_LCD_ // use TFT_eSPI lib
    #define LCD_FONT lcd_MEDIUM_FONT
    #define MAX_LCD_LINES 5
    #define MAX_LCD_CHARS_PER_LINE  17
    #define IS_LCD_FLIPPED true
    #include <TFT_eSPI.h>
    #include <SPI.h>
    TFT_eSPI tft = TFT_eSPI(135, 240);  // Invoke custom library    
    // pin defines for DRV8833 boards (NO_PIN elements will be set PWM pin based on motor direction)
    pinArray_t motorPins[]   = {12,13,NO_PIN, 33,32,NO_PIN};  // L-in1,L-in2, pwm,  R-in1,R-in2, pwm
    const byte tonePin = 25;
    //pinArray_t encoderPins[] = {NO_PIN,NO_PIN,NO_PIN,NO_PIN}; // NO ENCODERS 
    const byte bumpPins[] = {38,36};  
    const byte switchPin     = 0;  
    pinArray_t irReflectancePins[] = {NO_PIN, 2,15,39}; // IR always on 
    const byte potPin = 27;
    pinArray_t servoPins[]    = {26};
    pinArray_t distancePins[] = {21,22};  // distance uses i2c
    // the following pins use preprocessor defines to enable conditional compile
    // #define neoPixelPin 1
    // #define ledPin     LED_BUILTIN
    #define peripheralPinsToReserve 6 // six lcd pins to be reserved
    pinArray_t peripheralPins[] = {4,5,16,18,19,23}; // pins used for LCD
#else
#error "this code requires a teensy, Arduino Uno Wifi Rev2, pico2040, or ESP32"
#endif

#define encoderLeftPins  (&encoderPins[0])
#define encoderRightPins (&encoderPins[2])

#ifdef neoPixelPin
const byte MAX_PIXELS = 1;    
#include <Adafruit_NeoPixel.h>                                                               // increase this if more pixels are required
Adafruit_NeoPixel strip = Adafruit_NeoPixel(MAX_PIXELS, neoPixelPin, NEO_RGB + NEO_KHZ800);  // instantiate a strip
#endif

#endif // mirtoHardware_h
