// Library
//#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>


// #define debug                      // uncomment to debug

// WiFi settings
const char* ssid = "TeamDito";        // enter your credentials
const char* password = "idemdito";    //   to log in

// Host settings
const char* host = "teamdito.tk";     // enter ip or domain
#define     httpPort  80            // enter appropriate port number
const char* connectionType = "http";  // change to 'https' when secure connection
int httpCode;
char url[255];

// switch settings
#define   switchIdx   160          // change to switchidx of Domoticz

// Time to sleep (in seconds):
const int sleepTimeS = 10;



void setup() 
{

#ifdef debug  
  // initialize Serial
  Serial.begin(115200);
  Serial.println("ESP8266 in normal mode");
#endif
  
  // connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef debug
    Serial.print(".");
#endif
  }
#ifdef debug
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Print the IP address
  Serial.println(WiFi.localIP());
#endif  

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
#ifdef debug
    Serial.println("connection failed");
#endif
    return;
  }

  // build the url string
  sprintf(url,"%s://%s:%d/json.htm?type=command&param=switchlight&idx=%d&switchcmd=%s",
    connectionType, host, httpPort, switchIdx, "On");

  // setup the client and send url
  HTTPClient http;
  http.begin(url);
  // start connection and send HTTP header
  httpCode = http.GET();

  // wait a while for Domoticz server to process the on command
  delay(5000);

  // build the url string
  sprintf(url,"%s://%s:%d/json.htm?type=command&param=switchlight&idx=%d&switchcmd=%s",
    connectionType, host, httpPort, switchIdx, "Off");
  // start connection and send HTTP header
  http.begin(url);
  httpCode = http.GET();

  // send url
  http.begin(url);

  
  
  // go back to sleep
#ifdef debug
  Serial.println("ESP8266 in sleep mode");
#endif
  ESP.deepSleep(sleepTimeS * 1000000);
  
}

void loop() 
{
// added this comment
}
