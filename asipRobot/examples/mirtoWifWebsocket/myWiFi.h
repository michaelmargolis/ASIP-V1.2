// myWiFi.h

#ifdef _UNO_WIFI_R2_
#include <WiFiNINA.h>
#elif defined _UNO_WIFI_R4_
#include <WiFiS3.h>
#else
#include <WiFi.h>
#endif
// edit mirto_secrets.h with ssid and pw if mirto is a client
#include "mirto_secrets.h"
#include <EEPROM.h> // for storing changes in SSID and password 

#include "tunes.h" // for melody player
PlayMelody player(tonePin);  // just for some music while connecting to WiFi

char ssid[32] = SECRET_SSID;      // your network SSID (name)
char password[32] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

const char *MAGIC = "$$|";
const int SSID_START = strlen(MAGIC);
const int PW_START = SSID_START+32;

typedef void(*t_showFunc)(const char *, const char *);
static t_showFunc showFunc = NULL;  // callback to display ip on lcd

char *ipToStr(IPAddress ip) {
  static char buffer[16];
  char *pos = buffer;
  char octet[3];  // holds chars for a single octet
  for (int i = 0; i < 4; i++) {
    int num = ip[i];
    if (num == 0) *pos++ = '0';
    else {
      int j = 0;
      while (num != 0) {
        octet[j] = (num % 10) + '0';
        num = num / 10;
        j++;
      }
      while (j-- > 0)
        *pos++ = octet[j];  // reverse the digits
    }
    if (i < 3) *pos++ = '.';
  }
  *pos = '\0';
  return buffer;
}

class MyWiFi {
  private:
    int tonePin;
    unsigned long startTime;
    unsigned long waitTime; 

  public:
    MyWiFi (int tonepin) {
      tonePin = tonepin; 
    }

    void begin(t_showFunc func ) {
      showFunc = func;
      #if defined(TARGET_RP2040)
      EEPROM.begin(256);
      #endif
      if(!checkStoredSsidPw()) {
        saveSsidPw();
      }

      #if defined _PICO2040_ || defined _ESP32_
        WiFi.mode(WIFI_STA); // needed for esp and pico
      #endif
        showFunc("Connecting to ", ssid);
        startTime = millis();
        waitTime = 5000; 
        WiFi.begin(ssid, password);
        player.play(starwars, 108);
    }

    bool connect() {      
        if(WiFi.status() == WL_CONNECTED) {
          Serial.print("!Mirto is connected to WiFi network ");  Serial.println(WiFi.SSID());
          Serial.print("Assigned IP Address: ");  Serial.println(WiFi.localIP());
          showFunc(ssid, ipToStr(WiFi.localIP()));
          while( player.service(SCORE_SECTION) ); // play to end of section
          return true;
        }
        else {
          player.service(SCORE_END);
          if(millis() - startTime > waitTime) {
            WiFi.end();
            player.service(SCORE_END);
            delay(10);
            WiFi.begin(ssid, password);
            waitTime += 10000;
          }
        }
        return false;
    }
    
    static void saveSsidPw(){
      for(int i=0; i < strlen(MAGIC); i++)
        EEPROM.write(i, MAGIC[i]);
      for(int i=0; i < 32; i++){
        EEPROM.write(i+SSID_START, ssid[i]);
        EEPROM.write(i+PW_START, password[i]);
      } 
      #if defined(TARGET_RP2040)
      if (EEPROM.commit()) {
        Serial.println("EEPROM successfully committed");
      } else {
        Serial.println("ERROR! EEPROM commit failed");
      }
      #endif
    }

    bool checkStoredSsidPw(){
        Serial.println("checking eeprom");
        for(int i=0; i < strlen(MAGIC); i++){
          Serial.print((char) EEPROM.read(i)) ;
          if( EEPROM.read(i) !=  MAGIC[i])
            return false;
        }
        for(int i=0; i< 32; i++){
          ssid[i] = EEPROM.read(i+SSID_START);
          password[i] = EEPROM.read(i+PW_START);
        }
        Serial.print("got stored ssid and pw for");
        Serial.println(ssid);
        return true;
    }

    static void configCallback(){
    // check for changed wifi credentials at startup
      if(Serial.available()>=3){
        // check for: "$|" (leading $ has been found by caller)  
        if(Serial.read() == '$' &&  Serial.read() == '|'){
           memset(ssid, 0, 32);
           memset(password, 0, 32);
           int ssdCount = Serial.readBytesUntil('|', ssid, 31);
           if(ssdCount > 1){
             int pwCount =  Serial.readBytesUntil('\n', password, 31);
             if(pwCount > 1){
               Serial.print("!Got updated ssid and pw for ");
               Serial.println(ssid);
               saveSsidPw();
               WiFi.begin(ssid, password);
               showFunc("Connecting to ", ssid);              
             }
           }  
         }
      } 
    }

};