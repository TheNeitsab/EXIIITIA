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

#include <Servo.h>            //library managing servos' behavior
#include <SoftwareSerial.h>   //Used to emulate Serial ports on other pins

// =========================  SETTINGS  ===========================

const boolean   isRight    =   0;       //arm's/hand's position => right:1 | left:0
const boolean   onSerial   =   1;       //enabling Serial command
//out handling values
const int   outThumbMax      =   34;    //right:open | left:close
const int   outIndexMax      =   140;   //right:open | left:close
const int   outOtherMax      =   150;   //right:open | left:close
const int   outThumbMin      =   135;   //right:close | left:open
const int   outIndexMin      =   30;    //right:close | left:open
const int   outOtherMin      =   100;   //right:close | left:open
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

SoftwareSerial   mySerial(3, 4);            //Setting RX, TX on pin 3, 4

int const   RPin    =   12;                 //Digital Pin for Red component of RGB LED
int const   GPin    =   13;                 //Digital Pin for Green component of RGB LED
int const   BPin    =   9;                  //Digital Pin for Blue component of RGB LED $$$$$$10

int   pinCalib;                             //enable calibration
//int pinTBD;
int   pinThumb;                             //open/close thumb
int   pinOther;                             //lock/unlock other three fingers
int   pinSensor = A0;                       //sensor's input

// =========================  SOFTWARE  ===========================

//state checking for thumb and other fingers
boolean   isThumbOpen   =   1;
boolean   isOtherLock   =   0;

int   swCount0, swCount1, swCount2, swCount3   =   0;   //pushbutton troubleshooting counters

unsigned long   previousBlock    =   0;                //Previous time for blocking
unsigned long   currentBlock     =   0;                //Current time for blocking
unsigned long   previousInact    =   0;                //Previous time for inactivity
unsigned long   currentInact     =   0;                //Current time for inactivity
const long    intBlock    =   3000;                    //Checking interval for blocking
const long    intInact    =   15000;                   //Checking interval for inactivity
const int   thresholdInact   =   20;                   //Inactivity threshold

int   sensorValue   =   0;                              //value read from the sensor
int   sensorMax     =   1024;                           //upper bound 
int   sensorMin     =   0;                              //lower bound $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$254
int   speed         =   0;
int   position      =   0;

const int   positionMax   =   100;                      //upper bound 
const int   positionMin   =   0;                        //lower bound 

int   prePosition                   =   0;              //previous position
//initial out settings 
int   outThumb, outIndex, outOther  =   90;             //out values for positioning  
int   outThumbOpen, outThumbClose, outIndexOpen, outIndexClose, outOtherOpen, outOtherClose;

byte   com = 0;                                         //Message got from Vocal Recognition Module

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup(){
  Serial.begin(9600);
  mySerial.begin(9600);     //Initializing Vocal Module's Serial
  
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
  
  servoIndex.attach(2);         //index servo
  servoOther.attach(5);         //other servo $$$$$$$5
  servoThumb.attach(6);         //thumb servo
  
  pinMode(pinCalib, INPUT);     //A6
  digitalWrite(pinCalib, HIGH);
  //pinMode(pinTBD, INPUT);     //A5
  //digitalWrite(pinTBD, HIGH);
  pinMode(pinThumb, INPUT);     //A4
  digitalWrite(pinThumb, HIGH);
  pinMode(pinOther, INPUT);     //A3
  digitalWrite(pinOther, HIGH);
  pinMode(RPin, OUTPUT);        //Set Red component pin of RGB LED
  pinMode(GPin, OUTPUT);        //Set Green component pin of RGB LED
  pinMode(BPin, OUTPUT);        //Set Blue component pin of RGB LED

  if(onSerial) Serial.println("Starting Setup");
  //Setting the module in Compact Mode
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
    int comp = (sensorMax - sensorMin)/2;
    //LED contraction strength indicator
    //Low
    if((sensorValue >= sensorMin + 2*comp)){
      //Displaying Red LED
      colorLED('R');
    }
    //Medium
    else if((sensorValue >= sensorMin + comp) && (sensorValue < sensorMin + 2*comp)){
      //Displaying Green LED
      colorLED('G');
    }
    //High
    else if(sensorValue <= sensorMin + comp){
      //Displaying Blue LED
      colorLED('B');
    }
  
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
    if(isThumbOpen){
      while(outThumb != outThumbOpen){
        outThumb ++;
        servoThumb.write(outThumb);
        delay(10);
      }
    } 
    else{
      while(outThumb != outThumbClose){
        outThumb --;
        servoThumb.write(outThumb);
        delay(10);
      }
    } 
    if(onSerial) serialMonitor();

    vocalBlocking();
    inactivity();
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
  int i, sval, sval2;
  for(i = 0; i < 10; i++) {
    sval   +=   analogRead(pinSensor);
  }
  sval   =   sval/10;
  sval2 = map(analogRead(pinSensor),0,1023,1023,0);
  return sval2;
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
  colorLED('G');
  outIndex    =   outIndexOpen;
  servoIndex.write(outIndexOpen);
  servoOther.write(outOtherOpen);
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
    if(sensorMin < 0){
      sensorMin = 0;
    }
    else if(sensorMax > 1024){
      sensorMax = 1024;
    }
    //boundaries checking
    if(sensorValue < sensorMin){
      if(sensorValue < 0) sensorMin = 0;
      else sensorMin = sensorValue;
    }
    else if(sensorValue > sensorMax){
      if(sensorValue > 1024) sensorMax = 1024;
      else sensorMax = sensorValue;
    }
    
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
  Serial.print(",outOtherOpen="); Serial.println(outOtherOpen); 
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
      colorLED('P');
      timeResetBlock();
      //Checking if the interval is overpassed or if "DÉBLOQUE" has been said
      while((currentBlock - previousBlock < intBlock) && (com != 0x15)){
        currentBlock   =   millis();
        com   =   mySerial.read();

        switch(com){
          
          //Checking for "BLOQUE OUVERT" command
          case 0x12:  
            colorLED('N');   
            servoIndex.write(outIndexOpen);
            servoOther.write(outOtherOpen);
            servoThumb.write(outThumbOpen);
            delay(15);                        //Allow time for servo to change position
            blocking();      
          break;
          
          //Checking for "BLOQUE FERMÉ" command
          case 0x13:
            colorLED('N');           
            servoIndex.write(outIndexClose);
            servoOther.write(outOtherClose);
            servoThumb.write(outThumbClose);
            delay(15);                        //Allow time for servo to change position 
            blocking();          
          break;
          
          //Checking for "BLOQUE EN POSITION" command
          case 0x14:  
            colorLED('N');
            servoIndex.write(outIndex);
            servoOther.write(outOther);
            servoThumb.write(outThumbOpen);
            delay(15);                        //Allow time for servo to change position
            blocking();                    
          break;    
        }
      }
      timeResetBlock();
    }
  }
}

// =====================  void blocking()  =======================
/* Description : blocking loop
*/
// Parameters : NONE
void blocking(){
  while(com != 0x15){
    timeResetBlock();    
    com   =   mySerial.read();
    
    if(com == 0x11){
      colorLED('P');     
      
      while((currentBlock - previousBlock < intBlock) && (com != 0x15)){        
        currentBlock   =   millis();
        com   =   mySerial.read();
      }
      colorLED('N');    
    }
    else{
      com = 0x00;
    }
  }
}

// ==================  void timeResetBlock()  ====================
/* Description : time reset related to the check of the time
 *               interval for blocking.
*/
// Parameters : NONE
void timeResetBlock(){
  currentBlock   =   millis();
  previousBlock  =   currentBlock;
}

// ==================  void timeResetInact()  ====================
/* Description : time reset related to the check of the time
 *               interval for inactivity.
*/
// Parameters : NONE
void timeResetInact(){
  currentInact   =   millis();
  previousInact  =   currentInact;
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
      analogWrite(RPin, 254);
      analogWrite(GPin, 0);
      analogWrite(BPin, 0);
    break;
    case 'G': //GREEN
      analogWrite(RPin, 0);
      analogWrite(GPin, 254);
      analogWrite(BPin, 0);
    break;
    case 'B': //BLUE
      analogWrite(RPin, 0);
      analogWrite(GPin, 0);
      analogWrite(BPin, 254);
    break;
    case 'P': //PURPLE
      analogWrite(RPin, 254);
      analogWrite(GPin, 254);
      analogWrite(BPin, 254);
    break;
    case 'N': //NONE
      analogWrite(RPin, 0);
      analogWrite(GPin, 0);
      analogWrite(BPin, 0);
    break;
  }
}

// ==================  void inactivity()  ====================
/* Description : checking for inactivity in order to detach
 *               and save servos' lifetime
*/
// Parameters : NONE
void inactivity(){
  if(sensorValue > sensorMax - thresholdInact){
    currentInact = millis();
    
    if(currentInact - previousInact > intInact){
      timeResetInact();
      servoIndex.detach();         
      servoOther.detach();         
      servoThumb.detach();
               
      if(onSerial) Serial.println("Servos Detached");
      
      while(sensorValue > sensorMax - thresholdInact){
        sensorValue = readSensor();
      }
      
      servoIndex.attach(2);         
      servoOther.attach(5);         
      servoThumb.attach(6);
               
      if(onSerial) Serial.println("Servos Attached");
    }
  }
  else timeResetInact();
}
