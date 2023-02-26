/*
 * asip.cpp -  Arduino Services Interface Protocol (ASIP)
 * 
 * Copyright (C) 2014,2020 Michael Margolis
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */


#include "asip.h"
#include "asipIO.h"

// message strings, Move this to program memory ?
char const *errStr[] = {"NO_ERROR", "INVALID_SERVICE", "UNKNOWN_REQUEST", "INVALID_PIN", "MODE_UNAVAILABLE", "INVALID_MODE", "WRONG_MODE", "INVALID_DEVICE_NUMBER", "DEVICE_NOT_AVAILABLE", "I2C NOT ENABLED"};
 
asipClass:: asipClass(){
    // moved here from begin 6 May 2017
    // set all pins to UNALLOCATED_PIN state  
  for(byte p=0; p < TOTAL_PINCOUNT; p++) { 
     storePinMode(p, UNALLOCATED_PIN_MODE);
  }
}
 
void asipClass::begin(Stream *s, int svcCount, asipServiceClass **serviceArray, char const *sketchName )
{
 stream = s;  
 debug_printf("\n"); // debug output 
 debug_printf("ASIP %d.%d with sketch %s on %s\n", ASIP_MAJOR_VERSION, ASIP_MINOR_VERSION, sketchName, CHIP_NAME);
 verbose_printf("Verbose Debug enabled\n"); // this will only print if VERBOSE_DEBUG macro argument is uncommented

  services = serviceArray;
  nbrServices = svcCount; 

  programName = (char*)sketchName;
  s->write(INFO_MSG_HEADER);
  s->print(sketchName);  
  // list all implemented service tags
  s->print(F(" running on "));
  s->print(F(CHIP_NAME));
  s->print(F(" with Services: "));
  
  for(int i=0; i < svcCount; i++ ){
    s->write(services[i]->ServiceId);
    s->write(' ');
  }
  s->write(MSG_TERMINATOR); 
}

void asipClass::changeStream(Stream *s)
{ 
  stream = s;
}

void asipClass::service()
{   
  if(stream->available() >= MIN_MSG_LEN) {
     int tag = stream->read();
     if( tag > ' ') { // ignore control characters
        // Serial.print("got asip tag "); Serial.write(tag); Serial.println();
        if(tag == SYSTEM_MSG_HEADER) {
          if(stream->read() == ',') {// tag must be followed by a separator 
              processSystemMsg();
           }
        }           
        else if(tag == INFO_MSG_HEADER) {
           processDebugMsg();
        }        
        else {
          int svc = 0; 
          while(svc < nbrServices) {
            // Serial.print(" checking if tag "); Serial.print((char)tag); Serial.print(" equals "); Serial.print(services[svc]->ServiceId);  
            if( services[svc]->ServiceId == tag) {
              if(stream->read() == ',') {// tag must be followed by a separator
                // Serial.print("Received request for "); services[svc]->reportName(&Serial); Serial.print(", tag="); Serial.write(stream->peek()); Serial.println();
                services[svc]->processRequestMsg(stream);
                break;
              }    
            }
           svc++;
           if(svc >= nbrServices) { // check if no match

             sendErrorMessage((char)tag, (const char)'?',ERR_INVALID_SERVICE, stream);                  
             while( stream->available() && stream->read() != '\n') // skip to the end of  line
                 ;             
            }   
          } 
        }           
     }
  }  
  // service digital inputs
  sendDigitalPortChanges(stream, false);
  
  // auto events for services:
  uint32_t currentTick = millis();
  for(int i=0; i < nbrServices; i++) {
    if( services[i]->autoInterval > 0) {  // zero disables autoInterval
      if( currentTick >= services[i]->nextTrigger )  {
         services[i]->reportValues(stream);
         services[i]->nextTrigger =  currentTick + services[i]->autoInterval; // reset the count
         //verbose_printf("Counter reset to %u\n", services[i]->nextTrigger);
      }      
    }    
  }  
}

void asipClass::processDebugMsg()
{   
#ifdef ASIP_DEBUG
    // echo incoming debug messages to the debug stream
    int c;
    unsigned int startMillis = millis();
    debugStream->write(INFO_MSG_HEADER);
    do {
      c = stream->read();
      if (c >= 0) {
        debugStream->write(c);
      }      
    } while( c != '\n' && millis() - startMillis < 1000); // wait at most one second for the end of the message        
#endif    
}

void asipClass::processSystemMsg()
{
   int request = stream->read();   
   if(request == tag_SYSTEM_GET_INFO) {
      stream->write(EVENT_HEADER);   
      stream->write(SYSTEM_MSG_HEADER);
      stream->write(',');
      stream->write(tag_SYSTEM_GET_INFO);
      stream->write(',');
      stream->print(ASIP_MAJOR_VERSION);
      stream->write(',');
      stream->print(ASIP_MINOR_VERSION);
      stream->write(',');
      stream->print(CHIP_NAME);
      stream->write(',');
      stream->print(TOTAL_PINCOUNT);
      stream->write(',');
      stream->print(programName);
      stream->write(MSG_TERMINATOR);
   }
   else if(request == tag_SERVICES_NAMES) {
      // sends a list of service IDs and their friendly names
      stream->write(EVENT_HEADER);
      stream->write(SYSTEM_MSG_HEADER);
      stream->write(',');
      stream->write(tag_SERVICES_NAMES);
      stream->write(',');
      stream->print(nbrServices);
      stream->write(',');   
      stream->write('{');     
      for(byte i=0; i < nbrServices; i++) {
         char svcId = services[i]->ServiceId;
         if(isValidServiceId(svcId))  {
            stream->write( svcId ); 
            stream->write( ':' );
            services[i]->reportName(stream);                       
            if( i < nbrServices-1)
               stream->write(',');
         }  
       }
       stream->write('}');
       stream->write(MSG_TERMINATOR);
   }
   else if(request == tag_PIN_SERVICES_LIST) {
 // sends a list of all pins with associated service id if any
      stream->write(EVENT_HEADER);
      stream->write(SYSTEM_MSG_HEADER);
      stream->write(',');
      stream->write(tag_PIN_SERVICES_LIST);
      stream->write(',');
      stream->print(TOTAL_PINCOUNT);
      stream->write(','); 
      stream->write('{');
      for(byte p=0; p < TOTAL_PINCOUNT; p++) {    
         int svc = (char)getServiceId(p);
         stream->write( svc );          
         if( p != TOTAL_PINCOUNT-1)
            stream->write(',');
          else  
            stream->write('}');
      }
      stream->write(MSG_TERMINATOR);
   }   
   else if(request == tag_RESTART_REQUEST) {
      debug_printf("Resetting services\n");
       for(int i=0; i < nbrServices; i++) {
           services[i]->reset();
           services[i]->autoInterval = 0;   // this disables autoInterval          
       }
   }
   else {
     sendErrorMessage((char)SYSTEM_MSG_HEADER, (const char)request, ERR_UNKNOWN_REQUEST, stream);
   }
}

// returns error code
asipErr_t asipClass::registerPinMode(byte pin, pinMode_t mode, char serviceId)
{
  asipErr_t err = ERR_NO_ERROR; 
  if(pin >= 0 && pin < TOTAL_PINCOUNT) {    
    verbose_printf("registerPinMode for pin %d for mode %d by service %c\n", pin, mode, serviceId );  
    // only system can set RESERVE_MODE
    if( serviceId == id_IO_SERVICE || (mode == RESERVED_MODE && serviceId == SYSTEM_SERVICE_ID) || (mode == OTHER_SERVICE_MODE  && isValidServiceId(serviceId))){
        verbose_printf("in register pin %d has mode %d\n", pin, getPinMode(pin) );   
        if( getPinMode(pin) < RESERVED_MODE) {            
          storePinMode(pin,mode); 
          pinRegister[pin].service = serviceId - '@';     
          verbose_printf("register pin %d for mode %d for service %c (as %d)\n", pin, mode,serviceId,pinRegister[pin].service );                       
        }
        else {
         err = ERR_MODE_UNAVAILABLE;
        }    
    }
    else {
      err = ERR_INVALID_SERVICE;
    }
  }
  else {
     err = ERR_INVALID_PIN;
  }
  return err;
}

// only used for system reset requests
asipErr_t asipClass::deregisterPinMode(byte pin)
{
  asipErr_t err = ERR_NO_ERROR;  
  if(pin >= 0 && pin < TOTAL_PINCOUNT) {     
    if( getPinMode(pin) < RESERVED_MODE) {    
      storePinMode(pin,UNALLOCATED_PIN_MODE);  
      verbose_printf("deregister  pin %d\n", pin);  
    }
    else {
     err = ERR_MODE_UNAVAILABLE;
    }    
  }
  else {
     err = ERR_INVALID_PIN;
  }
  if(err) debug_printf("de-register error %d\n", err);
  return err;
}

// reserve the given pin
asipErr_t asipClass::reserve(byte pin)
{
     verbose_printf("Reserving pin %d\n", pin);  
     return registerPinMode(pin,RESERVED_MODE,SYSTEM_SERVICE_ID);
} 
 
bool asipClass::isValidServiceId(char serviceId)
{
  return (serviceId >= 'A' && serviceId <= 'Z');
}

asipServiceClass*  asipClass::serviceFromId( char tag)
{
   asipServiceClass* svcPtr = NULL;
    for(int svc=0; svc < nbrServices; svc++) {
         if( services[svc]->ServiceId == tag) {
            svcPtr = services[svc];
            break;
         }
    }
    return svcPtr;  
}   

// Stores the mode of the given pin 
void asipClass::storePinMode(byte pin, pinMode_t mode) 
{
  if( pin >=0 && pin < TOTAL_PINCOUNT) {
    //pinModes[pin] = mode;
    pinRegister[pin].mode = mode; 
   // Serial.print("!!!! pin "); Serial.print(pin); Serial.print(" set to mode "); Serial.println(mode);
  } 
}

// returns the mode of the given pin 
pinMode_t asipClass::getPinMode(byte pin) 
{
  if( pin >=0 && pin < TOTAL_PINCOUNT) {
    return (pinMode_t)pinRegister[pin].mode;
    //return pinModes[pin];
  }
  else {
    return INVALID_MODE;
  } 
}

// returns the service id associated with the given pin 
char asipClass::getServiceId(byte pin) 
{
  char svc = '?';
  if( pin >=0 && pin < TOTAL_PINCOUNT) {
    
    if( pinRegister[pin].mode == OTHER_SERVICE_MODE){
       svc =  (char) pinRegister[pin].service + '@';
     }
    else if( pinRegister[pin].mode == RESERVED_MODE) {
      svc = '@';      
    }   
    else
      svc = id_IO_SERVICE;
  }         
   return svc;  
}

 
void asipClass::sendPinModes()  // sends a list of all pin modes
{
  stream->write(EVENT_HEADER);
  stream->write(id_IO_SERVICE);
  stream->write(',');
  stream->write(tag_PIN_MODES);
  stream->write(',');
  stream->print(TOTAL_PINCOUNT);
  stream->write(',');  // comma added 21 June 2014
  stream->write('{');
  for(byte p=0; p < TOTAL_PINCOUNT; p++) {
     //int mode = (char)pinModes[p]; 
     int mode = (char)getPinMode(p);
     stream->print( mode); 
     if( p != TOTAL_PINCOUNT-1)
        stream->write(',');
      else  
        stream->write('}');
  }
  stream->write(MSG_TERMINATOR); 
} 

void asipClass::sendPinCapabilites()  // sends a bitfield array indicating capabilities all pins 
{
  capabilityMask mask;
  
  stream->write(EVENT_HEADER);
  stream->write(id_IO_SERVICE);
  stream->write(',');
  stream->write(tag_PIN_CAPABILITIES);
  stream->write(',');
  stream->print(TOTAL_PINCOUNT); 
  stream->write(',');  // comma added 21 June 2014
  stream->write('{');
  for(byte p=0; p < TOTAL_PINCOUNT; p++) {
     mask.ch = 0; // clear the mask
     IS_PIN_DIGITAL(p) ? mask.bits.DIGITAL_IO = 1 : mask.bits.DIGITAL_IO = 0;
     IS_PIN_ANALOG(p) ? mask.bits.ANALOG_INPUT = 1 :  mask.bits.ANALOG_INPUT = 0;
     IS_PIN_PWM(p)    ? mask.bits.PWM_OUTPUT = 1 : mask.bits.PWM_OUTPUT = 0 ;    
     stream->write( mask.ch + '0'); // convert to a printable character 
     if( p != TOTAL_PINCOUNT-1)
        stream->write(',');
      else  
        stream->write('}');
  }
  stream->write(MSG_TERMINATOR); 
} 

void asipClass::sendPortMap()
{
byte port,mask;
  // note that port numbers may not start at 0 and may not be consecutive
  // pins that have no digital capability have port and mask values of 0 
  stream->write(EVENT_HEADER);
  stream->write(id_IO_SERVICE);
  stream->write(',');
  stream->write(tag_GET_PORT_TO_PIN_MAPPING);
  stream->write(',');
  stream->print(TOTAL_PINCOUNT);
  stream->write(',');  // comma added 21 June 2014
  stream->write('{');
  for(byte p=0; p < TOTAL_PINCOUNT; p++) {
    if(IS_PIN_DIGITAL(p)) {
       port = DIGITAL_PIN_TO_PORT(p);
       mask = DIGITAL_PIN_TO_MASK(p);
    }   
    else {
       port = mask = 0;
    }
     stream->print(port); // port number sent as decimal
     stream->write(':');
     stream->print(mask, HEX); // note the mask is sent as Hex
     if( p != TOTAL_PINCOUNT-1)
        stream->write(',');
      else  
        stream->write('}');
  }
  stream->write(MSG_TERMINATOR); 
}

void asipClass::sendAnalogPinMap()
{
  int pinsToReport = TOTAL_ANALOG_PINS;
  // sends pairs of digital:analog pin associations
  stream->write(EVENT_HEADER);
  stream->write(id_IO_SERVICE);
  stream->write(',');
  stream->write(tag_GET_ANALOG_PIN_MAPPING);
  stream->write(',');
  stream->print(TOTAL_ANALOG_PINS);
  stream->write(','); 
  stream->write('{');
  for(byte p=0; p < TOTAL_PINCOUNT; p++) {
    if(IS_PIN_ANALOG(p)) {
      stream->print(p);
      stream->write(':');
      stream->print(PIN_TO_ANALOG(p)); 
     if( --pinsToReport > 0)
        stream->write(',');
      else  
        stream->write('}');
    }
  } 
  stream->write(MSG_TERMINATOR); 
  if( pinsToReport != 0) {
    debug_printf("number of analog pins is off by %d\n", pinsToReport);
  }
}

void asipClass::sendErrorMessage( const char svc, const char tag, const asipErr_t err, Stream *stream)
{
  stream->write(ERROR_MSG_HEADER);
  stream->write(svc);
  stream->write(',');
  stream->write(tag);  
  stream->write(',');  
  stream->print(err);
  stream->print('{');  
  stream->print(errStr[err]); 
  stream->write('}');
  stream->write(MSG_TERMINATOR);   
} 

asipClass asip;

asipServiceClass::asipServiceClass(const char svcId, const char evtId) :
   ServiceId(svcId), EventId(evtId) 
{
}

asipServiceClass::asipServiceClass(const char svcId) :
   ServiceId(svcId), EventId(tag_SERVICE_EVENT) 
{
}

void asipServiceClass::begin(byte _nbrElements, byte pinCount, const pinArray_t pins[])
{
  nbrElements =  _nbrElements;
  this->pinCount = pinCount;
  this->pins = pins;
  autoInterval = 0; // turn off auto events
  for( byte p=0; p <pinCount; p++) {
     asip.registerPinMode(pins[p], OTHER_SERVICE_MODE,ServiceId);
  } 
}
// begin with additional pins for secondary functionality (encoder for exampple)
void asipServiceClass::begin(byte _nbrElements, byte pinCount, const pinArray_t pins[], byte secondaryPinCount, const pinArray_t SecondaryPins[])
{
  nbrElements =  _nbrElements;
  this->pinCount = pinCount;
  this->pins = pins;
  autoInterval = 0; // turn off auto events
  for( byte p=0; p <pinCount; p++) {
     asip.registerPinMode(pins[p], OTHER_SERVICE_MODE,ServiceId);
  } 
  for( byte p=0; p < secondaryPinCount; p++) {
     asip.registerPinMode(SecondaryPins[p], OTHER_SERVICE_MODE,ServiceId);
  } 
  // todo - check to see if these pins need to be stored in service instance?
}

void asipServiceClass::begin(byte nbrElements, const pinArray_t pins[])
{
  begin(nbrElements, nbrElements, pins);
}

void asipServiceClass::begin(byte pin)
{
  singlePin = pin;
  begin(1, 1, &singlePin);
}


  
void asipServiceClass::begin(byte _nbrElements, serviceBeginCallback_t serviceBeginCallback) // begin with no pins starts an I2C service
{
  nbrElements =  _nbrElements;
  autoInterval = 0; // turn off auto events
  if(serviceBeginCallback != NULL) {
    if( serviceBeginCallback(ServiceId) == false) {
       // service failed to start
    }   
  }  
}

// can be invoked by clients to restore conditions to start-up state
 void asipServiceClass::reset()
 {
 
 }
 
asipServiceClass::~asipServiceClass(){} 

void asipServiceClass::reportValues(Stream *stream) 
{
  stream->write(EVENT_HEADER);
  stream->write(ServiceId);
  stream->write(',');
  stream->write(EventId);
  stream->write(',');
  stream->print(nbrElements);
  stream->write(',');
  stream->write('{');
  for(byte count = 0; count < nbrElements; count++){   
      reportValue(count, stream);
      if(count < nbrElements-1)
         stream->write(',');  // comma between all but last element
  }
  stream->write('}');
  stream->write(MSG_TERMINATOR); 
  
}

void asipServiceClass::setAutoreport(Stream *stream) // reads stream and sets number of ticks between events 
{
  unsigned int ticks = stream->parseInt();
  setAutoreport(ticks);
}

void asipServiceClass::setAutoreport(unsigned int ticks) // sets number ticks between events, 0 disables 
{
  autoInterval = ticks;
  //unsigned int currentTick = millis(); // truncate to a 16 bit value
  nextTrigger = millis() + autoInterval; // set the next trigger tick count
}

char asipServiceClass::getServiceId()
{
  return ServiceId;
}

void asipServiceClass::reportName(Stream *stream)
{
  PGM_P name  = this->svcName;
  for (uint8_t c; (c = pgm_read_byte(name)); name++) {
    stream->write(c); 
  }
}

void asipServiceClass::reportError( const char svc, const char request, asipErr_t err, Stream *stream)
{
   asip.sendErrorMessage(svc,request, err, stream);
}
