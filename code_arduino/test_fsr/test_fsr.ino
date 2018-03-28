#include <Servo.h>

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

const int   thresholdHigh   =   900;
const int   thresholdLow    =   20;

boolean   check1    =   false;
boolean   check2    =   false;
boolean   check3    =   false;
boolean   check4    =   false;
boolean   check5    =   false;

void setup() {
  myServo.attach(9);        //Set Servo pin
  
  pinMode(PBPin, INPUT);    //Set PushButton pin
  
  pinMode(RPin, OUTPUT);    //Set Red component pin of RGB LED
  pinMode(GPin, OUTPUT);    //Set Green component pin of RGB LED
  pinMode(BPin, OUTPUT);    //Set Blue component pin of RGB LED
    
  Serial.begin(9600);
  //while(!Serial);           //Waiting for Serial to be initialized
}

void loop() {
  
  //blockingHoldAndPB();
  blockingPatternAndPB();
  
  fsrVal   =   analogRead(fsrPin);
  Serial.println(fsrVal);
  angle    =   map(fsrVal, 0, 1023, 0, 179);   //Mapping FSR values to angle ones

  if((fsrVal >= 0) && (fsrVal < 300)){
    //Displaying Red LED
    analogWrite(RPin,254);
    analogWrite(GPin,0);
    analogWrite(BPin,0);
  }
  else if((fsrVal >= 300) && (fsrVal < 800)){
    //Displaying Green LED
    analogWrite(RPin,0);
    analogWrite(GPin,254);
    analogWrite(BPin,0);
  }
  else if(fsrVal >= 800){
    //Displaying Blue LED
    analogWrite(RPin,0);
    analogWrite(GPin,0);
    analogWrite(BPin,254);
  }

  myServo.write(angle);   //Moving servo
  delay(15);              //Allow time for servo to change position
}

//FUNCTIONS
void blockingHoldAndPB(){
  //Checking for (un)blocking after 3s holding or pushing on the PB

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

void blockingPatternAndPB(){
  //Checking for (un)blocking after 2 impulses or pushing on the PB

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
            check1   =   false;
            check2   =   false;
            check3   =   false;
            check4   =   false;
          }
        }
        else{
          currentMillis   =   millis();
          previousMillis  =   currentMillis;
        }  
      }
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
      check1   =   false;
      check2   =   false;
      check3   =   false;
      check4   =   false;
    }
  }
  else{
    currentMillis   =   millis();
    previousMillis  =   currentMillis; 
  }
}


