/*
  StreamToWebsocket.h
  This class pipes an Arduino stream to a websocket
*/

#pragma once

#include <Stream.h>
#include <WebSocketsServer.h>

using ConnectionEvent = void (*)(bool);

class StreamToWebsocket : public Stream {

    uint8_t *in_buffer ;
    uint16_t inbuf_size;
    uint16_t in_head, in_tail;
    uint8_t *out_buffer ; 
    uint16_t outbuf_size;
    uint16_t out_pos;


    static void defaultEvtHandler(bool);
    
  public:
      static const uint16_t DEFAULT_SIZE = 128;
      bool is_connected;
      ConnectionEvent conEvent;


      StreamToWebsocket(uint16_t inbuf_size= StreamToWebsocket::DEFAULT_SIZE, uint16_t outbuf_size= StreamToWebsocket::DEFAULT_SIZE);
      ~StreamToWebsocket();
        
      void begin();  // todo pass port in begin method
      void service(); // call this in loop to service websockets
      void registerCallback( ConnectionEvent conEvent); // return ip addr of client when connected, null string when discon
      IPAddress remoteIP();

      /** Clear the buffer */
      void clear(); 
      
      virtual size_t write(uint8_t);
      virtual int availableForWrite(void);
      
      virtual int available();
      //virtual bool contains(char);
      virtual int read();
      virtual int peek();
      virtual void flush();

      void cache_payload(char * payload); 
};

extern StreamToWebsocket streamToWebsocket;