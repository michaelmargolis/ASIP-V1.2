
#include <asip.h>       // the base class definitions
#include <asipIO.h>     // the core I/O class definition
#include "service/asipIMU.h"  // Inertial measurement services 
#include "I2Cdev.h"   // needed for above  
#include "MPU6050.h"
#include "HMC5883L.h"
#include "BMP085.h"

const char * sketchName = "TestI2C";

bool i2cStarted = false; // flag to indcate that i2c started

gyroClass gyro3Axis(id_GYRO_SERVICE); 
AccelerometerClass accelerometer3Axis(id_ACCELEROMETER_SERVICE); 
HeadingClass heading3Axis(id_HEADING_SERVICE);
PressureClass pressure(id_PRESSURE_SERVICE);

asipService services[] = { 
                                 &asipIO, // the core class for pin level I/O
                                 &gyro3Axis,
                                 &accelerometer3Axis,
                                 &heading3Axis,
                                 &pressure};


void setup() {
  while( !Serial); // For leonardo board
  Serial.begin(57600);
 // Serial.begin(250000);
  
  asip.begin(&Serial,  asipServiceCount(services), services, sketchName); 
  asip.reserve(SERIAL_RX_PIN);  // reserve pins used by the serial port 
  asip.reserve(SERIAL_TX_PIN);  // these defines are in asip/boards.h 
  asipIO.begin(); // start the IO service
  gyro3Axis.begin(NBR_GYRO_AXIS,startI2C); // I2C services use begin method with nbr of elements (axis) & start callback
  accelerometer3Axis.begin(NBR_ACCEL_AXIS,startI2C); // gyro and accel have x,y,z axis 
  heading3Axis.begin(NBR_MAG_AXIS,startI2C); // 3 raw values, 4th element is the calculated compass heading 
  pressure.begin(NBR_PRESSURE_FIELDS,startI2C); // pressure, tempearture and altitude
  
  asip.sendPinModes(); // for debug
  asip.sendPortMap();
  for(unsigned int i=0; i< asipServiceCount(services); i++)
  {
    services[i]->reportName(&Serial); 
    Serial.print(" is service ");
    Serial.write(services[i]->getServiceId());
    Serial.println();  
  }
}

void loop() 
{
  asip.service();
}

bool startI2C( char service)
{
  // for now, all services that call this method require I2C
  if( !i2cStarted) {  
     asip.reserve(SDA);  // reserve pins used by I2C 
     asip.reserve(SCL);  // these defines are in pins_arduino.h  
     Wire.begin();
     i2cStarted = true;
  }
  return true; 
}
