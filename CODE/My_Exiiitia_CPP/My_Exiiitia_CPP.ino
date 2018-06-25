// ================================================================
// ===                      ARDUINO CODE                        ===
// ===                     My_Exiitia_CPP                       ===
// ================================================================

// ================================================================
// ===                      DESCRIPTION                         ===
// ================================================================

/*
 * Arduino micro code for Exiiitia with C++ architecture.
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

 * Note : this code doesn't include neither vocal recognition module 
 * nor blocking system for the moment.
*/

// ================================================================
// ===                    GLOBAL DEFINITIONS                    ===
// ================================================================

// ==========================  INCLUDES  ==========================

#include <ExiiitiaLibV3.h>

// ==========================  SETTINGS  ==========================
//if you'd like to change for your own settings, check ExiiitiaLibV3.h

//fingers' settings according to hand's side (here hand is positioned LEFT,
//you may interchange ...MinPos and ...MaxPos if it is positioned RIGHT)
FingerData indexData = {indexPin,
                        indexMinPos,
                        indexMaxPos,
                        indexMinSpeed,
                        indexMaxSpeed};

FingerData thumbData = {thumbPin,
                        thumbMinPos,
                        thumbMaxPos,
                        thumbMinSpeed,
                        thumbMaxSpeed};

FingerData otherData = {otherPin,
                        otherMinPos,
                        otherMaxPos,
                        otherMinSpeed,
                        otherMaxSpeed};
LedData ledData = {redPin, greenPin, bluePin};
SensorData sensorData = {sensorPin, sensorMin, sensorMax};      
                  
Hand hand;

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
  Serial.begin(9600);
  //initializing hand
  hand = Hand(sensorData, ledData, indexData, thumbData, otherData);
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
  //starting waiting for first calirbation before anything else
  while(1){
    if(digitalRead(calibButtonPin) == LOW){
      hand.calibration();
      break;
    }
  }
  //normal behavior of the hand
  while(1){
    hand.updateSensorValue();
    if(digitalRead(thumbButtonPin) == LOW){
      while(digitalRead(thumbButtonPin) == LOW);
      hand.moveThumb();
    }  
    if(digitalRead(calibButtonPin) == LOW){
      hand.calibration();
    }
    if(digitalRead(shifumiButtonPin) == LOW){
      while(digitalRead(shifumiButtonPin) == LOW);
      hand.shifumi();
      while(digitalRead(shifumiButtonPin) == LOW);
    }
    hand.moveIndex();
    hand.moveOther();
    hand.saveServos();
  } 
}
