/*
   pixel test
   
    to change the onboard pixel, set the value after the colon to the desried 32bit color value
    P,P,0,1,{0:128}       <- set first pixel to blue  (half brightness)
    P,P,0,1,{0:32768}     <- pixel to green (half brightness)  = 128*255
    P,P,0,1,{0:8388608}   <- set first pixel to red  (half brightness)  = 128*255*255
*/

#include <asip.h>       // the base class definitions
#include <asipIO.h>     // the core I/O class definition

#include <asipPixels.h>

char const *sketchName = "PixelTest";

#define neoPixelPin  13 
const int MAX_PIXELS = 1;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(MAX_PIXELS, neoPixelPin, NEO_RGB + NEO_KHZ800);

// create the services
asipPixelsClass asipPixels(id_PIXELS_SERVICE, NO_EVENT);

// make a list of the created services
asipService services[] = {
  &asipIO, // the core class for pin level I/O  
  &asipPixels
};

void setup()
{
  beginAsipComms();
  // start the services   
  asipPixels.begin(neoPixelPin, &strip);
  asipIO.begin();  // NEW from  v1.1: core I/O service must follow all other service begin methods

  for (unsigned int i = 0; i < asipServiceCount(services); i++)
  {
    services[i]->reportName(&Serial);
    Serial.print(" is service ");
    Serial.write(services[i]->getServiceId());
    Serial.println();
  } 
  show();
}

void show() {
  asipPixels.setPixelColor(0, 255,0,0);
  delay(1000);
  asipPixels.setPixelColor(0, 0,255,0);
  delay(1000);
  asipPixels.setPixelColor(0, 0, 0, 255);
  delay(1000);
  for( int i = 0; i < 256; i++) {
    Wheel(i);
    delay(20);        
  }
   asipPixels.setPixelColor(0, 0, 0, 0);
}
// Input a value 0 to 255 to set color value.
// The colours are a transition r - g - b - back to r.
void Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
     asipPixels.setPixelColor(0, 255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
     asipPixels.setPixelColor(0, 0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
   asipPixels.setPixelColor(0, WheelPos * 3, 255 - WheelPos * 3, 0);
}

void beginAsipComms()
{  
  Serial.begin(ASIP_BAUD);   // init both ports even though only one used for ASIP
  delay(200); // wait for USB to init - todo  
  asip.begin(&Serial, asipServiceCount(services), services, sketchName);   
}

void loop()
{
  asip.service();
}