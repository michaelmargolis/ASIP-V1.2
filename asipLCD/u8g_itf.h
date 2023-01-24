/*
 * u8g_itf.h -  lcd text abstraction layer for u8g2 library
 *
 * 
 * Copyright (C) 2022 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */


// only boards with lots are RAM are supported
#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__) || \
    defined(__MKL26Z64__) ||defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) || \
    defined(__SAM3X8E__) ||         \
    defined(ARDUINO_SAMD_MKR1000) || \
    defined(_VARIANT_ARDUINO_ZERO_) || \
    defined(_VARIANT_ARDUINO_101_X_) || \
    defined(UNO_WIFI_REV2_328MODE) || \
    defined(ESP8266) || \
    defined(TARGET_RP2040) || defined(TARGET_RASPBERRY_PI_PICO)

#include <U8g2lib.h> 

class lcdInterface 
{
  public:  
    uint16_t displayHeight ;
    uint16_t displayWidth ;
    int fontHeight ;
    int fontWidth ;
    int nbrTextLines  ;
    int charsPerLine ;
    int lcdTxtTop[MAX_LCD_LINES]; 
    int lcdTxtBaseline[MAX_LCD_LINES] ;
    char lcdTextBuffer[MAX_LCD_LINES][MAX_LCD_CHARS_PER_LINE+1];  // max nbr chars for small font             
    lcdInterface();
    void begin(int fontSize, bool invert);
    void setFont(int fontSize);
    void setColor16(uint16_t color);
    void setColorRGB(uint8_t R, uint8_t G, uint8_t B);
    void text(const char *txt, int row, int column);
    void clear();
    void clearLine(int line);
    void hGraph(int line, int value);
    void hGraph(char * title, int value1, int value2,int value3,int value4);
  private:
    void show();
   
}; 


   
    lcdInterface::lcdInterface() {}
     
    void lcdInterface::begin(int fontSize, bool invert = false)
    {
        u8g2.begin();
        setFont(fontSize); 
        displayWidth = u8g2.getDisplayWidth(); 
        displayHeight =  u8g2.getDisplayHeight();
        if(invert)
            u8g2.setFlipMode(1);
    }

    void lcdInterface::setColor16(uint16_t color)
    {
       // ignored    
    }
    
    void lcdInterface::setColorRGB(uint8_t R, uint8_t G, uint8_t B)
    {
       // ignored       
    }
    
    void lcdInterface::setFont(int fontSize)
    {
        switch(fontSize) 
        {
            case 1: u8g2.setFont( u8g2_font_crox1hb_tr ); break;
            case 2: u8g2.setFont( u8g2_font_crox2hb_tr ); break;
            case 3: u8g2.setFont( u8g2_font_crox3hb_tr ); break;
            case 4: u8g2.setFont( u8g2_font_crox4hb_tr ); break;
            default: u8g2.setFont( u8g2_font_crox1hb_tr ); break;
        }

        
        int fontAscent =  u8g2.getFontAscent();
        int fontDescent = u8g2.getFontDescent();
        int maxFontHeight =  u8g2.getMaxCharHeight();
        fontHeight = maxFontHeight;
        fontWidth = u8g2.getStrWidth("0123456789")/10;
        nbrTextLines = (int) (u8g2.getDisplayHeight() / fontHeight) ;
        charsPerLine = (int)(u8g2.getDisplayWidth() / (fontWidth/10));
        int lineHeight = u8g2.getDisplayHeight()/nbrTextLines;
        

        //Serial.printf("font info %d: ascent=%d, descent=%d, height=%d, max height=%d, width=%d, nbr lines=%d, chars per line=%d\n", fontSize, fontAscent, fontDescent, fontHeight, maxFontHeight, fontWidth, nbrTextLines, charsPerLine); 
        
        for(byte i=0; i < nbrTextLines; i++){
            lcdTxtTop[i]= i * fontHeight; 
            lcdTxtBaseline[i] = lcdTxtTop[i] +  lineHeight + fontDescent; 
        }
    }
     
    void lcdInterface::text(const char *txt, int row, int column)
    {   
        strlcpy(&lcdTextBuffer[row][column], txt, charsPerLine-column);
        Serial.printf("text: row = %d, column= %d, txt=%s, buff=%s\n", row, column,txt, &lcdTextBuffer[row][column]);    
        show();
    }

    void lcdInterface::show()
    {     
        u8g2.clearBuffer(); // the pixel buffer
        for ( int line = 0; line < nbrTextLines; line++) {   
          u8g2.setCursor(0, lcdTxtBaseline[line]);
          u8g2.print(&lcdTextBuffer[line][0]);
           // Serial.printf("show: line = %d, y = %d, %s\n", line, lcdTxtBaseline[line], &lcdTextBuffer[line][0]);    
        }    
        u8g2.sendBuffer();         // transfer internal memory to the display
    }

    void lcdInterface::clear()
    {      
        memset(lcdTextBuffer, 0, sizeof(lcdTextBuffer)); // clear the text buffer
        u8g2.clearBuffer(); // the pixel buffer
        u8g2.sendBuffer();
    }

    void lcdInterface::clearLine(int line)
    {
        u8g2.setDrawColor(0);
        u8g2.drawBox( 0, lcdTxtTop[line], displayWidth-1, fontHeight);  
        u8g2.setDrawColor(1);
        u8g2.sendBuffer();
        memset(&lcdTextBuffer[line], 0, MAX_LCD_CHARS_PER_LINE);
        // Serial.printf("clearLine: x=%d, y=%d, x1=%d, y1=%d\n",0, lcdTxtTop[line], width-1, fontHeight ); 
    }

    void lcdInterface::hGraph(int line, int value)
    {
       u8g2.setDrawColor(0);
       u8g2.drawBox( 0, lcdTxtTop[line], displayWidth-1, fontHeight-1);
       u8g2.setDrawColor(1);
       u8g2.drawBox( 0, lcdTxtTop[line]+1, map(value, 0, 100, 0, displayWidth), fontHeight-1);
       u8g2.sendBuffer();
    }

    // the following is not implimented in ASIP
    void lcdInterface::hGraph(char * title, int value1, int value2,int value3,int value4)
    {
        u8g2.clearBuffer();
        u8g2.drawStr(0, 0, title);
        u8g2.drawBox( 0, lcdTxtTop[1]+1, map(value1, 0, 100, 0, displayWidth), fontHeight-1);
        u8g2.drawBox( 0, lcdTxtTop[2]+1, map(value2, 0, 100, 0, displayWidth), fontHeight-1);
        u8g2.drawBox( 0, lcdTxtTop[3]+1, map(value3, 0, 100, 0, displayWidth), fontHeight-1);
        u8g2.drawBox( 0, lcdTxtTop[4]+1, map(value4, 0, 100, 0, displayWidth), fontHeight-1);
        u8g2.sendBuffer();    
    }

lcdInterface lcd;
  
#else
#error("LCD not supported on this board, see u8g_itf.cpp for supported boards")    
#endif