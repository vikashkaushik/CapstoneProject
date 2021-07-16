/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/evanh/OneDrive/Desktop/CTD/InternetOfThings/CapstoneProject/src/CapstoneProject.ino"
/*TO-DO
#      bluetooth?
#      
*/
//                                                                      ##### BEGIN CODE #####

void callback(char* topic, byte* payload, unsigned int length);
void setup();
void loop();
void viewChoiceDisplay();
void viewCountDisplay();
void madlibSend(String output);
#line 7 "c:/Users/evanh/OneDrive/Desktop/CTD/InternetOfThings/CapstoneProject/src/CapstoneProject.ino"
SYSTEM_THREAD(ENABLED);
#include "MQTT.h" //libraries: MQTT
#include <blynk.h> //libraries: blynk

MQTT client("lab.thewcl.com", 1883, callback); 

#include "oled-wing-adafruit.h" //libraries: oled-wing-adafruit
OledWingAdafruit display;
String wordType;          // Word type



boolean viewChoices = true;
boolean answerSubmitted = false;
boolean isRunning = true;

//                                                                              ## CHANGE ##
String playerString = "1";
int playerID = 1; //change this as necessary


String channel = "madlibs";

//count votes
int countList[3] = {0, 0, 0};

String choiceList[3] = {"", "", ""};

void callback(char *topic, byte *payload, unsigned int length);
void callback(char* topic, byte* payload, unsigned int length)
{
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String s = p;

//finds input if begins with 1
/*ASSUMPTION - Format:
  1.<INPUT>
  2.<INPUT
  3.<INPUT>
  ASSUMPTION - User output:
  1. 1
*/
  if(s.equals("PAUSED")){
      isRunning = false;
  }
  else if(s.equals("UNPAUSED")){
      isRunning = true;
  }
  else if (isRunning == true){
    if(length>1 && p[0] == '1'){
      choiceList[0] = p;
    }
    else if(length>1 && p[0] == '2'){
      choiceList[1] = p;
    }
    else if(length>1 && p[0] == '3'){
      choiceList[2] = p;
    }  
    else if(length == 1 && p[0] == '1'){
      countList[0]++;
    }
    else if(length == 1 && p[0] == '2'){
      countList[1]++;
    }
    else if(length == 1 && p[0] == '3'){
      countList[2]++;
    }  
    else if (s.equals("UNPAUSED")){
      isRunning = true;
    }
    else{
      wordType = p;
      countList[0] = 0;
      countList[1] = 0;
      countList[2] = 0;
      choiceList[0] = "";
      choiceList[1] = "";
      choiceList[2] = "";
      viewChoices = true;
      answerSubmitted = false;
    }
  }
  

  //LED logic. only triggers if all three COUNT messages have been sent.

  if (countList[0] == 0 && countList[1] == 0 && countList[2] == 0){
    analogWrite(A5, 0);
  }
  else {
    if (countList[(playerID+2)%3] >= countList[(playerID)%3]){
      // if 2 > 1
      if (countList[(playerID+2)%3] >= countList[(playerID + 1)%3]){
        // and 2 > 3
        analogWrite(A5, 255); //tied for first

      }
      else {
        analogWrite(A5, 90); //second
      }
    }

    else{
      if (countList[(playerID+2) % 3] >= countList[(playerID+1)%3]){
        // if 2 > 3
        analogWrite(A5, 90); //second
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
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  pinMode(D7, INPUT);




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

  if (isRunning == true){
    //send to madlib
    if (digitalRead(D5) == HIGH && answerSubmitted == false){
      answerSubmitted = true;
      madlibSend("1");
    }

    else if (digitalRead(D6) == HIGH && answerSubmitted == false){
      answerSubmitted = true;
      madlibSend("2");
    }

    else if (digitalRead(D7) == HIGH && answerSubmitted == false){
      answerSubmitted = true;
      madlibSend("3");
    }
  
  }
}

void viewChoiceDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(wordType);
  display.println(choiceList[0]);
  display.println(choiceList[1]);
  display.println(choiceList[2]);
  display.display();
}

void viewCountDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Choice 1: ");
  display.println(countList[0]);
  display.print("Choice 2: ");
  display.println(countList[1]);
  display.print("Choice 3: ");
  display.println(countList[2]);
  display.display();
}
// function - sends OUTPUT to "madlibs"
void madlibSend(String output){
    client.publish(channel, output);
}

BLYNK_WRITE(V1){
// uses blynk to write messages
  if(isRunning){
    String inputText = param.asString();
    String outputText =  playerString + "." + inputText;
    madlibSend(outputText);
    choiceList[playerID-1] = inputText;
  }
  /*output format:
  1. <INPUT>
  */
}