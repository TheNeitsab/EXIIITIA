// ================================================================
// ===                    Charge Indicator                      ===
// ===                    GABRIELLI Bastien                     ===
// ================================================================

// ================================================================
// ===                      DESCRIPTION                         ===
// ================================================================

/*
 * Draft code implementing a digital battery management indicator 
 * for EXIIITIA's prosthesis project.
*/


// ================================================================
// ===                    GLOBAL DEFINITIONS                    ===
// ================================================================

// =========================  LIBRARIES  ==========================

#include <Adafruit_NeoPixel.h>

// ===================  PINS & OUT DEFINITIONS  ===================

#define   NeoPin    0
#define   NeoNb     4

#define   BatPin    1
#define   CheckPin  1

const double    Full    =   8.4;
const double    High    =   8.1;
const double    Mid     =   7.2;
const double    Low     =   6.3;

const int   interval      =   500;
double    currentMillis   =   0;
double    previousMillis  =   0;

Adafruit_NeoPixel Neo = Adafruit_NeoPixel(NeoNb, NeoPin, NEO_GRB + NEO_KHZ800);

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
  pinMode(BatPin, INPUT);
  pinMode(CheckPin, INPUT);
  digitalWrite(CheckPin, LOW);
  Neo.begin();
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
  
  int   BatVal    =   readSensor();                 //Getting Battery's value
  double    BatVolt3  =   mapf(BatVal, 0, 1023, 0, 3.3);  //Mapping it to 0. - 3.3 Volts
  double    BatVolt   =   mapf(BatVolt3, 0, 3.3, 0, 8.5);   //Mapping it to 0. - 8.5 Volts

  if(digitalRead(CheckPin) != HIGH){
    //State indication
    if(BatVolt > Full){
      Neo.setPixelColor(0, Neo.Color(0,20,0)); 
      Neo.setPixelColor(1, Neo.Color(0,20,0)); 
      Neo.setPixelColor(2, Neo.Color(0,20,0)); 
      Neo.setPixelColor(3, Neo.Color(0,20,0)); 
    }
    else if((BatVolt >= High) && (BatVolt < Full)){
      Neo.setPixelColor(0, Neo.Color(0,0,20)); 
      Neo.setPixelColor(1, Neo.Color(0,0,20)); 
      Neo.setPixelColor(2, Neo.Color(0,0,20)); 
      Neo.setPixelColor(3, Neo.Color(0,0,0)); 
    }
    else if((BatVolt >= Mid) && (BatVolt < High)){
      Neo.setPixelColor(0, Neo.Color(20,5,0)); 
      Neo.setPixelColor(1, Neo.Color(20,5,0)); 
      Neo.setPixelColor(2, Neo.Color(0,0,0)); 
      Neo.setPixelColor(3, Neo.Color(0,0,0)); 
    }
    else if((BatVolt >= Low) && (BatVolt < Mid)){
      Neo.setPixelColor(0, Neo.Color(20,0,0)); 
      Neo.setPixelColor(1, Neo.Color(0,0,0)); 
      Neo.setPixelColor(2, Neo.Color(0,0,0)); 
      Neo.setPixelColor(3, Neo.Color(0,0,0)); 
    }   
    Neo.show();
  }
}

// ================================================================
// ===                       FUNCTIONS                          ===
// ================================================================

// == double mapf(double val, double in_min, double in_max, double out_min, double out_max) ==
// Description : mapping function returning a floating value

// Parameters :
// IN  ->  double val     : value to convert
//     ->  double in_min  : in lower bound          
//     ->  double in_max  : in upper bound
//     ->  double out_min : out lower bound          
//     ->  double out_max : out upper bound
// OUT ->  returns the double converted value 

double mapf(double val, double in_min, double in_max, double out_min, double out_max) {
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// ======================  int readSensor()  ======================
// Description : getting sensor's value making an average over a 10
//               values buffer to avoid any flickering

// Parameters : OUT -> returns the averaged value

int readSensor() {
  int i, sval;
  for(i = 0; i < 30; i++) {
    sval   +=   analogRead(BatPin);
  }
  sval   =   sval/30;
  return sval;
}
