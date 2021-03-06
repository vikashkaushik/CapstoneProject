/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/Vikash/Downloads/CapstoneProject/src/CapstoneProject.ino"
/*

 * Description:
 * Author: Vikash Kaushik
 * Date: 7/12/21
 */
void callback(char* topic, byte* payload, unsigned int length);
void setup();
void loop();
#line 7 "c:/Users/Vikash/Downloads/CapstoneProject/src/CapstoneProject.ino"
SYSTEM_THREAD(ENABLED);
#include "MQTT.h"
#include <blynk.h>
MQTT client("lab.thewcl.com", 1883, callback);
#include "oled-wing-adafruit.h"
OledWingAdafruit display;
void callback(char *topic, byte *payload, unsigned int length);
void callback(char* topic, byte* payload, unsigned int length)
{
  
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  Serial.printf("%s",p);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(p);
  display.display();
  delay(1000);
  Serial.println();
}  
// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  display.setup();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  
  display.display();
  Serial.begin(9600);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  display.loop();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  if (client.isConnected()) {
    client.loop();
    client.subscribe("madlibs");
  } 
  else {
    client.connect(System.deviceID());
    client.subscribe("madlibs");
    
  }
  
  
}
