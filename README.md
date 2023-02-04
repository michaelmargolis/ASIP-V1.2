# Asip 1.2.0#
ASIP (Arduino Service Interface Protocol) is a protocol to control an Arduino board from a computer. The protocol is intended to be a more extensible replacement for Firmata.

### What is this repository for? ###

This repository includes the code for core Asip functionality and the following additional services:
* asipRobot - motor and other Mirto hardware specific libraries  
* asipPixel - Neopixel support
* asipLCD - Support for monochrome and TFT lCDs
* asipIMU - Support for  MPU6050 (gyro and accelerometer), HMC5883L (magnetometer), BMP085 (altimeter)



### Changes in this release ###
* Added support for a wider range of boards, including ESP32 AND Pico2040 boards. Note that boards without EEPROM such as the Pico do not save changes to PID parameters.
* Added support for wider range of LCD hardware, including color displays
* Enhanced ASIP pixels to enable setting of background color on LCD displays
* Changes are backwards compatible – all ASIP 1.1 clients should work as is with V1.2


### Installation ###
Copy the following folders from this repository to your Arduino libraries directory:
* ASIP
* asipPixels - needs Adafruit_NeoPixel library
* asipLCD - needs U8g2 and/or TFT-eSPI libraries depending on sketch
* asipRobot - Needs modification to some third party libraries, see readme file in 'Modified arduino libraries' folder
* asipHeading and/or asipIMU if you need IMU support
