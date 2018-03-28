#include <Servo.h>

Servo myServo1;
Servo myServo2;

int const   fsr1Pin  =   A0;                 //Analog Pin for 1st FSR Sensor
int const   fsr2Pin  =   A1;                 //Analog Pin for 2nd FSR Sensor

int const   PBPin   =   2;                   //Digital Pin for PushButton

int const   R1Pin    =   12;                 //Digital Pin for Red component of RGB LED
int const   G1Pin    =   11;                 //Digital Pin for Green component of RGB LED
int const   B1Pin    =   6;                  //Digital Pin for Blue component of RGB LED     

int const   R2Pin    =   13;                 //Digital Pin for Red component of RGB LED
int const   G2Pin    =   5;                  //Digital Pin for Green component of RGB LED
int const   B2Pin    =   4;                  //Digital Pin for Blue component of RGB LED 

int   fsr1Val    =   0;                      //Value returned by FSR sensor
int   angle1     =   0;                      //Converted value in degrees for Servo

int   fsr2Val    =   0;                      //Value returned by FSR sensor
int   angle2     =   0;                      //Converted value in degrees for Servo

unsigned long   previousMillis    =   0;     //Previous time for blocking
unsigned long   currentMillis     =   0;     //Current time for blocking 

const long    interval    =   3000;          //Checking interval for blocking

const int   thresholdHigh   =   900;
const int   thresholdLow    =   20;

boolean   check1    =   false;
boolean   check2    =   false;
boolean   check3    =   false;
boolean   check4    =   false;
boolean   check5    =   false;

void setup() {
  myServo1.attach(9);        //Set Servo pin
  myServo2.attach(10);       //Set Servo pin
  
  pinMode(PBPin, INPUT);     //Set PushButton pin
  
  pinMode(R1Pin, OUTPUT);    //Set Red component pin of RGB LED
  pinMode(G1Pin, OUTPUT);    //Set Green component pin of RGB LED
  pinMode(B1Pin, OUTPUT);    //Set Blue component pin of RGB LED
  
  pinMode(R2Pin, OUTPUT);    //Set Red component pin of RGB LED
  pinMode(G2Pin, OUTPUT);    //Set Green component pin of RGB LED
  pinMode(B2Pin, OUTPUT);    //Set Blue component pin of RGB LED
    
  Serial.begin(9600);
  //while(!Serial);           //Waiting for Serial to be initialized
}

void loop() {
    
  blockingHoldAndPB(fsr1Pin, R1Pin, G1Pin, B1Pin);
  //blockingPatternAndPB();
  
  fsr1Val   =   analogRead(fsr1Pin);
  fsr2Val   =   analogRead(fsr2Pin);
  Serial.print("fsr1 : ");
  Serial.print(fsr1Val);
  Serial.print(" | fsr2 : ");
  Serial.println(fsr2Val);
  
  angle1    =   map(fsr1Val, 0, 1023, 0, 179);   //Mapping FSR values to angle ones
  angle2    =   map(fsr2Val, 0, 1023, 0, 179);   //Mapping FSR values to angle ones
  
  if((fsr1Val >= 0) && (fsr1Val < 300)){
    //Displaying Red LED
    analogWrite(R1Pin,254);
    analogWrite(G1Pin,0);
    analogWrite(B1Pin,0);
  }
  else if((fsr1Val >= 300) && (fsr1Val < 800)){
    //Displaying Green LED
    analogWrite(R1Pin,0);
    analogWrite(G1Pin,254);
    analogWrite(B1Pin,0);
  }
  else if(fsr1Val >= 800){
    //Displaying Blue LED
    analogWrite(R1Pin,0);
    analogWrite(G1Pin,0);
    analogWrite(B1Pin,254);
  }
  if((fsr2Val >= 0) && (fsr2Val < 300)){
    //Displaying Red LED
    analogWrite(R2Pin,254);
    analogWrite(G2Pin,0);
    analogWrite(B2Pin,0);
  }
  else if((fsr2Val >= 300) && (fsr2Val < 800)){
    //Displaying Green LED
    analogWrite(R2Pin,0);
    analogWrite(G2Pin,0);
    analogWrite(B2Pin,254);
  }
  else if(fsr2Val >= 800){
    //Displaying Blue LED
    analogWrite(R2Pin,0);
    analogWrite(G2Pin,254);
    analogWrite(B2Pin,0);
  }
  myServo1.write(angle1);   //Moving servo
  myServo2.write(angle2);   //Moving servo
  delay(15);                //Allow time for servo to change position
}

//FUNCTIONS
void blockingHoldAndPB(int fsrPin, int RPin, int GPin, int BPin){
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

void blockingPatternAndPB(int fsrPin, int RPin, int GPin, int BPin){
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


