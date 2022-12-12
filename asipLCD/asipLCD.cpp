/*
 * asipLCD.cpp -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2015 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This version uses the U8Gg2 library: https://github.com/olikraus/u8g2
 
 * updated  May 2019 to support 2 line oleds
 * u8g2 instance should be created in sketch
 */


#include "asipLCD.h" 

// only boards with lots are RAM are supported
#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__) || \
    defined(__MKL26Z64__) ||defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) || \
    defined(__SAM3X8E__) ||         \
    defined(ARDUINO_SAMD_MKR1000) || \
    defined(_VARIANT_ARDUINO_ZERO_) || \
    defined(_VARIANT_ARDUINO_101_X_) || \
    defined(UNO_WIFI_REV2_328MODE) ||\
    defined(ESP8266)                   // ESP8266   

   
  //static U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE); // removed by MEM 2 Mar 2019
  //static U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, SCL, SDA);
  extern U8G2 u8g2; 
  static byte nbrTextLines = 5;  // actual lines and nbr chars may be less with larger fonts
  static byte charsPerLine = 21; // default for small font 
  static const byte lcdHeight = 64;
  static byte fontHeight = 11;  // default font ascender (cap) height 
  static byte txtTop[5];    // 5 is max nbr of text lines  
  static byte txtBottom[5]; 
  
  static char textBuffer[5][21+1];  // max nbr chars for small font

/*  
  static U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, SCL, SDA);
  static byte nbrTextLines = 5;
  static byte charsPerLine = 21;
  static const byte lcdHeight    = 64;
  static byte fontHeight   = 11;  // font ascender (cap) height
  static byte lineSpace    = (lcdHeight- fontHeight) / 4; 
  static byte txtTop[nbrTextLines]   = {0,lineSpace,lineSpace*2,lineSpace*3,lcdHeight-fontHeight-1};
  static byte txtBottom[nbrTextLines] = {fontHeight,lineSpace+fontHeight,lineSpace*2+fontHeight,lineSpace*3+fontHeight,lcdHeight-1};
*/
  
 void configureBuffer(int fontSize)
 {

    switch(fontSize) 
    {
        case 1: u8g2.setFont( u8g2_font_crox1hb_tr ); break;
        case 2: u8g2.setFont( u8g2_font_crox2hb_tr ); fontHeight=12; nbrTextLines=4;  break;
        case 3: u8g2.setFont( u8g2_font_crox3hb_tr );  fontHeight=13; nbrTextLines=4; break;
        default: u8g2.setFont( u8g2_font_crox1hb_tr ); break;
    }
    byte lineSpace    = (lcdHeight - fontHeight) / (nbrTextLines-1);   // default
    for(byte i=0; i < nbrTextLines; i++){
        txtTop[i]= i * lineSpace; 
        txtBottom[i] = txtTop[i] + fontHeight;        
    }     
 }
 
 asipLCDClass::asipLCDClass(const char svcId, const char evtId)
  :asipServiceClass(svcId)
{
  svcName = PSTR("LCD");
}

void asipLCDClass::begin(int fontSize)
{
     u8g2.begin();
    // font , color 
    configureBuffer(fontSize);
    clear();
}

void asipLCDClass::reset()
{

}

void asipLCDClass::reportValues(Stream * stream)
{
}

void asipLCDClass::reportValue(int sequenceId, Stream * stream)  // send the value of the given device   
{
   stream->println("LCD");
}

void asipLCDClass::processRequestMsg(Stream *stream)
{
  // request:  "L,W,line,string\n"  // top line is line 0
   int request = stream->read();  
   if( request == tag_WRITE) {
      int line = stream->parseInt();  
      if(line < nbrTextLines) {
         if(stream->find(",")) // skip past comma to get to text
         {
           char lineBuffer[charsPerLine+1];
           memset(&lineBuffer, 0, sizeof(lineBuffer));
           stream->readBytesUntil('\n', (char*)lineBuffer, charsPerLine);                    
           text(lineBuffer, line);
         }
      }
    }
    else if(request == tag_WRITE_RC){
/*
       this command is not yet supported        
        Request: write text starting from  given line and column (0 is first line and column)
          Header		Tag		Line Number		Column		Text            	Terminator
           ‘L ’ 	 ,	‘w’	 ,	digits	    ,	digits	‘	ASCII text string	‘\n’
        Example:  "L,w,0,6, again\n"    write again in 7th column of first line
*/        

      int line = stream->parseInt();  
      if(line < nbrTextLines) {
         if(stream->find(","))// skip past comma to get to text
         {
           int column =  stream->parseInt();
           if(column < charsPerLine && stream->read() == ',' ){    
              char lineBuffer[charsPerLine+1];
              memset(&lineBuffer, 0, sizeof(lineBuffer));              
              stream->readBytesUntil('\n', (char*)lineBuffer, charsPerLine - column);           
              text(lineBuffer, line, column);
           }              
         }
      }
        
    }
    else if(request == tag_GRAPH){
       int count = stream->parseInt(); 
       //Serial.printf("got graph request for %d lines\n", count);
       if(stream->find(",{")){ // skip to data
          for(int i=0; i < count; i++){
              int line = stream->parseInt();
              int val = stream->parseInt();
              hGraph(line,val);  // todo bounds checking
          }
       }       
    }   
    else if(request == tag_CLEAR) {
       clear();
    }    
}

void asipLCDClass::text(const char *txt, int row)
{
   clearLine(row);
   text(txt,row, 0);
}

void asipLCDClass::text(const char *txt, int row, int column)
{   
    //u8g2.clearBuffer(); // the pixel buffer 
    strlcpy(&textBuffer[row][column], txt, charsPerLine-column);
    //Serial.printf("text: row = %d, column= %d, txt=%s, buff=%s\n", row, column,txt, &textBuffer[row][column]);    
    show();
}

void asipLCDClass::show()
{     
    for ( int line = 0; line < nbrTextLines; line++) {   
      u8g2.setCursor(0, txtBottom[line]);
      u8g2.print(&textBuffer[line][0]);
      //Serial.printf("show: line = %d, y = %d, %s\n", line, txtBottom[line], &textBuffer[line][0]);    
    }    
    u8g2.sendBuffer();         // transfer internal memory to the display
}

void asipLCDClass::clear()
{      
    memset(textBuffer, 0, sizeof(textBuffer)); // the text buffer
    u8g2.clearBuffer(); // the pixel buffer
    u8g2.sendBuffer();
}

void asipLCDClass::clearLine(int line)
{
    int width =  u8g2.getDisplayWidth() - 1;
    u8g2.setDrawColor(0);
    u8g2.drawBox( 0, txtTop[line], width-1, txtBottom[line]);  
    u8g2.setDrawColor(1);
    u8g2.sendBuffer();
    //Serial.printf("clearLine: x=%d, y=%d, x1=%d, y1=%d\n",0, txtTop[line], width, txtBottom[line] ); 
}

void asipLCDClass::hGraph(int line, int value)
{
   int width =  u8g2.getDisplayWidth() - 1;
   u8g2.setDrawColor(0);
   u8g2.drawBox( 0, txtTop[line], width-1, fontHeight);
   u8g2.setDrawColor(1);
   u8g2.drawBox( 0, txtTop[line]+1, map(value, 0, 100, 0, width), fontHeight-1);
   u8g2.sendBuffer();
}

// the following is not implimented in ASIP
void asipLCDClass::hGraph(char * title, int value1, int value2,int value3,int value4)
{
    int width =  u8g2.getDisplayWidth() - 1;
    u8g2.clearBuffer();
    u8g2.drawStr(0, 0, title);
    u8g2.drawBox( 0, txtTop[1]+1, map(value1, 0, 100, 0, width), fontHeight-1);
    u8g2.drawBox( 0, txtTop[2]+1, map(value2, 0, 100, 0, width), fontHeight-1);
    u8g2.drawBox( 0, txtTop[3]+1, map(value3, 0, 100, 0, width), fontHeight-1);
    u8g2.drawBox( 0, txtTop[4]+1, map(value4, 0, 100, 0, width), fontHeight-1);
    u8g2.sendBuffer();    
}

#else
#error("LCD not supported on this board, see asipLCD.h for supported boards")    
#endif
