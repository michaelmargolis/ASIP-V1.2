/*
   ASIP Robot sketch for mirto 2016 and 2018 boards

    This sketch depends on the following libraries in addition to core ASIP:
     (libraries preceeded with * are third party, install using library manager)
      asipRobot
      asipPixels
      Encoder
      u8g2 LCD librrary
      Adafruit_NeoPixel
*/

/*
  ASIP test messages (newline terminates each messsage):
  protocol reference document is here: https://github.com/michaelmargolis/ASIP-V1.1/tree/master/Documents

    A,I,0        <- stop ir autoevents
    A,I,50       < set ir autoevent period to 50ms

    M,M,30,-30   <- Set first motor power to 30%, second to -30%
    M,M,0,0      <- stop both motors

    T,P,440,500   <- play 440Hz tone for 500ms

    L,W,0,Hello World  <-  write Hello World to first line of LCD

    to change the onboard pixel, set the value after the colon to the desried 32bit color value
    P,P,0,1,{0:128}       <- set first pixel to blue  (half brightness)
    P,P,0,1,{0:32768}     <- pixel to green (half brightness)  = 128*255
    P,P,0,1,{0:8388608}   <- set first pixel to red  (half brightness)  = 128*255*255

*/

#include <asip.h>                  // the base class definitions
#include <asipIO.h>                // the core I/O class definition

// the following three services are supported by the standard Arduino distribution
#include <services/asipTone.h>      // square wave tone generator

// the following services use seperate libraries
#include <asipRobot.h>        // definitions for mirtle services(motor, ir, encoder etc) 
#include <asipPixels.h>       // for neopixels 
#include <asipLCD.h>          // LCD
#include <services/asipDistance.h> // ultrasonics distance sensor

#include "UnoWifiRobot_pins.h"  // hardware pin defines

#ifdef ASIP_DEBUG
//#error "ASIP_DEBUG must be disabled when using this board, (it's in debug.h in asip lib folder)
#endif

// for wifi board
#include <SPI.h>
#include <WiFiNINA.h>

// #define MIRTO_IS_AP  // comment out if mirto is a client

int asipPort = 6789;

#ifdef MIRTO_IS_AP
// change both of the following two lines if a different access point IP address is required
char *ipAddressStr[16] = {"192.168.42.1"}; // the string to display
IPAddress ap_ip(192, 168, 42, 1);          // the address to use
#else
// edit mirto_secrets.h with ssid and pw if mirto is a client
#include "mirto_secrets.h"
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
#endif

WiFiServer server(asipPort);

int status = WL_IDLE_STATUS;

char const *sketchName = "MirtoUnoWiFi";

int tonePin;

const byte MAX_PIXELS = 1;  // increase this if more pixels are required
Adafruit_NeoPixel strip = Adafruit_NeoPixel(MAX_PIXELS, neoPixelPin, NEO_GRB + NEO_KHZ800);  // instantiate a strip

//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0); 
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, SCL, SDA);

// create the services defined in asipRobot.h
robotMotorClass motors(id_MOTOR_SERVICE, NO_EVENT);
//encoderClass encoders(id_ENCODER_SERVICE);
bumpSensorClass bumpSensors(id_BUMP_SERVICE);
irLineSensorClass irLineSensors(id_IR_REFLECTANCE_SERVICE);
//AccelerometerClass accelerometer(id_ACCELEROMETER_SERVICE); 
// create other asip services
asipToneClass asipTone(id_TONE_SERVICE, NO_EVENT);
asipPixelsClass asipPixels(id_PIXELS_SERVICE, NO_EVENT);
asipLCDClass asipLCD(id_LCD_SERVICE, NO_EVENT);
asipDistanceClass asipDistance(id_DISTANCE_SERVICE);


// make a list of the created services
asipService services[] = {
  &asipIO, // the core class for pin level I/O
  &motors,
  &bumpSensors,
  &irLineSensors,
 // &accelerometer,
  &asipTone,
  &asipPixels,
  &asipLCD,
  &asipDistance
};

void setup()
{
  asipLCD.begin(lcd_MEDIUM_FONT);

  beginAsipComms();
#ifdef MIRTO_IS_AP
  beginWiFiAP();
#else
  beginWiFiClient();
#endif

  // start the services
  // Board define is in UnoWifiRobot_pins.h
  motors.begin(2, 6, motorPins, 4, encoderPins); // two motors,a total of 6 motor pins,4 encoder pins
  tonePin = _tonePin;  // #defined in robot_pins.h

  bumpSensors.begin(2, 2, bumpPins);
  irLineSensors.begin(3, 4, irReflectancePins); // 3 sensors plus control pin
 // accelerometer.begin(3,startSPI);
  asipTone.begin(tonePin);
#ifdef ledPin
  asipIO.PinMode(ledPin, OUTPUT_MODE);
#endif
  asipDistance.begin(1,2, distancePins); // one distance sensor, two pins

  asipIO.begin();  // NEW from  v1.1: core I/O service must follow all other service begin methods
  asip.sendPinModes(); // for debug
  asip.sendPortMap();

  //showPins();  delay(4000);
  asipPixels.begin(neoPixelPin, &strip);
}

WiFiClient client;

void startSPI(char service)
{
  // no pins need reserving because UnoWiFiR2 uses 'hidden' pins for SPI 
}

void loop()
{
  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    showConnection("client connected", "");
    asip.changeStream(&client);              // pipe client stream to ASIP
    while (client.connected()) {            // loop while the client's connected
      while (client.available() > 0 && client.peek() < 32) {
        // strip control characters
        client.read();
      }
      asip.service();
    }
    // close the connection: ?
    client.stop();
    Serial.println("client disconnected");
    IPAddress ip = WiFi.localIP();
    showConnection("client disconnect", ipToStr(ip));
    motors.stopMotors();
    asip.changeStream(&Serial);                // restore ASIP to USB until next connection
  }
  else {
    asip.service();
  }
}

void beginAsipComms()
{
  Serial.begin(ASIP_BAUD);  // init the port even though only Wifi used for ASIP
  if (ASIP_MAJOR_VERSION == 1 && ASIP_MINOR_VERSION == 2)
    asipLCD.text("ASIP Ver 1.2", 0);
  else
    asipLCD.text("Unsupported ASIP Version", 0);
  asipLCD.text(sketchName, 1);
  asipLCD.text(CHIP_NAME, 2);

  asip.begin(&Serial, asipServiceCount(services), services, sketchName);
  showConnection("Connecting to", SECRET_SSID);
}

void showConnection(const char *line1, const char *line2)
{
  asipLCD.text(line1, 2);
  asipLCD.text(line2, 3);
}

void showSSID(const char *ssid)
{
  asipLCD.text(ssid, 0);
}

#ifdef MIRTO_IS_AP
void beginWiFiAP()
{
  // create ssid from mac
  byte mac[6];
  WiFi.macAddress(mac);
  uint64_t shortMac = (uint64_t)mac;
  char ssid[15];
  snprintf(ssid, 15, "Mirto-%04X", shortMac);

  //asip.changeStream(&Serial1);  // temp !!!!!!
  //Serial.println("temp divert serial stream to enable viewing of status msgs");
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    showConnection("WiFI comms Err", "Please reboot");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  // by default the local IP address of will be 192.168.4.1
  // override to the address at the top of this sketch
  WiFi.config(ap_ip);

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);
  showSSID(ssid);

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    showConnection("AP Error!", "Please reboot");
    // don't continue
    while (true);
  }
  ap_ip = WiFi.localIP();
  ipToStr(ap_ip);
  showConnection("Starting AP!", "");

  IPAddress ip;

  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed, update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      // start the server
      server.begin();
      // device connected to the AP, waiting for user
      Serial.print("Waiting for client, ");
      ip = WiFi.localIP();
      showConnection("waiting client", ipToStr(ip));
      Serial.print("IP Address: ");  Serial.println(ip);
      Serial.println(ipToStr(ip));

    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
      showConnection("Listening", ipToStr(ip));
    }
  }
}

#else

void beginWiFiClient()
{
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:

    server.begin();
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address:"); Serial.println(ipToStr(ip));
    showConnection("IP Address:", ipToStr(ip));
  }
}
#endif

void showPins()
{
  Serial.print("motor pins ");
  for (int i = 0; i < 6; i++) {
    Serial.print(motorPins[i]); Serial.print(",");
  }
  Serial.print("\nEncoder pins ");
  for (int i = 0; i < 4; i++) {
    Serial.print(encoderPins[i]); Serial.print(",");
  }
  Serial.print("\ntone pin ");
  Serial.println(tonePin);
  Serial.print("bump pins ");
  for (int i = 0; i < 2; i++) {
    Serial.print(bumpPins[i]); Serial.print(",");
  }
  Serial.println("\nreflectance pins ");
  for (int i = 0; i < 4; i++) {
    Serial.print(irReflectancePins[i]); Serial.print(",");
  }
  Serial.println("\n");
}

char *ipToStr(IPAddress ip)
{
  static char buffer[16];
  char *pos = buffer;
  char octet[3]; // holds chars for a single octet
  for (int i = 0; i < 4; i++) {
    int num = ip[i];
    if (num == 0) *pos++ = '0';
    else {
      int j = 0;
      while (num != 0) {
        octet[j]  = (num % 10) + '0';
        num = num / 10;
        j++;
      }
      while (j-- > 0)
        *pos++ = octet[j]; // reverse the digits
    }
    if (i < 3) *pos++ = '.';
  }
  *pos = '\0';
  return buffer;
}