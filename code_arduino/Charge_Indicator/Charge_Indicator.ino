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

#define   NeoPin    6
#define   NeoNb     1

#define   BatPin    A0

const double    Full    =   4.13;
const double    High    =   3.99;
const double    Mid     =   3.85;
const double    Low     =   3.3;

#define   onSerial  true

Adafruit_NeoPixel Neo = Adafruit_NeoPixel(NeoNb, NeoPin, NEO_GRB + NEO_KHZ800);

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
  Neo.begin();
  Serial.begin(9600); 
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
  
  int   BatVal    =   analogRead(BatPin);               //Getting Battery's value
  double    BatVolt   =   mapf(BatVal, 0, 1023, 0, 5);  //Mapping it to 0. - 5. Volts
  if(onSerial){
    Serial.print(BatVal);
    Serial.print(" | ");
    Serial.println(BatVolt);
  }

  //State indication
  if(BatVolt > Full){
    Neo.setPixelColor(0, Neo.Color(0,0,255));
  }
  else if((BatVolt >= High) && (BatVolt < Full)){
    Neo.setPixelColor(0, Neo.Color(0,255,0));
  }
  else if((BatVolt >= Mid) && (BatVolt < High)){
    Neo.setPixelColor(0, Neo.Color(255,255,0));
  }
  else if((BatVolt >= Low) && (BatVolt < Mid)){
    Neo.setPixelColor(0, Neo.Color(255,173,0));
  }
  else{
    Neo.setPixelColor(0, Neo.Color(255,0,0)); 
  }
  
  Neo.show();
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
