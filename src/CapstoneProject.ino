/*TO-DO
#      account for multiple questions (basically just IF statements from here)
#      bluetooth?
#      
*/
//                                                                      ##### BEGIN CODE #####

SYSTEM_THREAD(ENABLED);
#include "MQTT.h"
#include <blynk.h>
MQTT client("lab.thewcl.com", 1883, callback);

#include "oled-wing-adafruit.h"
OledWingAdafruit display;

String choiceToDisplay1;  // answer choice 1
String choiceToDisplay2;  // answer choice 2
String choiceToDisplay3;  // answer choice 3
String wordType;          // Word type

String playerID = "1";

int count1;
int count2;
int count3;

void callback(char *topic, byte *payload, unsigned int length);
void callback(char* topic, byte* payload, unsigned int length)
{
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;

//finds input if begins with 1
/*ASSUMPTION - Format:
  1.<INPUT>
  2.<INPUT
  3.<INPUT>
*/

  if(length>1 && p[0] == '1'){
    choiceToDisplay1 = p;
  }
  else if(length>1 && p[0] == '2'){
    choiceToDisplay2 = p;
  }
  else if(length>1 && p[0] == '3'){
      choiceToDisplay3 = p;
  }
  else {
    wordType = p;
  }
    

//prints list of choices
  Serial.printf("%s",p);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(wordType);
  display.println(choiceToDisplay1);
  display.println(choiceToDisplay2);
  display.println(choiceToDisplay3);
  display.display();
  Serial.println();

/*
ASSUMPTION: Formatted in:
  COUNT1:<NUM> /end
  COUNT2:<NUM> /end
  COUNT3:<NUM /end
*/
  if (p[5] == '1'){
    count1 = p[7] - '0';
    Serial.println(count1);
  }
  else if (p[5] == '2'){
    count2 = p[7] - '0';
        Serial.println(count2);

  }
  else if (p[5] == '3'){
    count3 = p[7] - '0';
        Serial.println(count3);

  }

  //LED logic. only triggers if all three COUNT messages have been sent.
  if (count3 >= 0){
    if (count1 >= count3){
      if (count1 >= count2){
        analogWrite(A5, 255); //tied for first

      }
      else {
        analogWrite(A5, 127); //second
      }
    }

    else{
      if (count1 >= count2){
        analogWrite(A5, 127); //second
      }
      else{
        analogWrite(A5, 1); //last
      }
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print("Choice 1: ");
    display.println(count1);
    display.print("Choice 2: ");
    display.println(count2);
    display.print("Choice 3: ");
    display.println(count3);
    display.display();
    Serial.println();
  }
}  
// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  pinMode(A5, OUTPUT);
  display.setup();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  
  display.display();
  Serial.begin(9600);

  Blynk.begin("_XeCkFwjobmuT88r_NK86K2oypTbPhYB", IPAddress(167, 172, 234, 162), 9090);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.

  Blynk.run();
  display.loop();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  if (client.isConnected()) {
    client.loop();
    client.subscribe("madlibscount");

  } 
  else {
    client.connect(System.deviceID());
    client.subscribe("madlibscount");
  }

//button input
  if (display.pressedA()){
    madlibSend("1");
  }

  else if (display.pressedB()){
    madlibSend("2");
  }

  else if (display.pressedC()){
    madlibSend("3");
  }
  

}

// function - sends OUTPUT to "madlibs"
void madlibSend(String output){
    client.subscribe("madlibs");
    client.publish("madlibs", output);
    client.unsubscribe("madlibs");
}

BLYNK_WRITE(V1){
// uses blynk to write messages
  String inputText = param.asString();
  String outputText =  playerID + ". " + inputText;
  madlibSend(outputText);
  /*output format:
  1. <INPUT>
  */
}