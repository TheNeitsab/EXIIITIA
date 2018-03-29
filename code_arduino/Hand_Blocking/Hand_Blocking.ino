// ================================================================
// ===                      Hand_Blocking                       ===
// ===                    GABRIELLI Bastien                     ===
// ================================================================

// ================================================================
// ===                      DESCRIPTION                         ===
// ================================================================

/*
 * Draft code implementing some (un)blocking functions in order to 
 * hold EXIIITIA's hand prosthesis in position. One is (un)blocking 
 * after a 3 seconds contraction, while the other needs a pattern 
 * of 2 short contractions. Both are also allowing the (un)blocking 
 * using the available pushbutton on the hand.
*/


// ================================================================
// ===                    GLOBAL DEFINITIONS                    ===
// ================================================================

// =========================  LIBRARIES  ==========================

#include <Servo.h>

// ===================  PINS & OUT DEFINITIONS  ===================

Servo myServo;

int const   fsrPin  =   A0;                 //Analog Pin for FSR Sensor

int const   PBPin   =   2;                  //Digital Pin for PushButton

int const   RPin    =   12;                 //Digital Pin for Red component of RGB LED
int const   GPin    =   11;                 //Digital Pin for Green component of RGB LED
int const   BPin    =   6;                  //Digital Pin for Blue component of RGB LED     

int   fsrVal    =   0;                      //Value returned by FSR sensor
int   angle     =   0;                      //Converted value in degrees for Servo

unsigned long   previousMillis    =   0;    //Previous time for blocking
unsigned long   currentMillis     =   0;    //Current time for blocking 

const long    interval    =   3000;         //Checking interval for blocking

const int   thresholdHigh   =   900;        //Upper bound
const int   thresholdLow    =   20;         //Lower bound

//Step checkers for the blocking pattern
boolean   check1    =   false;      
boolean   check2    =   false;
boolean   check3    =   false;
boolean   check4    =   false;
boolean   check5    =   false;

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {

// =======================  PINS SETTINGS  =========================

  myServo.attach(9);        //Set Servo pin
  
  pinMode(PBPin, INPUT);    //Set PushButton pin
  pinMode(RPin, OUTPUT);    //Set Red component pin of RGB LED
  pinMode(GPin, OUTPUT);    //Set Green component pin of RGB LED
  pinMode(BPin, OUTPUT);    //Set Blue component pin of RGB LED
    
  Serial.begin(9600);
  //while(!Serial);           //Waiting for Serial to be initialized
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
  
  //blockingHoldAndPB();
  //blockingPatternAndPB();
  
  fsrVal   =   analogRead(fsrPin);             //Getting FSR value's
  Serial.println(fsrVal);
  angle    =   map(fsrVal, 0, 1023, 0, 179);   //Mapping FSR values to angle ones

  //LED contraction strength indicator
  //Low
  if((fsrVal >= 0) && (fsrVal < 300)){
    //Displaying Red LED
    analogWrite(RPin,254);
    analogWrite(GPin,0);
    analogWrite(BPin,0);
  }
  //Medium
  else if((fsrVal >= 300) && (fsrVal < 800)){
    //Displaying Green LED
    analogWrite(RPin,0);
    analogWrite(GPin,254);
    analogWrite(BPin,0);
  }
  //High
  else if(fsrVal >= 800){
    //Displaying Blue LED
    analogWrite(RPin,0);
    analogWrite(GPin,0);
    analogWrite(BPin,254);
  }

  myServo.write(angle);   //Moving servo
  delay(15);              //Allow time for servo to change position
}

// ================================================================
// ===                       FUNCTIONS                          ===
// ================================================================

// =================  void blockingHoldAndPB()  ===================
// Description : (un)blocking the hand in current position after a
//                3 second contraction. Possibility to (un)block it
//                using the pushbutton.

// Parameters : NONE

void blockingHoldAndPB(){

  int   fsr   =   analogRead(fsrPin);
  boolean   PB   =   digitalRead(PBPin);
  
  if(fsr > thresholdHigh){    
    currentMillis   =   millis();

    //Checking for blocking confirmation
    if((currentMillis - previousMillis >= interval) || PB){
      currentMillis   =   millis();
      previousMillis  =   currentMillis;
      
      PB   =   false;

      //Displaying White LED
      analogWrite(RPin,254);
      analogWrite(GPin,254);
      analogWrite(BPin,254);

      delay(1000);    //Allow time for blocking correctly

      //Holding condition for blocking
      while((currentMillis - previousMillis < interval) && (!PB)){
        fsr   =   analogRead(fsrPin);
        PB    =   digitalRead(PBPin);
        
        if(fsr > thresholdHigh){
          currentMillis   =   millis();
        }
        else{
          currentMillis   =   millis();
          previousMillis  =   currentMillis;
        }  
      }
      //Reset
      currentMillis   =   millis();
      previousMillis  =   currentMillis;

      analogWrite(RPin,0);
      analogWrite(GPin,0);
      analogWrite(BPin,0);
    }
  }
  else{
    //Getting current time of execution
    currentMillis   =   millis();
    previousMillis  =   currentMillis;  
  }
}

// =================  void blockingHoldAndPB()  ===================
// Description : (un)blocking the hand in current position after a
//                2 short contracions pattern. Possibility to 
//               (un)block it using the pushbutton.

// Parameters : NONE

void blockingPatternAndPB(){

  int   fsr   =   analogRead(fsrPin);
  boolean   PB   =   digitalRead(PBPin);
  
  if((fsr >= thresholdHigh) || check1){
    check1    =   true;
    
    currentMillis   =   millis();
    
    if(check1 && (fsr <= thresholdLow) && (currentMillis - previousMillis >= 500) && (currentMillis - previousMillis <= 1000)){
      check2   =   true;
    }
    else if(check2 && (fsr >= thresholdHigh) && (currentMillis - previousMillis >= 1000) && (currentMillis - previousMillis <= 1500)){
      check3   =   true;
    }
    else if(check3 && (fsr <= thresholdLow) && (currentMillis - previousMillis >= 1500) && (currentMillis - previousMillis <= 2000)){
      check4   =   true;
    }
    else if(PB){
      check5   =   true;
    }
    else if(check4 || check5){
      //Displaying White LED
      analogWrite(RPin,254);
      analogWrite(GPin,254);
      analogWrite(BPin,254);

      //Reset
      previousMillis   =   currentMillis;
      
      PB   =   false;
  
      check1   =   false;
      check2   =   false;
      check3   =   false;
      check4   =   false;
      check5   =   false;
      
      delay(30);
      
      while((check4 == false) && (!PB)){
        fsr   =   analogRead(fsrPin);
        PB    =   digitalRead(PBPin);
        
        if((fsr >= thresholdHigh) || check1){
          check1   =   true;
          
          currentMillis   =   millis();
          
          if(check1 && (fsr <= thresholdLow) && (currentMillis - previousMillis >= 500) && (currentMillis - previousMillis <= 1000)){
            check2   =   true;
          }
          else if(check2 && (fsr >= thresholdHigh) && (currentMillis - previousMillis >= 1000) && (currentMillis - previousMillis <= 1500)){
            check3   =   true;
          }
          else if(check3 && (fsr <= thresholdLow) && (currentMillis - previousMillis >= 1500) && (currentMillis - previousMillis <= 2000)){
            check4   =   true;
          }
          else if((currentMillis - previousMillis >= 2000)){
            //Timeout reset
            check1   =   false;
            check2   =   false;
            check3   =   false;
            check4   =   false;
          }
        }
        else{
          //Time reset
          currentMillis   =   millis();
          previousMillis  =   currentMillis;
        }  
      }
      //Reset
      currentMillis   =   millis();
      previousMillis  =   currentMillis;
      
      check1   =   false;
      check2   =   false;
      check3   =   false;
      check4   =   false;
      check5   =   false;
      
      analogWrite(RPin,0);
      analogWrite(GPin,0);
      analogWrite(BPin,0);
    }
    else if((currentMillis - previousMillis >= 2000)){
      //Timeout reset
      check1   =   false;
      check2   =   false;
      check3   =   false;
      check4   =   false;
    }
  }
  else{
    //Time reset
    currentMillis   =   millis();
    previousMillis  =   currentMillis; 
  }
}


