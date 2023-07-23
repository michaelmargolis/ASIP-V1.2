/*
  StreamToWebsocket.h
  This class pipes an Arduino stream to a websocket
*/

#include "StreamToWebsocket.h"

#define debugPrintf // Serial.printf

StreamToWebsocket streamToWebsocket = StreamToWebsocket();  // the class instance

static WebSocketsServer webSocket = WebSocketsServer(9006);  // the websocket
static uint8_t gClientId = 0;

void webSocketEvent(uint8_t clientId, WStype_t type, uint8_t* payload, size_t length) {
  gClientId = clientId;
  switch (type) {
    case WStype_DISCONNECTED:
      debugPrintf("![%u] Disconnected!\n", clientId);
      streamToWebsocket.is_connected = false;
      streamToWebsocket.conEvent(false);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(clientId);
        debugPrintf("![%u] Connected from %d.%d.%d.%d url: %s\n", clientId, ip[0], ip[1], ip[2], ip[3], payload);
  
        streamToWebsocket.is_connected = true;
        streamToWebsocket.conEvent(true);
      }
      break;

    case WStype_TEXT:
      streamToWebsocket.cache_payload((char*)payload);
      debugPrintf("![%u] get Text: %s\n", clientId, payload);
      break;

    case WStype_BIN:
      break;
  }
}

StreamToWebsocket::StreamToWebsocket(uint16_t inbuf_size, uint16_t outbuf_size) {
  this->in_buffer = (uint8_t*)malloc(inbuf_size);
  this->inbuf_size = inbuf_size;
  this->in_head = this->in_tail = 0;
  this->out_buffer = (uint8_t*)malloc(outbuf_size);
  this->outbuf_size = outbuf_size;
  this->out_pos = 0;
  this->is_connected = false;
  registerCallback(this->defaultEvtHandler);
}


StreamToWebsocket::~StreamToWebsocket() {
  free(this->in_buffer);
  free(this->out_buffer);
}

void StreamToWebsocket::registerCallback(ConnectionEvent cb) {
  conEvent = cb;
}

void StreamToWebsocket::defaultEvtHandler(bool isConnected) {
  if ( isConnected) {
    debugPrintf("! Client connected\n");
    //Serial.println(webSocket.remoteIP(gClientId));
  } else {
    debugPrintf("! Client disconnected");
  }
}

IPAddress StreamToWebsocket::remoteIP(){
  #if !defined(ARDUINO_UNOWIFIR4) 
   return webSocket.remoteIP(gClientId);
  #else
    return " "; // Todo: remote Ip not supported in current websocket lib 
  #endif  
}

void StreamToWebsocket::clear() {
  this->in_head = this->in_tail = 0;
  this->out_pos = 0;
}

void StreamToWebsocket::flush() {
  this->in_head = this->in_tail = 0;
  this->out_pos = 0;
}


int StreamToWebsocket::read() {

  uint8_t c, i;

  if (in_head == in_tail) return -1;
  i = in_tail + 1;
  if (i >= inbuf_size) i = 0;
  c = in_buffer[i];
  in_tail = i;
  return c;
}

size_t StreamToWebsocket::write(uint8_t v) {
  if (streamToWebsocket.is_connected) {
    if (out_pos == outbuf_size - 1) {
      // data lost if here, figure out how to handle, perhaps send fragment
      Serial.println("! Out buffer full");
      return 0;
    } else {
      out_buffer[out_pos++] = v;
      if (v == '\n') {
        out_buffer[out_pos] = '\0';
        out_pos = 0;
        debugPrintf("!sending: %s to client %d\n", (char*)out_buffer, gClientId);
        webSocket.sendTXT(gClientId, out_buffer);
        webSocket.loop();
      }
      return 1;
    }
  } else
    return 0;  // not connected
}

int StreamToWebsocket::available() {
  uint8_t head, tail;

  head = in_head;
  tail = in_tail;
  if (head >= tail) return head - tail;
  return inbuf_size + head - tail;
}

int StreamToWebsocket::availableForWrite() {
  return outbuf_size - out_pos - 1;
}

/*
bool StreamToWebsocket::contains(char ch) {
  for (int i = 0; i < inbuf_size; i++) {
    int p = (in_pos + i) % inbuf_size;
    if (in_buffer[p] == ch) {
      return true;
    }
  }
  return false;
}
*/

int StreamToWebsocket::peek() {
  uint8_t head, tail;

  head = in_head;
  tail = in_tail;
  if (head == tail) return -1;
  if (++tail >= inbuf_size) tail = 0;
  return in_buffer[tail];
}

/* ----------- websocket code  ----------*/

void StreamToWebsocket::begin() {
  // call this after connecting to WiFi
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void StreamToWebsocket::service() {
  webSocket.loop();
}

void StreamToWebsocket::cache_payload(char* payload) {
  for (int i = 0; i < strlen(payload); i++) {
    uint8_t pos = in_head + 1;
    if (pos >= inbuf_size) pos = 0;
    if (pos != in_tail) {
      in_buffer[pos] = payload[i];
      in_head = pos;
    }
  }
}
