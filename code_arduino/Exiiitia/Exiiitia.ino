// ================================================================
// ===                      ARDUINO CODE                        ===
// ===                  HACKberry - EXIIITIA                    ===
// ================================================================

// ================================================================
// ===                      DESCRIPTION                         ===
// ================================================================

/*
 *  Arduino micro code for HACKberry.
 *  Origially created by exiii Inc.
 *  Edited by Genta Kondo on 2017/6/11.
 *  Adapted for EXTIA's E-Nable Project.
*/


// ================================================================
// ===                    GLOBAL DEFINITIONS                    ===
// ================================================================

#include <Servo.h>    //library managing servos' behavior

// =========================  SETTINGS  ===========================

const boolean   isRight    =   1;       //arm's/hand's position => right:1 | left:0
const boolean   onSerial   =   1;       //enabling Serial command
//out handling values
const int   outThumbMax      =   140;   //right:open | left:close
const int   outIndexMax      =   130;   //right:open | left:close
const int   outOtherMax      =   145;   //right:open | left:close
const int   outThumbMin      =   47;    //right:close | left:open
const int   outIndexMin      =   27;    //right:close | left:open
const int   outOtherMin      =   105;   //right:close | left:open
//speed handling values
const int   speedMax         =   6;     //upper bound
const int   speedMin         =   0;     //lower bound
const int   speedReverse     =   -3;    
const int   thSpeedReverse   =   15;    //0-100 
const int   thSpeedZero      =   30;    //0-100

// =========================  HARDWARE  ===========================
//servos' definition
Servo   servoIndex;     //index finger
Servo   servoOther;     //other three fingers
Servo   servoThumb;     //thumb

int   pinCalib;         //enable calibration
//int pinTBD;
int   pinThumb;         //open/close thumb
int   pinOther;         //lock/unlock other three fingers
int   pinSensor = A0;   //sensor's input

// =========================  SOFTWARE  ===========================

//state checking for thumb and other fingers
boolean   isThumbOpen   =   1;
boolean   isOtherLock   =   0;

int   swCount0, swCount1, swCount2, swCount3   =   0;   //pushbutton troubleshooting counters

int   sensorValue   =   0;    //value read from the sensor
int   sensorMax     =   700;  //upper bound 
int   sensorMin     =   0;    //lower bound
int   speed         =   0;
int   position      =   0;

const int   positionMax   =   100;  //upper bound 
const int   positionMin   =   0;    //lower bound 

int   prePosition                   =   0;    //previous position
//initial out settings 
int   outThumb, outIndex, outOther  =   90;   //out values for positioning  
int   outThumbOpen, outThumbClose, outIndexOpen, outIndexClose, outOtherOpen, outOtherClose;


// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup(){
  Serial.begin(9600);

// ===================  PINS & OUT DEFINITIONS  ===================

  if(isRight){
    pinCalib   =   A6;
    //pinTBD   =   A5; 
    pinThumb   =   A4;
    pinOther   =   A3;
    
    outThumbOpen = outThumbMax; outThumbClose = outThumbMin;
    outIndexOpen = outIndexMax; outIndexClose = outIndexMin;
    outOtherOpen = outOtherMax; outOtherClose = outOtherMin;
  }
  else{
    pinCalib   =   11;
    //pinTBD   =   10; 
    pinThumb   =   8;
    pinOther   =   7;
    
    outThumbOpen = outThumbMin; outThumbClose = outThumbMax;
    outIndexOpen = outIndexMin; outIndexClose = outIndexMax;
    outOtherOpen = outOtherMin; outOtherClose = outOtherMax;
  }
  
  servoIndex.attach(3);         //index servo
  servoOther.attach(5);         //other servo
  servoThumb.attach(6);         //thumb servo
  
  pinMode(pinCalib, INPUT);     //A6
  digitalWrite(pinCalib, HIGH);
  //pinMode(pinTBD, INPUT);     //A5
  //digitalWrite(pinTBD, HIGH);
  pinMode(pinThumb, INPUT);     //A4
  digitalWrite(pinThumb, HIGH);
  pinMode(pinOther, INPUT);     //A3
  digitalWrite(pinOther, HIGH);
}


// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop(){
  
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

  position      =   positionMin;
  prePosition   =   positionMin;
  //buttons state checking
  while(1){
    if(digitalRead(pinCalib) == LOW) swCount0 += 1;
    else swCount0 = 0;
    if(swCount0 == 10) {
      swCount0   =   0;
      calibration();
    }
    if(digitalRead(pinThumb) == LOW) swCount2 += 1;
    else swCount2 = 0;
    if(swCount2 == 10) {
      swCount2    =   0;
      isThumbOpen =   !isThumbOpen;
      while (digitalRead(pinThumb) == LOW) delay(1);
    }
    if(digitalRead(pinOther) == LOW) swCount3 += 1;//A3
    else swCount3   =   0;
    if(swCount3 == 10) {
      swCount3    =   0;
      isOtherLock =   !isOtherLock;
      while(digitalRead(pinOther) == LOW) delay(1);
    }
    
// =====================  HAND'S BEHAVIOUR  =======================  
 
    sensorValue = readSensor();
    delay(25);
    //boundaries checking
    if(sensorValue<sensorMin) sensorValue=sensorMin;
    else if(sensorValue>sensorMax) sensorValue=sensorMax;
    sensorToPosition();
    //moving servos to position part
    outIndex = map(position, positionMin, positionMax, outIndexOpen, outIndexClose);
    servoIndex.write(outIndex);
    if(!isOtherLock){
      outOther = map(position, positionMin, positionMax, outOtherOpen, outOtherClose);
      servoOther.write(outOther);
    }
    if(isThumbOpen) servoThumb.write(outThumbOpen);
    else servoThumb.write(outThumbClose);  
    if(onSerial) serialMonitor();
  } 
}

// ================================================================
// ===                       FUNCTIONS                          ===
// ================================================================

// ======================  int readSensor()  ======================
// Description : getting sensor's value making an average over a 10
//               values buffer to avoid any flickering

// Parameters : OUT -> returns the averaged value
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
  int tmpVal   =   map(sensorValue, sensorMin, sensorMax, 100, 0);
  if(tmpVal < thSpeedReverse) speed   =   speedReverse;
  else if(tmpVal<thSpeedZero) speed   =   speedMin;
  else speed   =   map(tmpVal,40,100,speedMin,speedMax);
  
  position = prePosition + speed;
  if(position < positionMin) position = positionMin;
  if(position > positionMax) position = positionMax;
  prePosition   =   position;
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
  //going to default position
  outIndex    =   outIndexOpen;
  servoIndex.write(outIndexOpen);
  servoOther.write(outOtherClose);
  servoThumb.write(outThumbOpen);
  position    =   positionMin; 
  prePosition =   positionMin;
  
  delay(200);
  if(onSerial) Serial.println("======calibration start======");

  sensorMax   =   readSensor();
  sensorMin   =   sensorMax - 50;
  unsigned long time = millis();
  //doing a 4 seconds calibration process
  while(millis() < time + 4000) {
    sensorValue = readSensor();
    delay(25);
    //boundaries checking
    if(sensorValue < sensorMin) sensorMin = sensorValue;
    else if(sensorValue > sensorMax) sensorMax = sensorValue;
    
    sensorToPosition();
    outIndex   =   map(position, positionMin, positionMax, outIndexOpen, outIndexClose);    
    servoIndex.write(outIndex);
    
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
  Serial.print(",sensor="); Serial.print(sensorValue);
  Serial.print(",speed="); Serial.print(speed);
  Serial.print(",position="); Serial.print(position);
  Serial.print(",outIndex="); Serial.print(outIndex);
  Serial.print(",isThumbOpen="); Serial.print(isThumbOpen);
  Serial.print(",isOtherLock="); Serial.println(isOtherLock);
}
