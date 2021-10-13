/*
  Attempt at making connection between specified port and ESP8266
*/

#include <ESP8266WiFi.h>
#include <FastLED.h>

#define SendKey 0 //Button to send data flash BTN on NodeMCU

#define NUM_LEDS 360 //Set number of LEDs in strip/strand
#define DATA_PIN 4

int port = 8888; //Port #
WiFiServer server(port);

//Connect server to WiFi
const char *ssid = ""; //Network name
const char *password = ""; //Network pass

//Global variables
CRGB leds[NUM_LEDS];
String fullMsg;
char received;

void setup() {
  Serial.begin(9600);
  pinMode(SendKey,INPUT_PULLUP);  //Btn to send data
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); //Connect to wifi
 
  // Wait for connection  
  Serial.println("Connecting to Wifi");
  while (WiFi.status() != WL_CONNECTED) {   
    delay(500);
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
  server.begin();
  Serial.print("Open Telnet and connect to IP:");
  Serial.print(WiFi.localIP());
  Serial.print(" on port ");
  Serial.println(port);
  
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop() {
    WiFiClient client = server.available();
  
  if (client) {
    if(client.connected())
    {
      Serial.println("Client Connected");
    }
    
    while(client.connected()){      
      while(client.available()>0){
        // Read data from the connected client
        received = client.read();
        while (received != ';') {
          fullMsg += received;
          received = client.read();
          Serial.write(received); 
          // If statements were here ******
        }
        if (fullMsg.equals("0")){
            Serial.write("Received a 0! Turning lights off...");
            turnOff();
          }
          else if (fullMsg.equals("1")){
            Serial.write("Received a 1! Turning lights red...");
            fullColor('r');
          }
          else if (fullMsg.equals("2")){
            Serial.write("Received a 2! Turning lights blue...");
            fullColor('b');
          }
          else if (fullMsg.equals("3")){
            Serial.write("Received a 3! Turning lights green...");
            fullColor('g');
          }
          else if (fullMsg.startsWith("f")) {
            Serial.write("Received a hex color.");
            handleHexColor(fullMsg);
          }
        //Send Data to connected client
        while(Serial.available()>0)
        {
          client.write(Serial.read());
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected");    
  }
}

void turnOff() {
  FastLED.showColor(CRGB(0,0,0));
}

void fullColor(char color) {
  switch (color) {
    case 'r':
      FastLED.showColor(CRGB(200,0,0));
      break;
    case 'g':
      FastLED.showColor(CRGB(0,200,0));
      break;
    case 'b':
      FastLED.showColor(CRGB(0,0,200));
      break;
  }
}

void handleHexColor(String hex) {
  String colorString = hex.substring(2);
  long color = strtol(colorString.c_str(), NULL, 16);
  
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
}
