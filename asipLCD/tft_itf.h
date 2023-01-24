/*
 * tft_itf.h -  lcd text abstraction layer for tft_eSPI library
 *
 * 
 * Copyright (C) 2022 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

// todo TFT_I2C_POWER set high to enable power for adafruit esp32 feather tft
// fixme use hard coded value for chars per line, fix font select

// only boards with lots are RAM are supported
#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__) || \
    defined(__MKL26Z64__) ||defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) || \
    defined(__SAM3X8E__) ||         \
    defined(ARDUINO_SAMD_MKR1000) || \
    defined(_VARIANT_ARDUINO_ZERO_) || \
    defined(_VARIANT_ARDUINO_101_X_) || \
    defined(UNO_WIFI_REV2_328MODE) || \
    defined(ARDUINO_ARCH_ESP32) || \
    defined(TARGET_RP2040)  

#include <TFT_eSPI.h>
#include <SPI.h>

uint16_t tft_colors[] = {TFT_BLACK,TFT_NAVY,TFT_DARKGREEN,TFT_DARKCYAN,TFT_MAROON,TFT_PURPLE,TFT_OLIVE,      
                         TFT_LIGHTGREY,TFT_DARKGREY,TFT_BLUE,TFT_GREEN,TFT_CYAN,TFT_RED,TFT_MAGENTA,    
                         TFT_YELLOW,TFT_WHITE,TFT_ORANGE,TFT_GREENYELLOW,TFT_PINK,TFT_BROWN,TFT_GOLD,
                         TFT_SILVER, TFT_SKYBLUE, TFT_VIOLET}; 

int nbrTftColors = 24;

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
    TFT_eSprite txtSprite[MAX_LCD_LINES] = {TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft)};  
    uint16_t foreColor = TFT_WHITE;
    uint16_t backColor = TFT_BLACK;
    // setup_t user; // The library defines the type "setup_t" as a struct of config info
   
};    
    lcdInterface::lcdInterface() {}
     
    void lcdInterface::begin(int fontSize, bool invert = false)
    {
      tft.init();
      // tft.getSetup(user); // note this struct uses 88 bytes
      displayHeight = tft.width();
      displayWidth = tft.height();
      //tft.setTextDatum(TL_DATUM); // Set datum 
      if(invert)
          tft.setRotation(3);
      else
          tft.setRotation(1);
      setFont(fontSize);
      tft.fillScreen(backColor);
      tft.setTextSize(4);
      tft.setTextColor(foreColor);  
      
      if (TFT_BL > 0) {                          // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
        pinMode(TFT_BL, OUTPUT);                 // Set backlight pin to output mode
        digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);  // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
      }

      for(int i=0; i <  nbrTextLines; i++){
       //txtSprite[i].setColorDepth(1);
       txtSprite[i].createSprite(displayWidth, (int)(displayHeight/5));
       txtSprite[i].fillSprite(backColor);
       txtSprite[i].setTextColor(foreColor, 0);
      }
      #if defined TFT_I2C_POWER
            Serial.print("TFT_I2C_POWER is defined");
      #endif
    }

    void lcdInterface::setColor16(uint16_t color)
    {
        
      // sets given color as background, forground changed to contrasting color
      backColor = color;
      uint32_t color24  = tft.color16to24(color);
      uint16_t R =  ((color24 >> 16) & 0xFF) ; 
      uint16_t G =  ((color24 >> 8) & 0xFF); 
      uint16_t B = (color24 & 0x0000ff);
      uint16_t luminance = 0.2126 * R + 0.7152 * G + 0.0722 * B;
      if( luminance > 127)
        foreColor =  TFT_BLACK;
      else
        foreColor = TFT_WHITE; 
      tft.fillScreen(backColor);  
      //Serial.printf("color=%d, r=%d, g=%d, b=%d, val=%f\n", color, R,G,B, luminance );     
      for(int i=0; i < nbrTextLines; i++) {
          text(lcdTextBuffer[i], i, 0);  // redraw using new color
      }      
    }

    void lcdInterface::setColorRGB(uint8_t R, uint8_t G, uint8_t B)
    {
       setColor16(tft.color565(R, G, B));       
    }
    
    void lcdInterface::setFont(int fontSize)
    {

        tft.setTextSize(fontSize); 

        int maxFontHeight =  tft.fontHeight();
        int fontAscent =  maxFontHeight; //fixme
        int fontDescent = 0; // fixme 
        fontHeight = maxFontHeight;
        fontWidth = tft.textWidth("0123456789")/10;
        nbrTextLines = (int) (displayHeight / fontHeight) ;
        if( nbrTextLines > MAX_LCD_LINES) nbrTextLines = MAX_LCD_LINES;
        charsPerLine = (int)(displayWidth / fontWidth);
        int lineHeight = displayHeight / nbrTextLines;       

        // Serial.printf("font info %d: ascent=%d, descent=%d, height=%d, max height=%d, width=%d, nbr lines=%d, chars per line=%d\n", fontSize, fontAscent, fontDescent, fontHeight, maxFontHeight, fontWidth, nbrTextLines, charsPerLine); 

    }
     
    void lcdInterface::text(const char *text, int row, int column)
    {   
        txtSprite[row].setTextColor(foreColor, backColor);
        txtSprite[row].fillSprite(backColor);
        txtSprite[row].drawString(text, 0, 0, 4);
        txtSprite[row].pushSprite(1, 4+(displayHeight/nbrTextLines) * row); // left margin 1, top 4
        if(strcmp(text, lcdTextBuffer[row]) != 0 ) // check if text is cached
           strlcpy(&lcdTextBuffer[row][column], text, charsPerLine-column); // cache text in case of color change
    }

    void lcdInterface::clear()
    {     
        tft.fillScreen(backColor);
        memset(lcdTextBuffer, 0, sizeof(lcdTextBuffer)); // clear the text buffer 
    }

    void lcdInterface::clearLine(int line)
    {
        tft.fillRect(0, lcdTxtTop[line], displayWidth-1, fontHeight, backColor);  
        memset(&lcdTextBuffer[line], 0, MAX_LCD_CHARS_PER_LINE);
    }

    void lcdInterface::hGraph(int line, int value)
    {        
       /* 
       u8g2.setDrawColor(0);
       u8g2.drawBox( 0, lcdTxtTop[line], width-1, fontHeight-1);
       u8g2.setDrawColor(1);
       u8g2.drawBox( 0, lcdTxtTop[line]+1, map(value, 0, 100, 0, width), fontHeight-1);
       */     
    }

    // the following is not implimented in ASIP
    void lcdInterface::hGraph(char * title, int value1, int value2,int value3,int value4)
    {
       /*   
        u8g2.clearBuffer();
        u8g2.drawStr(0, 0, title);
        u8g2.drawBox( 0, lcdTxtTop[1]+1, map(value1, 0, 100, 0, width), fontHeight-1);
        u8g2.drawBox( 0, lcdTxtTop[2]+1, map(value2, 0, 100, 0, width), fontHeight-1);
        u8g2.drawBox( 0, lcdTxtTop[3]+1, map(value3, 0, 100, 0, width), fontHeight-1);
        u8g2.drawBox( 0, lcdTxtTop[4]+1, map(value4, 0, 100, 0, width), fontHeight-1);
        */    
    }


lcdInterface lcd;
  
#else
#error("LCD not supported on this board, see u8g_itf.cpp for supported boards")    
#endif