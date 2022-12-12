// tinyLcd.h

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include <U8x8lib.h>
static U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);

const int FONT_HEIGHT = 1;

//temp, move this into class
static const int nbrTextLines = 4;
static const int charsPerLine = 16;
static const int lcdHeight    = 64;
static const int fontHeight   = 8 * FONT_HEIGHT;  // font ascender (cap) height

class LCD
{
  public:

    LCD() {};

    void begin()
    {
      u8x8.begin();
      u8x8.setFont(u8x8_font_chroma48medium8_r);
      clear();
    }

    void text(char *txt, int row)
    {
        if ( strlen(txt) > charsPerLine)
          txt[charsPerLine] = 0;
        clearLine(row );
        u8x8.drawString(0,1+row*2,txt);  
        //Serial.println( txt );   
    }

    void clear()
    {
      u8x8.clearDisplay();
    }

    void clearLine(int line)
    {
        u8x8.clearLine(1+line*2);      
    }
};

LCD lcd;
