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

boolean viewChoices = true;
boolean answerSubmitted = false;

String playerID = "3";
String channel = "madlibs";

//count votes
int count1 = 0;
int count2 = 0;
int count3 = 0;

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
  ASSUMPTION - User output:
  1. 1
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
  else if(length == 1 && p[0] == '1'){
    choiceToDisplay1 = p;
    count1++;
  }
  else if(length == 1 && p[0] == '2'){
    choiceToDisplay2 = p;
    count2++;
  }
  else if(length == 1 && p[0] == '3'){
    choiceToDisplay3 = p;
    count3++;
  }  else{
    wordType = p;
    count1 = 0;
    count2 = 0;
    count3 = 0;
    choiceToDisplay1 = "";
    choiceToDisplay2 = "";
    choiceToDisplay3 = "";
    viewChoices = true;
    answerSubmitted = false;
  }

  //LED logic. only triggers if all three COUNT messages have been sent.
  if (count3 >= 0){
    if (count3 >= count1){
      if (count3 >= count2){
        analogWrite(A5, 255); //tied for first

      }
      else {
        analogWrite(A5, 127); //second
      }
    }

    else{
      if (count3 >= count2){
        analogWrite(A5, 127); //second
      }
      else{
        analogWrite(A5, 1); //last
      }
    }
  }

  if (viewChoices == true){
    viewChoiceDisplay();
  }
  else{
    viewCountDisplay();
  }
}  
// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  pinMode(A5, OUTPUT);
  pinMode(D2, INPUT);
  pinMode(D3, INPUT);
  pinMode(D4, INPUT);

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

  } 
  else {
    client.connect(System.deviceID());
    client.subscribe(channel);
  }

  //button input
  if (display.pressedA()){
    viewChoices = true;
    viewChoiceDisplay();
  }

  else if (display.pressedC()){
    viewChoices = false;
    viewCountDisplay();
  }
  
  //send to madlib
  if (digitalRead(A2) == HIGH && answerSubmitted == false){
    answerSubmitted = true;
    madlibSend("1");
  }

  else if (digitalRead(A3) == HIGH && answerSubmitted == false){
    answerSubmitted = true;
    madlibSend("2");
  }

  else if (digitalRead(A4) == HIGH && answerSubmitted == false){
    answerSubmitted = true;
    madlibSend("3");
  }


}

void viewChoiceDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(wordType);
  display.println(choiceToDisplay1);
  display.println(choiceToDisplay2);
  display.println(choiceToDisplay3);
  display.display();
}

void viewCountDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(wordType);
  display.print("Choice 1: ");
  display.println(count1);
  display.print("Choice 2: ");
  display.println(count2);
  display.println("Choice 3: ");
  display.println(count3);
  display.display();
}
// function - sends OUTPUT to "madlibs"
void madlibSend(String output){
    client.publish(channel, output);
}

BLYNK_WRITE(V1){
// uses blynk to write messages
  String inputText = param.asString();
  String outputText =  playerID + "." + inputText;
  madlibSend(outputText);
  choiceToDisplay3 = inputText;
  /*output format:
  1. <INPUT>
  */
}