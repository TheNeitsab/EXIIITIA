// ================================================================
// ===                      ARDUINO CODE                        ===
// ===                        EXIIITIA                          ===
// ================================================================

// ================================================================
// ===                      DESCRIPTION                         ===
// ================================================================

/*
 * Arduino micro code for Exiiitia.
 * Inspired by the one created by exiii Inc.

 * Copyright (c) 2018 Bastien GABRIELLI

 * Exiiitia is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, 
 * or (at your option) any later version.

 * Exiiitia is distributed in the hope that it will be useful,but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

// ================================================================
// ===                    GLOBAL DEFINITIONS                    ===
// ================================================================

// ==========================  INCLUDES  ==========================

#include <VarSpeedServo.h>    //library managing servos' behavior
#include <SoftwareSerial.h>   //Used to emulate Serial ports on other pins

// =====================  SOFTWARE SETTINGS  ======================

//shifumi game switch values
#define   ROCK      1
#define   PAPER     2
#define   SCISSORS  3

//state checking booleans
//const settings
const boolean   isRight     =   0;    //arm's/hand's position => right:1 | left:0
const boolean   onSerial    =   1;    //enabling Serial command
//variables
boolean   isThumbOpen   =   1;
boolean   isOtherLock   =   0;

//out position handling values
//const settings
const int   outThumbMax   =   34;     //right:open | left:close
const int   outThumbMin   =   135;    //right:close | left:open
const int   outIndexMax   =   140;    //right:open | left:close
const int   outIndexMin   =   30;     //right:close | left:open
const int   outOtherMax   =   150;    //right:open | left:close
const int   outOtherMin   =   100;    //right:close | left:open
//variables
int   outThumb, outIndex, outOther  =   90;  
int   outThumbOpen, outThumbClose, 
      outIndexOpen, outIndexClose, 
      outOtherOpen, outOtherClose;
      
//speed and position values 
//const settings      
const int   positionMax   =   160;  //upper bound 
const int   positionMin   =   20;   //lower bound 
//variables
int   position      =   20;         //current position
int   prePosition   =   20;         //previous position

//speed handling values
//const settings
const int   speedMax    =   150;    //upper bound
const int   speedMin    =   60;     //lower bound
//variables
int   speed         =   10;         //current speed

//sensor read value and borders
//variables
int   sensorValue   =   0;          //current sensor value                             
int   sensorMax     =   0;          //upper bound                    
int   sensorMin     =   1024;       //lower bound

//communication data for vocal recognition module
byte   com  =  0;

//inactivity checking parameters
int   thresholdInact   =   50;      //position threshlod
int   countInact       =    0;      //inactivity counter

// =====================  HARDWARE SETTINGS  ======================
//servos' definition
VarSpeedServo   servoIndex;   //index finger
VarSpeedServo   servoOther;   //other three fingers
VarSpeedServo   servoThumb;   //thumb

//setting vocal module serial communication : RX, TX on pin 3, 4
SoftwareSerial   mySerial(3, 4);    

//digital pins for RGB components of RGB LED
int const   pinR   =   12;    //RED          
int const   pinG   =   13;    //GREEN       
int const   pinB   =   9;     //BLUE
     
//digital pins for pushbuttons
int   pinCalib;    //enable calibration
int   pinThumb;    //open/close thumb
int   pinOther;    //(un)lock other three fingers

//pushbutton troubleshooting counters
int   countCalib, countThumb, countOther, countGame = 0;

//sensor's input pin
int   pinSensor = A0;              

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup(){
  Serial.begin(9600);     //initializing Arduino IDE's Serial
  mySerial.begin(9600);   //initializing Vocal Module's Serial

  servoIndex.attach(2);   //index servo
  servoOther.attach(5);   //other servo
  servoThumb.attach(6);   //thumb servo
  
// ===================  PINS & OUT DEFINITIONS  ===================
  if(isRight){
    pinCalib   =   A6;
    pinThumb   =   A4;
    pinOther   =   A3;
    
    outThumbOpen = outThumbMax; outThumbClose = outThumbMin;
    outIndexOpen = outIndexMax; outIndexClose = outIndexMin;
    outOtherOpen = outOtherMax; outOtherClose = outOtherMin;
  }
  else{
    pinCalib   =   11;
    pinThumb   =   8;
    pinOther   =   7;
    
    outThumbOpen = outThumbMin; outThumbClose = outThumbMax;
    outIndexOpen = outIndexMin; outIndexClose = outIndexMax;
    outOtherOpen = outOtherMin; outOtherClose = outOtherMax;
  }

  pinMode(pinCalib, INPUT);     
  digitalWrite(pinCalib, HIGH);
  pinMode(pinThumb, INPUT);     
  digitalWrite(pinThumb, HIGH);
  pinMode(pinOther, INPUT);     
  digitalWrite(pinOther, HIGH);
  
  pinMode(pinR, OUTPUT);        
  pinMode(pinG, OUTPUT);        
  pinMode(pinB, OUTPUT);    
  
// ===================  VOICE MODULE INIT  ===================
  if(onSerial) Serial.println("Starting Setup");
  //Setting the voice module in Compact Mode
  mySerial.write(0xAA);
  mySerial.write(0x37);
  if(onSerial) Serial.println("Compact Mode OK");
  
  delay(2000);
  
  //Importing Group1 vocal commands
  mySerial.write(0xAA);
  mySerial.write(0x21);
  if(onSerial) Serial.println("Group 1 imported");
  if(onSerial) Serial.println("Setup is done !");
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
// ========================  CALIBRATION  =========================
  if(onSerial) Serial.println("======Waiting for Calibration======");
  while(1){
    //holding open position waiting for calibration
    servoIndex.write(outIndexOpen);
    servoOther.write(outOtherOpen);
    servoThumb.write(outThumbOpen);
    
    if(onSerial) serialMonitor();
    delay(10);
    
    if(digitalRead(pinCalib) == LOW){
      calibration();
      break;
    }
  }
   
// =======================  MANUAL CONTROLS  ======================
  //buttons state checking
  while(1){
    if(digitalRead(pinCalib) == LOW) countCalib += 1;
    else countCalib = 0;
    if(countCalib == 10) {
      countCalib   =   0;
      calibration();
    }
    if(digitalRead(pinThumb) == LOW) countThumb += 1;
    else countThumb = 0;
    if(countThumb == 10) {
      countThumb    =   0;
      isThumbOpen =   !isThumbOpen;
      while (digitalRead(pinThumb) == LOW) delay(1);
    }
    if(digitalRead(pinOther) == LOW) countOther += 1;
    else countOther   =   0;
    if(countOther == 10) {
      countOther    =   0;
      isOtherLock =   !isOtherLock;
      while(digitalRead(pinOther) == LOW) delay(1);
    }
    
// =====================  HAND'S BEHAVIOUR  =======================  
    sensorValue = readSensor();   //getting current sensor value
    delay(25);

    //borders overpass checking
    if(sensorValue < sensorMin) sensorValue = sensorMin;
    else if(sensorValue > sensorMax) sensorValue = sensorMax;
     
    sensorToPosition();   //converting sensor's value to position
    
    //moving index to position
    outIndex = map(position, positionMin, positionMax, outIndexOpen, outIndexClose);
    servoIndex.write(outIndex, speed);
    //moving others to position
    if(!isOtherLock){
      outOther = map(position, positionMin, positionMax, outOtherOpen, outOtherClose);
      servoOther.write(outOther, speed);
    }
    //moving thumb to position
    if(isThumbOpen)  servoThumb.write(outThumbOpen, 30); 
    else servoThumb.write(outThumbClose, 30);
    
    if(onSerial) serialMonitor();

    visualStrength();
    inactivity();    
    shifumiGame();
  }
}

// ================================================================
// ===                       FUNCTIONS                          ===
// ================================================================

// ======================  int readSensor()  ======================
// Description : getting sensor's value making an average over a 10
//               values buffer to avoid any flickering

// Parameters : OUT -> sval : returns the averaged value
int readSensor() {
  int i, sval;
  for(i = 0; i < 10; i++) {
    sval   +=   analogRead(pinSensor);
  }
  sval   =   sval/10;
  return sval;
}

// ==================  void sensorToPosition()  ===================
// Description : converting the sensor's value to the position one
//               according to speed's set thresholds and boundaries

// Parameters : NONE
void sensorToPosition(){
  position   =   map(sensorValue, sensorMin, sensorMax, positionMin, positionMax);

  speed = map(position, positionMin, positionMax, speedMin, speedMax);
}

// ====================  void calibration()  =====================
// Description : performing the hand's calibration according to
//               the set values in the GLOBAL DEFINITIONS and the
//               values got from the sensor. 
//               The ideal calibration procedure would be to do at
//               least 3 contractions and realeasing during the 
//               process.

// Parameters : NONE
void calibration() {
  if(onSerial) Serial.println("======calibration start======");
  colorLED('G');    //Lighting RGB LED in GREEN

  int count = 0;

  //starting the calibration process
  while(count < 250) {
    sensorValue = readSensor();   //getting current sensor value
    delay(25);

    //borders checking
    if(sensorValue < sensorMin){
      if(sensorValue >= 0) sensorMin = sensorValue;
    }
    else if(sensorValue > sensorMax){
      if(sensorValue <= 1024) sensorMax = sensorValue;
    }
    
    sensorToPosition();   //converting sensor's value to position

    //moving index to position
    outIndex   =   map(position, positionMin, positionMax, outIndexOpen, outIndexClose);    
    servoIndex.write(outIndex, 100);

    count++;
    if(onSerial) serialMonitor();
  }
  if(onSerial)  Serial.println("======calibration finish======");
}

// ===================  void serialMonitor()  ====================
// Description : troublehsooting/debugging function using Arduino
//               serial monitor.

// Parameters : NONE
void serialMonitor(){
  Serial.print("Min="); Serial.print(sensorMin);
  Serial.print(",Max="); Serial.print(sensorMax);
  Serial.print(",Sensor="); Serial.print(sensorValue);
  Serial.print(",Speed="); Serial.print(speed);
  Serial.print(",Position="); Serial.print(position);
  Serial.print(",OutIndex="); Serial.print(outIndex);
  Serial.print(",isThumbOpen="); Serial.print(isThumbOpen);
  Serial.print(",isOtherLock="); Serial.println(isOtherLock);
}

// ===============  void colorLED(char color)  ==================
/* Description : setting the RGB LED to the according color passed
 *               to the function.
*/
// Parameters : char color -> character representing the first
//                            letter of the color.

void colorLED(char color){
  switch(color){
    case 'R': //RED
      analogWrite(pinR, 254);
      analogWrite(pinG, 0);
      analogWrite(pinB, 0);
    break;
    case 'G': //GREEN
      analogWrite(pinR, 0);
      analogWrite(pinG, 254);
      analogWrite(pinB, 0);
    break;
    case 'B': //BLUE
      analogWrite(pinR, 0);
      analogWrite(pinG, 0);
      analogWrite(pinB, 254);
    break;
    case 'P': //PURPLE
      analogWrite(pinR, 254);
      analogWrite(pinG, 254);
      analogWrite(pinB, 254);
    break;
    case 'N': //NONE
      analogWrite(pinR, 0);
      analogWrite(pinG, 0);
      analogWrite(pinB, 0);
    break;
  }
}

// ================= void visualStrength()  ===================
/* Description : giving a visual feedback of tha amount of
 *               strength put on the sensor.
 *               Low force  => RED
 *               TO
 *               High force => GREEN
*/
// Parameters : NONE
void visualStrength(){
  int R,G,B = 0;
  
  R = map(sensorValue, sensorMin, sensorMax, 255, 0);
  G = map(sensorValue, sensorMin, sensorMax, 0, 255);

  analogWrite(pinR, R);
  analogWrite(pinG, G);
  analogWrite(pinB, 0); 
}

// ==================  void inactivity()  ====================
/* Description : checking for inactivity in order to detach
 *               and save servos' lifetime
*/
// Parameters : NONE
void inactivity(){
  if(sensorValue < sensorMin + thresholdInact) countInact += 1;
    else countInact   =   0;
    if(countInact == 300) {
      countInact    =   0;
      
      servoIndex.detach();         
      servoOther.detach();         
      servoThumb.detach();
      if(onSerial) Serial.println("Servos Detached");
      
      while(sensorValue < sensorMin + thresholdInact) sensorValue = readSensor();

      servoIndex.attach(2);         
      servoOther.attach(5);         
      servoThumb.attach(6);            
      if(onSerial) Serial.println("Servos Attached");
    }
}

// ===================  shifumiGame()  ====================
/* Description : implemented shifumi game doing randomly one
 *               of the 3 possible positions according to
 *               ROCK, PAPER or SCISSORS play.
*/
// Parameters : NONE
void shifumiGame(){

  if(digitalRead(10) == LOW) countGame += 1;
    else countGame = 0;
    if(countGame == 10) {
      countGame    =   0;
      colorLED('P');

      if(onSerial)  Serial.println("Sarting SHIFUMI !!");
      int shifumi = random(1,4);

      switch(shifumi){
        case ROCK :
          if(onSerial)  Serial.println("ROCK");
          servoIndex.write(outIndexClose);
          servoOther.write(outOtherClose);
          servoThumb.write(outThumbClose);
          isThumbOpen =   !isThumbOpen;
          delay(15);                        
          break;
        case PAPER :
          if(onSerial)  Serial.println("PAPER");
          servoIndex.write(outIndexOpen);
          servoOther.write(outOtherOpen);
          servoThumb.write(outThumbOpen);
          delay(15);                        
          break;
        case SCISSORS :
          if(onSerial)  Serial.println("SCISSORS");
          servoIndex.write(outIndexOpen);
          servoOther.write(outOtherClose);
          servoThumb.write(outThumbOpen);
          delay(15);                        
          break;
      }
      delay(100); 
      while (digitalRead(10) == LOW) delay(1);  
         
      while (1){
        if(digitalRead(10) == LOW) countGame += 1;
        else countGame = 0;
        if(countGame == 10) {
          countGame    =   0;
          break;
        }
      }
      if(onSerial)  Serial.println("End of the game !");
    }
}

// ===================  void vocalBlocking()  =====================
/* Description : (un)blocking the hand thanks to vocal recognition.
 *               In order to initialize the blocking process, you
 *               must say "OK EXIII". This will automatically block 
 *               in current position for 3 seconds waiting for one 
 *               of the following available orders :
 *               - "BLOQUE OUVERT" : blocks the hand in OPEN position
 *               - "BLOQUE FERMÉ"  : blocks the hand in CLOSED position
 *               - "BLOQUE EN POSITION" : blocks the hand in current position
 *               After 3 seconds without any correct, the system is
 *               unblocked.
 *               In order to unblock it after blocking in one of
 *               the previous positions, you must say "DÉBLOQUE".
*/
// Parameters : NONE
void vocalBlocking(){
  //Checking if something has been received
  if (mySerial.available()){
    com   =   mySerial.read();              //Storing the received data
    
    //Checking for "OK EXIII" command
    if(com == 0x11){    
      colorLED('P');      //Lighting RGB LED in PURPLE
      int count = 0;
      //Checking if the interval is overpassed or if "DÉBLOQUE" has been said
      while((count < 200) && (com != 0x15)){
        count++;
        com   =   mySerial.read();

        switch(com){
          
          //Checking for "BLOQUE OUVERT" command
          case 0x12:  
            colorLED('N');                    //Switching the RGB LED OFF
            servoIndex.write(outIndexOpen);
            servoOther.write(outOtherOpen);
            servoThumb.write(outThumbOpen);
            delay(15);                        //Allow time for servo to change position
            blocking();      
          break;
          
          //Checking for "BLOQUE FERMÉ" command
          case 0x13:
            colorLED('N');                    //Switching the RGB LED OFF          
            servoIndex.write(outIndexClose);
            servoOther.write(outOtherClose);
            servoThumb.write(outThumbClose);
            delay(15);                        //Allow time for servo to change position 
            blocking();          
          break;
          
          //Checking for "BLOQUE EN POSITION" command
          case 0x14:  
            colorLED('N');                    //Switching the RGB LED OFF
            servoIndex.write(outIndex);
            servoOther.write(outOther);
            servoThumb.write(outThumbOpen);
            delay(15);                        //Allow time for servo to change position
            blocking();                    
          break;    
        }
      }
      count = 0;
    }
  }
}

// =====================  void blocking()  =======================
/* Description : blocking loop
*/
// Parameters : NONE
void blocking(){
  while(com != 0x15){       
    com   =   mySerial.read();
    int count = 0;
    
    if(com == 0x11){
      colorLED('P');    //Lighting RGB LED in PURPLE
      
      while((count < 200) && (com != 0x15)){        
        count++;
        com   =   mySerial.read();
      }
      colorLED('N');    //Switching the RGB LED OFF
    }
    else{
      com = 0x00;
    }
  }
}
