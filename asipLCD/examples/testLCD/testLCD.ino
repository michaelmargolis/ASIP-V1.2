/*
   testLCD
   This version uses the library: U8g2 by oliver
   install using library manager or download from https://github.com/olikraus/u8g2
      
*/

#include <asip.h>       // the base class definitions
#include <asipIO.h>     // the core I/O class definition

    #define _U8G_LCD_ // use u8g2lib
    #include <U8g2lib.h> 
    U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
    //U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, SCL, SDA); // 128x32 oled
    #define nbrLcdPins 2 // lcd pins to be reserved
    pinArray_t lcdPins[nbrLcdPins] = {SCL, SDA}; // pins to reserve
    #define LCD_FONT lcd_MEDIUM_FONT
    #define MAX_LCD_LINES 5
    #define MAX_LCD_CHARS_PER_LINE  21 
    #define IS_LCD_FLIPPED false

#include <asipLCD.h>

char const *sketchName = "LCD Test sketch";

// create the services
asipLCDClass asipLCD(id_LCD_SERVICE, NO_EVENT);

// make a list of the created services
asipService services[] = {
  &asipIO, // the core class for pin level I/O  
  &asipLCD
};

void setup()
{
  beginAsipComms();
  // start the services   
  asipLCD.begin(nbrLcdPins, lcdPins, LCD_FONT);
  asipIO.begin();  // NEW from  v1.1: core I/O service must follow all other service begin methods

  for (unsigned int i = 0; i < asipServiceCount(services); i++)
  {
    services[i]->reportName(&Serial);
    Serial.print(" is service ");
    Serial.write(services[i]->getServiceId());
    Serial.println();
  } 
  
  char buffer[22];
  sprintf(buffer,"ASIP Ver %d.%d",ASIP_MAJOR_VERSION,ASIP_MINOR_VERSION );
  asipLCD.text(buffer,0);
  asipLCD.text(sketchName,1); 
  asipLCD.text(CHIP_NAME,2);
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
