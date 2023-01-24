/*
 * asipPixels.cpp -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2015 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *  
 */

#include "asipPixels.h"

const byte DEFAULT_BRIGHTNESS = 64;


asipPixelsClass::asipPixelsClass(const char svcId, const char evtId)
  : asipServiceClass(svcId) {
  svcName = PSTR("Pixels");
  setColorCallback = nullptr;
}

void asipPixelsClass::begin(byte pin, Adafruit_NeoPixel* strip) {
  stripPtr = strip;
  stripIndex = 0;
  nbrStrips = 1;                             // this constructor is for a single strip
  strip->begin();                            // initialize the NeoPixel library.
  strip->setBrightness(DEFAULT_BRIGHTNESS);  // 1/4 brightness
  pinMode(pin, OUTPUT);
  asipServiceClass::begin(pin);  // register the pin
}

void asipPixelsClass::begin(byte nbrElements, const pinArray_t pins[], Adafruit_NeoPixel* strips) {
  //each strip uses one pin so pinCount equals nbrElements
  stripPtr = strips;
  for (int i = 0; i < nbrElements; i++) {
    strips[i].begin();  // initialize the NeoPixel library.
    pinMode(pins[i], OUTPUT);
    strips[i].setBrightness(DEFAULT_BRIGHTNESS);  // 1/4 brightness
  }
  stripIndex = 0;
  nbrStrips = nbrElements;
  asipServiceClass::begin(nbrElements, nbrElements, pins);
}

void asipPixelsClass::begin(setColorCallback_t callback) {
  setColorCallback = callback;
}

void asipPixelsClass::begin(byte pin, Adafruit_NeoPixel* strip, setColorCallback_t callback) {
  setColorCallback = callback;
  begin(pin, strip);
}

void asipPixelsClass::reset() {
}

void asipPixelsClass::reportValues(Stream* stream) {
}

void asipPixelsClass::reportValue(int sequenceId, Stream* stream)  // send the value of the given device
{
  stream->println("Pixels");
}

void asipPixelsClass::processRequestMsg(Stream* stream) {
  int request = stream->read();
  stream->print("processing request "); stream->write(request); stream->println();
  if (request == tag_SET_PIXELS) {
    //Comma separated pairs of colon separated pixel positions and color values
    // request:  "P,P,strip,count,{pixel:color,...}\n"  // first pixel is 0
    stripIndex = stream->parseInt();
    int count = stream->parseInt();
    debug_printf("got pixel msg for %d pixels on strip %d\n", count, stripIndex);
    if (stream->read() == ',' && stream->read() == '{')  // skip to start of parms
    {
      // todo - check to ensure  count, data pairs, and closing bracket are consistent
      while (count--) {
        int pixel = stream->parseInt();
        unsigned long color = stream->parseInt();
        debug_printf("set pixel %d to %d\n", pixel, color);
        ///stripPtr[stripIndex].setPixelColor(pixel, color);
        setPixelColor(pixel, color);
      }
      // show is now called for each pixel, fixme if the latency is too high
      //stripPtr[stripIndex].show();
    }


  } else if (request == tag_SET_PIXELS_RGB) {
    //Comma separated pairs of colon separated pixel positions and RGB color values
    // request:  "P,p,strip,count,{pixel:R:G:B,...}\n"  // first pixel is 0
    stripIndex = stream->parseInt();
    int count = stream->parseInt();
    debug_printf("got pixel msg for %d pixels on strip %d\n", count, stripIndex);
    if (stream->read() == ',' && stream->read() == '{')  // skip to start of parms
    {
      // todo - check to ensure  count, data pairs, and closing bracket are consistent
      while (count--) {
        int pixel = stream->parseInt();
        uint8_t R = stream->parseInt();
        uint8_t G = stream->parseInt();
        uint8_t B = stream->parseInt();
        setPixelColor(pixel, R, G, B);
        //stripPtr[stripIndex].setPixelColor(pixel, color);
        // todo check if count > 0 then next char must be comma
      }
      // show is now called for each pixel, fixme if the latency is too high
      //stripPtr[stripIndex].show();
    }
  }

  else if (request == tag_SET_PIXEL_SEQUENCE) {
    // Comma separated hex color values for pixels starting from position given in the firstPixel field
    // request:  "P,S,strip,count,firstPixel,{color,color,...}\n"
    stripIndex = stream->parseInt();
    int count = stream->parseInt();
    int firstPixel = stream->parseInt();
    debug_printf("pixel sequence msg for %d pixels starting from %d\n", count, firstPixel);
    if (stream->read() == ',' && stream->read() == '{')  // skip to start of parms
    {
      // todo - check to ensure  count, data pairs, and closing bracket are consistent
      while (count--) {
        unsigned long color = stream->parseInt();
        //stripPtr[stripIndex].setPixelColor(firstPixel++, color);
        setPixelColor(firstPixel++, color);
      }
      //stripPtr[stripIndex].show();
    } else {
      debug_printf("Pixel Sequence msg missing comma or open bracket\n");
    }

  } else if (request == tag_SET_PIXEL_SEQUENCE_RGB) {
    stripIndex = stream->parseInt();
    int count = stream->parseInt();
    int firstPixel = stream->parseInt();
    debug_printf("pixel sequence msg for %d pixels starting from %d\n", count, firstPixel);
    if (stream->read() == ',' && stream->read() == '{')  // skip to start of parms
    {
      // todo - check to ensure  count, data pairs, and closing bracket are consistent
      while (count--) {
        uint8_t R = stream->parseInt();
        uint8_t G = stream->parseInt();
        uint8_t B = stream->parseInt();
        //stripPtr[stripIndex].setPixelColor(firstPixel++, color);
        setPixelColor(firstPixel++, R, G, B);
      }
      //stripPtr[stripIndex].show();
    } else {
      debug_printf("Pixel Sequence msg missing comma or open bracket\n");
    }

  } else if (request == tag_SET_BRIGHTNESS) {
    stripIndex = stream->parseInt();
    int brightness = stream->parseInt();
    // sets overall strip brightness, 255 is max
    // stripPtr[stripIndex].setBrightness(brightness);
    // stripPtr[stripIndex].show();
    setBrightness(brightness);


  } else if (request == tag_GET_NUMBER_PIXELS) {
    // request:  "P,I\n"
    // reply:  @P,I,count\n"
    int index = stream->parseInt();
    if (index < nbrStrips) {
      stripIndex = index;
      int count = stripPtr[stripIndex].numPixels();
      stream->write(EVENT_HEADER);
      stream->write(id_PIXELS_SERVICE);
      stream->write(',');
      stream->write(tag_GET_NUMBER_PIXELS);
      stream->write(',');
      stream->print(stripIndex);
      stream->write(',');
      stream->print(count);  // actual connected pixels may be less
      stream->write(MSG_TERMINATOR);
    }
    {
      debug_printf("pixel strip %d is out of range\n", index);
    }
  } else if (request == tag_CLEAR_ALL_PIXELS) {
    int index = stream->parseInt();
    if (index < nbrStrips) {
      stripIndex = index;
      clear();
      //stripPtr[stripIndex].clear();
      //stripPtr[stripIndex].show();
    } else {
      debug_printf("pixel strip %d is out of range\n", index);
    }
  } else {
    reportError(ServiceId, request, ERR_UNKNOWN_REQUEST, stream);
  }
  while (stream->available() && stream->read() != '\n')
    ;  // skip to end of line

  stripIndex = 0;  // set index back to default
}

// enable calling from sketch to control strip 0
void asipPixelsClass::setPixelColor(int pixel, uint32_t color) {
  debug_printf("in func, set pixel %d to hex %x\n", pixel, color);
  if (stripPtr && stripIndex < nbrStrips) {
    stripPtr[stripIndex].setPixelColor(pixel, color);
    stripPtr[stripIndex].show();
  }
  if (setColorCallback) {
    uint8_t R = ((color >> 16) & 0xFF);
    uint8_t G = ((color >> 8) & 0xFF);
    uint8_t B = (color & 0x0000ff);
    setColorCallback(R, G, B);
  }
}

void asipPixelsClass::setPixelColor(int pixel, uint8_t r, uint8_t g, uint8_t b)  // enable calling from sketch
{
  debug_printf("in func, set pixel %d to %d,%d,%d\n", pixel, r, g, b);
  if (stripPtr && stripIndex < nbrStrips) {
    stripPtr[stripIndex].setPixelColor(pixel, r, g, b);
    stripPtr[stripIndex].show();
  }
  if (setColorCallback) {
    setColorCallback(r, g, b);
  }
}

void asipPixelsClass::setBrightness(int brightness)  // ignored if no strip
{
  if (stripPtr && stripIndex < nbrStrips) {
    // sets overall strip brightness, 255 is max
    stripPtr[stripIndex].setBrightness(brightness);
    stripPtr[stripIndex].show();
  }
}

void asipPixelsClass::clear() {
  if (stripPtr && stripIndex < nbrStrips) {
    // sets overall strip brightness, 255 is max
    stripPtr[stripIndex].clear();
    stripPtr[stripIndex].show();
  }
  if (setColorCallback) {
    setColorCallback(0, 0, 0);
  }
}
