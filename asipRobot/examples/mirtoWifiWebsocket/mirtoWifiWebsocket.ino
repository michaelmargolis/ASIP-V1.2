/*
   ASIP Robot sketch for mirto 2016 and 2018 boards

    This sketch depends on the following libraries in addition to core ASIP:
     (libraries preceeded with * are third party, install using library manager)
      asipRobot
      asipPixels
      Encoder
      u8g2 or TFT_eSPI LCD library
      Adafruit_NeoPixel
*/

/*
  ASIP test messages (newline terminates each messsage):
  protocol reference document is here: https://github.com/michaelmargolis/ASIP-V1.1/tree/master/Documents

    R,A,0        <- stop ir autoevents
    R,A,50       < set ir autoevent period to 50ms

    M,M,30,-30   <- Set first motor power to 30%, second to -30%
    M,M,0,0      <- stop both motors

    T,P,440,500   <- play 440Hz tone for 500ms

    L,W,0,Hello World  <-  write Hello World to first line of LCD

    to change the onboard pixel, set the value after the colon to the desried 32bit color value
    P,P,0,1,{0:128}       <- set first pixel to blue  (half brightness)
    P,P,0,1,{0:32768}     <- pixel to green (half brightness)  = 128*255
    P,P,0,1,{0:8388608}   <- set first pixel to red  (half brightness)  = 128*255*255
    P,p,0,1,{0:128,128,0} <- set first pixel to yellow (both red and green, no blue)
*/

#include <asip.h>    // the base class definitions
#include <asipIO.h>  // the core I/O class definition

#include "mirtoHardware.h"  // these defines were previously in robot_pins header file
#include "Robot_pins.h" // defines for encoder pins

// the following three services are supported by the standard Arduino distribution
#include <services/asipTone.h>  // square wave tone generator

// the following services use seperate libraries
#include <asipRobot.h>              // definitions for mirtle services(motor, ir, encoder etc)
#include <asipPixels.h>             // for neopixels
#include <asipLCD.h>                // LCD
#include <services/asipDistance.h>  // ultrasonics distance sensor
#include <services/asipServos.h>    // definitions for servo

#include "StreamToWebsocket.h"

#ifdef ASIP_DEBUG
//#error "ASIP_DEBUG must be disabled when using this board, (it's in debug.h in asip lib folder)
#endif

#include <Wire.h>

#ifdef HAS_WIFI_ONBOARD 
// here if board supports WiFi
#ifdef _UNO_WIFI_R2_
#include <WiFiNINA.h>
#else
#include <WiFi.h>
#endif
// edit mirto_secrets.h with ssid and pw if mirto is a client
#include "mirto_secrets.h"
char ssid[] = SECRET_SSID;      // your network SSID (name)
char password[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)
#endif

char const *sketchName = "MirtoWifiWebsocket";

// create the services defined in asipRobot.h
robotMotorClass motors(id_MOTOR_SERVICE, NO_EVENT);
//encoderClass encoders(id_ENCODER_SERVICE);
bumpSensorClass bumpSensors(id_BUMP_SERVICE);
irLineSensorClass irLineSensors(id_IR_REFLECTANCE_SERVICE);
asipPixelsClass asipPixels(id_PIXELS_SERVICE, NO_EVENT);
asipLCDClass asipLCD(id_LCD_SERVICE, NO_EVENT);
//AccelerometerClass accelerometer(id_ACCELEROMETER_SERVICE);
asipDistanceClass asipDistance(id_DISTANCE_SERVICE);
asipToneClass asipTone(id_TONE_SERVICE, NO_EVENT);
asipServoClass asipServo(id_SERVO_SERVICE, NO_EVENT);

Servo myServos[1];  // create one servo object

#include "tunes.h" // for melody player
PlayMelody player(tonePin);  // just for some music while connecting to WiFi

// make a list of the created services
asipService services[] = {
  &motors,
  &bumpSensors,
  &irLineSensors,
  &asipPixels,
  &asipLCD,
  // &accelerometer,
  &asipDistance,
  &asipServo,     
  &asipTone,
  &asipIO  // the core class for pin level I/O
};

void setup() {
  Serial.begin(57600);
  // start the services
  // Board defines in mirtoHardware.h
  asipLCD.begin(nbrLcdPins, lcdPins, LCD_FONT, IS_LCD_FLIPPED); // invert the display? (set in mirtoHardware.h)
  
#if defined neoPixelPin
  asipPixels.begin(neoPixelPin, &strip, setColorCallback );
  asipPixels.setPixelColor(0, 128 << 16); // red while starting up
#else
  asipPixels.begin(setColorCallback); // pixel requests will be resolved on LCD
#endif
  beginAsipComms();
  #if defined HAS_WIFI_ONBOARD
     beginWiFiClient();
  #endif
  motors.begin(2, 6, motorPins, 4, encoderPins);  // two motors,a total of 6 motor pins,4 encoder pins
  bumpSensors.begin(2, 2, bumpPins);
  irLineSensors.begin(3, 4, irReflectancePins);  // 3 sensors plus control pin
  // accelerometer.begin(3);
  asipTone.begin(tonePin);
  asipServo.begin(1,servoPins,myServos);  
#ifdef ledPin
  #ifndef _PICO2040_  // do not use with pico w
      asipIO.PinMode(ledPin, OUTPUT_MODE);
  #endif    
#endif
#ifdef _PICO2040_  
   pinMode(switchPin, INPUT_PULLUP);
#endif
#ifdef DISTANCE_I2C_BUS
  asipDistance.begin(1, 2, distancePins, DISTANCE_I2C_BUS, DISTANCE_ADDR);   
#endif

  //asipDistance.begin(1, 2, distancePins);  // one distance sensor, two pins
  //asip.sendPinModes();  // for debug
  //asip.sendPortMap();

  //showPins();  delay(4000);

#if defined peripheralPinsToReserve
     for(int i=0; i < peripheralPinsToReserve; i++){
        asip.reserve(peripheralPins[i]);
     }
#endif
  asipIO.begin();       // NEW from  v1.1: core I/O service must follow all other service begin methods
  delay(5000);
  asipPixels.setPixelColor(0, 16 << 8); // dim  green
}

void loop() {

#if defined HAS_WIFI_ONBOARD
  streamToWebsocket.service();
  if(streamToWebsocket.is_connected){
      asip.service();
  }
#endif  // HAS_WIFI_ONBOARD
  asip.service();
}

void setColorCallback(byte R, byte G, byte B){
   // this is called from Pixel service when color is changed
   // Serial.printf("Callback with R=%d, G=%d, B=%d\n", R, G, B);
   asipLCD.setColorRGB( R, G, B);
}

void connectionCallback(bool isConnected) {

  if ( isConnected) {
    Serial.print("Client connected: ");
    showConnection("client connected", ipToStr(streamToWebsocket.remoteIP())) ;
  } else {
    showConnection("client disconnected", ipToStr(WiFi.localIP()));
    delay(1000);
    showConnection(ssid, ipToStr(WiFi.localIP()));
  }
}

void beginAsipComms() {
  Serial.begin(ASIP_BAUD);  // init the port even though only Wifi used for ASIP
  if (ASIP_MAJOR_VERSION == 1 && ASIP_MINOR_VERSION == 2)
    asipLCD.text("ASIP Ver 1.2", 0);
  else
    asipLCD.text("Unsupported ASIP Version", 0);
  asipLCD.text(sketchName, 1);
  asipLCD.text(CHIP_NAME, 2);
  asip.begin(&Serial, asipServiceCount(services), services, sketchName);
 #if defined HAS_WIFI_ONBOARD 
  streamToWebsocket.begin();
  streamToWebsocket.registerCallback(connectionCallback);
  Serial.println("waiting for websocket to connect");
  while(! streamToWebsocket.is_connected){
      ;
  }
  Serial.println("connected");
  asip.changeStream(&streamToWebsocket);
   asip.begin(&streamToWebsocket, asipServiceCount(services), services, sketchName);
 #endif   
}

void showConnection(const char *line1, const char *line2) {
  asipLCD.text(line1, 2 % asipLCD.nbrTextLines());
  asipLCD.text(line2, 3 % asipLCD.nbrTextLines());
}

void showSSID(const char *ssid) {
  asipLCD.text(ssid, 0);
}

void beginWiFiClient() {
#if defined HAS_WIFI_ONBOARD
#ifndef _UNO_WIFI_R2_
  WiFi.mode(WIFI_STA); // needed for esp and pico
#endif
  showConnection("Connecting to", SECRET_SSID);
  unsigned long startTime = millis();
  unsigned long waitTime = 5000;   
  WiFi.begin(ssid, password);
  player.play(starwars, 108);
  while (WiFi.status() != WL_CONNECTED) {
    player.service(SCORE_END);
    if(millis() - startTime > waitTime) {
      WiFi.end();
      player.service(SCORE_END);
      delay(10);
      WiFi.begin(ssid, password);
      waitTime += 10000;
    }
  }
  while( player.service(SCORE_SECTION) ); // play to end of section
  Serial.println("");
  Serial.print("Mirto is connected to WiFi network ");  Serial.println(WiFi.SSID());
  Serial.print("Assigned IP Address: ");  Serial.println(WiFi.localIP());
  showConnection(ssid, ipToStr(WiFi.localIP()));
#endif  
}

void showPins() {
  Serial.print("motor pins ");
  for (int i = 0; i < 6; i++) {
    Serial.print(motorPins[i]);
    Serial.print(",");
  }
  Serial.println("\nEncoder pins: see Robot_pins.h in asipRobot folder");
  Serial.print("\ntone pin ");
  Serial.println(tonePin);
  Serial.print("bump pins ");
  for (int i = 0; i < 2; i++) {
    Serial.print(bumpPins[i]);
    Serial.print(",");
  }
  Serial.println("\nreflectance pins ");
  for (int i = 0; i < 4; i++) {
    Serial.print(irReflectancePins[i]);
    Serial.print(",");
  }
  Serial.println("\n");
}

#if defined HAS_WIFI_ONBOARD
char *ipToStr(IPAddress ip) {
  static char buffer[16];
  char *pos = buffer;
  char octet[3];  // holds chars for a single octet
  for (int i = 0; i < 4; i++) {
    int num = ip[i];
    if (num == 0) *pos++ = '0';
    else {
      int j = 0;
      while (num != 0) {
        octet[j] = (num % 10) + '0';
        num = num / 10;
        j++;
      }
      while (j-- > 0)
        *pos++ = octet[j];  // reverse the digits
    }
    if (i < 3) *pos++ = '.';
  }
  *pos = '\0';
  return buffer;
}
#endif
