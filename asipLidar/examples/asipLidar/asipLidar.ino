/*
  asipLidar test sketch
*/
#include <asip.h>       // the base class definitions
#include <asipIO.h>     // the core I/O class definition
#include <asipLidar.h>  // Lidar service


const char* sketchName = "asipLidar";

#define LIDAR_SERIAL_PORT Serial1
pinArray_t lidarPins[2] = { 17,16 };  // hardware serial pins used on above port: rx,tx

asipLidarClass asipLidar(id_LIDAR_SERVICE);

asipService services[] = {
  &asipLidar,
  &asipIO  // the core class for pin level I/O
};


void setup() {
  while (!Serial)
    ;  // For leonardo board
  Serial.begin(57600);

  asip.begin(&Serial, asipServiceCount(services), services, sketchName);
  asip.reserve(SERIAL_RX_PIN);        // reserve pins used by the serial port
  asip.reserve(SERIAL_TX_PIN);        // these defines are in asip/boards.h
  LIDAR_SERIAL_PORT.setRX(lidarPins[0]);  // set pin assgnments for serial
  LIDAR_SERIAL_PORT.setTX(lidarPins[1]);
  LIDAR_SERIAL_PORT.setFIFOSize(64);  // fifo must be at least 64 bytes
  asipLidar.begin(&LIDAR_SERIAL_PORT, lidarPins);
  asipIO.begin();  // start the IO service

  for (unsigned int i = 0; i < asipServiceCount(services); i++) {
    services[i]->reportName(&Serial);
    Serial.print(" is service ");
    Serial.write(services[i]->getServiceId());
    Serial.println();
  }
}

void loop() {
  asip.service();
  asipLidar.service(); // lidar events sent outside asip event scheduler
}