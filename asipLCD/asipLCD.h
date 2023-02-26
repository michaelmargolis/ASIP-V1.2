/*
 * asipLCD.h -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2015 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

/*
 * This version requires assigning LCD type in sketch, see Mirtle2020.ino for example usage
 */

#ifndef asipLCD_h
#define asipLCD_h

#include "asip.h"

#if defined (_U8G_LCD_)
#include "u8g_itf.h"  // for i2c oled
#elif defined (_TFT_LCD_)
#include "tft_itf.h"
#else
#error "lcd interface not defined"    
#endif

// Service and method defines
// Service IDs must be unique across all services
// Method and event IDs must be unique within a service

//IDs already used by the core mirtle services:  I,M,E,B,R

// OLED LCD service
const char id_LCD_SERVICE = 'L';
// methods
const char tag_WRITE = 'W';            // write a line of text
const char tag_WRITE_RC = 'w';         // write text at given row and column
const char tag_GRAPH = 'G';            // draw graph
const char tag_CLEAR = 'C';            // clear screen

const int lcd_SMALL_FONT = 1;
const int lcd_MEDIUM_FONT = 2;
const int lcd_LARGE_FONT = 3;
const int lcd_XLARGE_FONT = 4;


class asipLCDClass : public asipServiceClass
{  
public:
   asipLCDClass(const char svcId, const char evtId);
   void begin(int fontSize=lcd_SMALL_FONT, bool invert=false);
   void begin(byte nbrPins, const pinArray_t pins[], int fontSize=lcd_SMALL_FONT, bool invert=false); 
   void reset();
   void reportValues(Stream * stream);
   void reportValue(int sequenceId, Stream * stream) ; // not used in this service  
   void processRequestMsg(Stream *stream);  
   void setColor16( uint16_t color);
   void setColorRGB(uint8_t R, uint8_t G, uint8_t B);
  void text(const char *txt, int row);
  void text(const char *txt, int row, int column);
  void clear();
  void clearLine(int line);
  void hGraph(int line, int value);
  void hGraph(char * title, int value1, int value2,int value3,int value4);
  int displayHeight();
  int nbrTextLines();
private:
    void configureBuffer(int fontSize);
};   

 asipLCDClass::asipLCDClass(const char svcId, const char evtId)
  :asipServiceClass(svcId)
{
  svcName = PSTR("LCD");
}

void asipLCDClass::begin(int fontSize, bool invert)
{
    lcd.begin(fontSize, invert);
}

void asipLCDClass::begin(byte nbrPins, const pinArray_t pins[], int fontSize, bool invert)
{
    asipServiceClass::begin(1, nbrPins, pins);
    lcd.begin(fontSize, invert);
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
      if(line < lcd.nbrTextLines) {
         if(stream->find(",")) // skip past comma to get to text
         {
           char lineBuffer[lcd.charsPerLine+1];
           memset(&lineBuffer, 0, sizeof(lineBuffer));
           stream->readBytesUntil('\n', (char*)lineBuffer, lcd.charsPerLine);     
           // Serial.printf("asipLcd Line %d {%s}\n",  line, lineBuffer ) ;   
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
      if(line < lcd.nbrTextLines) {
         if(stream->find(","))// skip past comma to get to text
         {
           int column =  stream->parseInt();
           if(column < lcd.charsPerLine && stream->read() == ',' ){    
              char lineBuffer[lcd.charsPerLine+1];
              memset(&lineBuffer, 0, sizeof(lineBuffer));              
              stream->readBytesUntil('\n', (char*)lineBuffer, lcd.charsPerLine - column);           
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

void asipLCDClass::setColor16( uint16_t color)
{
   lcd.setColor16(color);
}

void asipLCDClass::setColorRGB(uint8_t R, uint8_t G, uint8_t B)
{
   lcd.setColorRGB(R, G, B);
}

void asipLCDClass::text(const char *txt, int row)
{
   //clearLine(row);
   text(txt,row, 0);
}

void asipLCDClass::text(const char *txt, int row, int column)
{   
    //lcd.clearBuffer(); // the pixel buffer 
    lcd.text(txt, row, column);
}

void asipLCDClass::clear()
{      
    lcd.clear();
}

void asipLCDClass::clearLine(int line)
{
    lcd.clearLine(line);
}

void asipLCDClass::hGraph(int line, int value)
{
  lcd.hGraph(line, value);
}

// the following is not implimented in ASIP
void asipLCDClass::hGraph(char * title, int value1, int value2,int value3,int value4)
{
    lcd.hGraph(title,  value1,  value2, value3, value4);
}

int asipLCDClass::displayHeight()
{
    return lcd.displayHeight;
}

int asipLCDClass::nbrTextLines()
{
     return lcd.nbrTextLines;
}
  
//extern asipLCDClass asipLCD;

#endif
 
   


