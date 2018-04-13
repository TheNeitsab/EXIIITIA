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
 * Finally a vocal recognition based solution has been implemented.
*/


// ================================================================
// ===                    GLOBAL DEFINITIONS                    ===
// ================================================================

// =========================  LIBRARIES  ==========================

#include <Servo.h>
#include <SoftwareSerial.h>                 //Used to emulate Serial ports on other pins

// ===================  PINS & OUT DEFINITIONS  ===================

Servo            myServo;
SoftwareSerial   mySerial(10, 11);          //Setting RX, TX on pin 10, 11

byte com = 0;                               //Message got from Vocal Recognition Module

int const   fsrPin  =   A0;                 //Analog Pin for FSR Sensor

int const   PBPin   =   2;                  //Digital Pin for PushButton

int const   RPin    =   12;                 //Digital Pin for Red component of RGB LED
int const   GPin    =   13;                 //Digital Pin for Green component of RGB LED
int const   BPin    =   6;                  //Digital Pin for Blue component of RGB LED    

int const   Buzzer  =   4;                  //Digital Pin for Buzzer

int   fsrVal    =   0;                      //Value returned by FSR sensor
int   angle     =   0;                      //Converted value in degrees for Servo

boolean PBVal   =   false;                  //Value returned by PushButton

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

boolean   Bcheck    =   true;               //Buzzer checker to avoid repetition


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
  pinMode(Buzzer, OUTPUT);  //Set Buzzer's pin

  Serial.begin(9600);       //Initializing USB Serial
  //while(!Serial);           //Waiting for Serial to be initialized
  
  mySerial.begin(9600);     //Initializing Vocal Module's Serial

  Serial.println("Starting Setup");
  //Setting the module in Compact Mode
  mySerial.write(0xAA);
  mySerial.write(0x37);
  Serial.println("Compact Mode OK");
  
  delay(2000);
  
  //Importing Group1 vocal commands
  mySerial.write(0xAA);
  mySerial.write(0x21);
  Serial.println("Group 1 imported");
  Serial.println("Setup has done !");
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
  
  fsrVal   =   readSensor();             //Getting FSR value's
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

  //Selecting the blocking function
  vocalBlocking();
  //blockingHoldAndPB();
  //blockingPatternAndPB();
  
  myServo.write(angle);   //Moving servo
  delay(15);              //Allow time for servo to change position
}

// ================================================================
// ===                       FUNCTIONS                          ===
// ================================================================

// =================  void blockingHoldAndPB()  ===================
/* Description : (un)blocking the hand in current position after a
 *                3 second contraction. Possibility to (un)block it
 *                using the pushbutton.
*/
// Parameters : NONE

void blockingHoldAndPB(){

  int   fsr   =   readSensor();
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
        fsr   =   readSensor();
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
/* Description : (un)blocking the hand in current position after a
 *                2 short contractions pattern. Possibility to 
 *               (un)block it using the pushbutton.
*/
// Parameters : NONE

void blockingPatternAndPB(){

  int   fsr   =   readSensor();
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
        fsr   =   readSensor();
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
      currentMillis   =   millis();
      previousMillis  =   currentMillis;

      if(Bcheck){
        tone(Buzzer, 500, 50);
        Bcheck = !Bcheck;
      } 
      //Checking if the interval is overpassed or if "DÉBLOQUE" has been said
      while((currentMillis - previousMillis < interval) && (com != 0x15)){
        currentMillis   =   millis();
        com   =   mySerial.read();

        switch(com){
          //Checking for "BLOQUE OUVERT" command
          case 0x12:  
            Bcheck = !Bcheck;

            if(Bcheck){
              tone(Buzzer, 500, 50);
              Bcheck = !Bcheck;
            }
            myServo.write(0);     //Moving servo
            delay(15);                //Allow time for servo to change position
            while((com != 0x15)){
              com   =   mySerial.read();
            }      
          break;
          //Checking for "BLOQUE FERMÉ" command
          case 0x13:
            Bcheck = !Bcheck;

            if(Bcheck){
              tone(Buzzer, 500, 50);
              Bcheck = !Bcheck;
            }  
            myServo.write(175);       //Moving servo
            delay(15);                //Allow time for servo to change position 
            while((com != 0x15)){
              com   =   mySerial.read();
            }           
          break;
          //Checking for "BLOQUE EN POSITION" command
          case 0x14:  
            Bcheck = !Bcheck;

            if(Bcheck){
              tone(Buzzer, 500, 50);
              Bcheck = !Bcheck;
            }
            myServo.write(angle);     //Moving servo
            delay(15);                //Allow time for servo to change position
            while((com != 0x15)){
              com   =   mySerial.read();
            }           
          break;    
        }
      }
      
      //Time reset
      currentMillis   =   millis();
      previousMillis  =   currentMillis;

      Bcheck = !Bcheck;
      tone(Buzzer, 500, 50);
    }
  }
}

int readSensor() {
  int i, sval;
  for(i = 0; i < 10; i++) {
    sval   +=   analogRead(fsrPin);
  }
  sval   =   sval/10;
  return sval;
}


