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
#include <U8g2lib.h> // for LCD

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

class asipLCDClass : public asipServiceClass
{  
public:
   asipLCDClass(const char svcId, const char evtId);
   void begin(int fontSize=lcd_SMALL_FONT); 
   void reset();
   void reportValues(Stream * stream);
   void reportValue(int sequenceId, Stream * stream) ; // not used in this service  
   void processRequestMsg(Stream *stream);  
  void text(const char *txt, int row);
  void text(const char *txt, int row, int column);
  void clear();
  void clearLine(int line);
  void hGraph(int line, int value);
  void hGraph(char * title, int value1, int value2,int value3,int value4);
private:
  void show();
};   

extern asipLCDClass asipLCD;

#endif
 
   


