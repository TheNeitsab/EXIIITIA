// ================================================================
// ===                      ARDUINO CODE                        ===
// ===                      My_Exiitia_V2                       ===
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

#include <VarSpeedServo.h>    //managing servos' behavior
#include <SoftwareSerial.h>   //emulating Serial ports on other pins
#include <ExtiaCounter.h>     //managing timers/counters

// =====================  SOFTWARE SETTINGS  ======================
//#define DEBUG

#ifdef DEBUG
 #define DEBUG_PRINT(x) { Serial.println (x); }
 #define DATA_DEBUG() {  Serial.print("Min="); Serial.print(sensorMin); \
                            Serial.print(",Max="); Serial.print(sensorMax); \
                            Serial.print(",Sensor="); Serial.print(sensorValue); \
                            Serial.print(",Speed="); Serial.print(speed); \
                            Serial.print(",Position="); Serial.print(position); \
                            Serial.print(",OutIndex="); Serial.print(outIndex); \
                            Serial.print(",isThumbOpen="); Serial.print(isThumbOpen); \
                            Serial.print(",isOtherLock="); Serial.println(isOtherLock); }
#else
 #define DEBUG_PRINT(x)
 #define DATA_DEBUG()
#endif

//shifumi game switch values
const int   ROCK     =  0;
const int   PAPER    =  1;
const int   SCISSORS =  2;

//state checking booleans
//const settings
const boolean   isRight     =   0;    //arm's/hand's position => right:1 | left:0
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
//byte   com  =  0;

//inactivity checking parameters
int   thresholdInact   =   70;      //position threshold

// =====================  HARDWARE SETTINGS  ======================
//servos' definition
VarSpeedServo   servoIndex;   //index finger
VarSpeedServo   servoOther;   //other three fingers
VarSpeedServo   servoThumb;   //thumb

//setting vocal module serial communication : RX, TX on pin 3, 4
SoftwareSerial   mySerial(3, 4);    

//counters' definition
ExtiaCounter  Calib;
volatile boolean  checkCalib = false;
ExtiaCounter  Inact;
volatile boolean  checkInact = false;
ExtiaCounter  TEST;
boolean  checkTEST = false;
//digital pins for RGB components of RGB LED
int const   pinR   =   12;    //RED          
int const   pinG   =   13;    //GREEN       
int const   pinB   =   9;     //BLUE
//different kinds of colors available for colorLED function
enum COLOR {RED, GREEN, BLUE, PURPLE, NONE};
     
//digital pins for pushbuttons
int   pinCalib;    //enable calibration
int   pinThumb;    //open/close thumb
int   pinOther;    //(un)lock other three fingers

//pushbutton troubleshooting counters
int   countCalib, countThumb, countOther, countGame = 0;

//sensor's input pin
int   pinSensor = A0;              

int const pinCheck = 10;

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup(){
  Serial.begin(9600);     //initializing Arduino IDE's Serial
  mySerial.begin(9600);   //initializing Vocal Module's Serial

  servoIndex.attach(2);   //index servo
  servoOther.attach(5);   //other servo
  servoThumb.attach(6);   //thumb servo

  Calib.setCounter(0,4000,callbackCalib);
  Inact.setCounter(1,5000,callbackInact);
  
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

  pinMode(pinCheck, OUTPUT);
  digitalWrite(pinCheck, LOW);
  
// ===================  VOICE MODULE INIT  ===================
  DEBUG_PRINT("Starting Setup");
  //Setting the voice module in Compact Mode
  mySerial.write(0xAA);
  mySerial.write(0x37);
  DEBUG_PRINT("Compact Mode OK");
  
  delay(2000);
  
  //Importing Group1 vocal commands
  mySerial.write(0xAA);
  mySerial.write(0x21);
  DEBUG_PRINT("Group 1 imported");
  DEBUG_PRINT("Setup is done !");
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
// ========================  CALIBRATION  =========================
  DEBUG_PRINT("======Waiting for Calibration======");
  while(1){
    //holding open position waiting for calibration
    servoIndex.write(outIndexOpen);
    servoOther.write(outOtherOpen);
    servoThumb.write(outThumbOpen);
    
    DATA_DEBUG();
    delay(10);
    
    if(digitalRead(pinCalib) == LOW){
      calibration();
      break;
    }
  }

// =======================  MANUAL CONTROLS  ======================
  //buttons state checking
  while(1){    
    if(digitalRead(pinCalib) == LOW) { 
      countCalib += 1;
    }
    else countCalib = 0;
    if(countCalib == 10) {
      countCalib   =   0;
      calibration();
    }
    if(digitalRead(pinThumb) == LOW) { 
      countThumb += 1;
    }
    else countThumb = 0;
    if(countThumb == 10) {
      countThumb    =   0;
      isThumbOpen =   !isThumbOpen;
      while (digitalRead(pinThumb) == LOW) delay(1);
    }
    if(digitalRead(pinOther) == LOW) {
      countOther += 1;
    }
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
    
    DATA_DEBUG();
    
    visualStrength(); 
    inactivity();    
    //shifumiGame();
    troubleshooting();
                    //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ Timer blocage vocal
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
  DEBUG_PRINT("======calibration start======");
  colorLED(GREEN);    //Lighting RGB LED in GREEN

  sensorMin = 1024;
  sensorMax = 0;
  checkCalib = false;
  Calib.startCounter(0);

  //starting the calibration process
  while(!checkCalib) {
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

    DATA_DEBUG();
  }
  colorLED(BLUE);
  DEBUG_PRINT("======calibration finish======");
}

// ===============  void colorLED(COLOR color)  ==================
/* Description : setting the RGB LED to the according color passed
 *               to the function.
*/
// Parameters : COLOR color -> enum of the available colors

void colorLED(COLOR color){
  switch(color){
    case RED: //RED
      analogWrite(pinR, 254);
      analogWrite(pinG, 0);
      analogWrite(pinB, 0);
    break;
    case GREEN: //GREEN
      analogWrite(pinR, 0);
      analogWrite(pinG, 254);
      analogWrite(pinB, 0);
    break;
    case BLUE: //BLUE
      analogWrite(pinR, 0);
      analogWrite(pinG, 0);
      analogWrite(pinB, 254);
    break;
    case PURPLE: //PURPLE
      analogWrite(pinR, 254);
      analogWrite(pinG, 254);
      analogWrite(pinB, 254);
    break;
    case NONE: //NONE
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
  if(sensorValue < sensorMin + thresholdInact) {
    Inact.startCounter(1);
  }
  else {
    Inact.resetCounter(1);
    checkInact = false;
  }
  if(checkInact) {  
    colorLED('P');
    servoIndex.detach();         
    servoOther.detach();         
    servoThumb.detach();
    DEBUG_PRINT("Servos Detached");
    
    while(sensorValue < sensorMin + thresholdInact) {
      sensorValue = readSensor();
    }
    colorLED('N');
    servoIndex.attach(2);         
    servoOther.attach(5);         
    servoThumb.attach(6);            
    DEBUG_PRINT("Servos Attached");
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

      DEBUG_PRINT("Sarting SHIFUMI !!");
      int shifumi = random(0,3);

      switch(shifumi){
        case ROCK :
          DEBUG_PRINT("ROCK");
          servoIndex.write(outIndexClose);
          servoOther.write(outOtherClose);
          servoThumb.write(outThumbClose);
          isThumbOpen =   !isThumbOpen;
          delay(15);                        
          break;
        case PAPER :
          DEBUG_PRINT("PAPER");
          servoIndex.write(outIndexOpen);
          servoOther.write(outOtherOpen);
          servoThumb.write(outThumbOpen);
          delay(15);                        
          break;
        case SCISSORS :
          DEBUG_PRINT("SCISSORS");
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
      DEBUG_PRINT("End of the game !");
    }
}

// ===================  void troubleshooting()  ===================
// Description : bypassing gemma behavior while a servo is moving
//               to avoid any NeoPixel flickering

// Parameters : NONE
void troubleshooting(){
  if(servoIndex.isMoving() || servoThumb.isMoving() || servoOther.isMoving() || (sensorValue >= sensorMin + 10)){
      digitalWrite(pinCheck, HIGH);      
    }
    else{
      digitalWrite(pinCheck, LOW);
    }
}

void callbackCalib(){
  checkCalib = true;
  Calib.resetCounter(0);
}

void callbackInact(){
  checkInact = true;
  Inact.resetCounter(1);
}

