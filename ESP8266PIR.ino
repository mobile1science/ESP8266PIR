  /**
 * WifiPir.ino
 *
 *  Created on: 21.03.2017
 *
 *  Simple test to discover working of PIR connected to D1
 *  and sending status to Domoticz
 *  
 *  This version only connects to wifi when necessary
 *  
 *  v 1.00  
 */

//#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>
extern "C" {
#include "user_interface.h"
}

#define debug
#define EEPROM_TIMER_ADDR 0
#define EEPROM_PIRSTATE_ADDR (EEPROM_TIMER_ADDR+1)
#define TIMERTIMEOUT 240
HTTPClient http;
int httpCode;


#include "settings.h"

// Time to sleep (in seconds):
const int sleepTimeS = 10;

// WiFi settings
char ssid[] = SSID;             // enter your credentials
char password[] = PASSWORD;     //   to log in

// Domoticz settings
char connectionType[] = CONTYPE;// change to 'https' when secure connection (not tested on https)
char hostIP[] = DOMIP;         // enter ip or domain
int  hostPort = DOMPORT;       // enter appropriate port number
int  pirIdx   = PIRIDX;         // Pir number in Domoticz

// ESP settings
char espName[] = ESPNAME;

// constants won't change. They're used here to

// https://things4u.github.io/HardwareGuide/Single-Channel-Gway/images/d1-mini-esp8266-board-sh_fixled.jpg
// set pin numbers:
const int pirPin = 5;     // the number of the pir pin (D1)
const int ledPin = 13;    // number of the led pin (D7)
// variables will change:
int pirState = 0;         // variable for reading the pushbutton status
int timer = 0;
char url[255];

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  // initialize pirPin as Input
  pinMode(pirPin, INPUT);
  
  Serial.begin(115200);
  // Serial.setDebugOutput(true);

          Serial.println("Connecting Wifi...");
  
          // config static IP
          IPAddress ip(192, 168, 0, 102); // where xx is the desired IP Address
          IPAddress gateway(192, 168, 0, 1); // set gateway to match your network
          IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network
          WiFi.config(ip, gateway, subnet);
          WiFi.mode(WIFI_STA);
        
          wifi_station_set_hostname((char *) espName);
          WiFi.begin(ssid, password);


  Serial.println();
  Serial.println();
  Serial.println();

  // read the last saved state of the timer
  EEPROM.begin(512);
  timer = EEPROM.read(EEPROM_TIMER_ADDR);
  Serial.print("timer: ");
  Serial.println(timer);

  // read the state of the pirPin value:
  pirState = digitalRead(pirPin) || EEPROM.read(EEPROM_PIRSTATE_ADDR); // force On if previous detection was On but not send
  Serial.print("Sensor: ");
  Serial.println(pirState);

  Serial.print("Crash Sensor: ");
  Serial.println(EEPROM.read(EEPROM_PIRSTATE_ADDR));

   if ( pirState == HIGH || timer == TIMERTIMEOUT ) {
          
          // wait for WiFi connection
          int countdown=20; // about countdown/2 seconds          
          while (( WiFi.status() != WL_CONNECTED)) {
            delay(500);
            if ( --countdown == 0 ) {
              if ( pirState ) {
                  EEPROM.write(EEPROM_PIRSTATE_ADDR, pirState);
                  EEPROM.commit();
              }
              Serial.println("sleep forced :(");
              ESP.deepSleep(sleepTimeS*1000000/10);
              
            }
          }
              
          Serial.print("IP address:");
          Serial.println(WiFi.localIP());
          Serial.println(( pirState ? "Pir On" : "Pir Off" ));
  
          switchLight( pirIdx, ( pirState ? (char*) "On" : (char*) "Off" ) );
          pirState = 0; // set to 0, so next time it is not forced to send state.
          timer = -1;
  }
      
  // save counter
  Serial.println("save counter");
  EEPROM.write(EEPROM_TIMER_ADDR, ++timer);  
  EEPROM.write(EEPROM_PIRSTATE_ADDR, pirState);
  EEPROM.commit();
 
        
  Serial.println("sleep");
  ESP.deepSleep(sleepTimeS*1000000);



 
}

void buildConnectionURL(char* conType, char* conIP, int conPort)
{
  sprintf(url, "%s://%s:%d", conType, conIP, conPort);
}

void switchLight(int switchIdx, char* switchCmd)
{
  char urlcmd[255];

  // build the connection string
  buildConnectionURL(connectionType, hostIP, hostPort);

  
  sprintf(urlcmd, "http://%s:%s@%s:%d/json.htm?type=command&param=switchlight&idx=%d&switchcmd=%s", DOMUSER, DOMPASS, DOMIP, DOMPORT, switchIdx, switchCmd);
//  Serial.println(urlcmd);
  http.begin(urlcmd);
  httpCode = http.GET();
  http.end();
}

void loop() {

}

