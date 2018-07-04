// ================================================================
// ===                    Charge Indicator                      ===
// ===                    GABRIELLI Bastien                     ===
// ================================================================

// ================================================================
// ===                      DESCRIPTION                         ===
// ================================================================

/*
 * Draft code implementing a digital battery level indicator for
 * EXIIITIA's prosthesis project, created by GABRIELLI Bastien.
*/


// ================================================================
// ===                    GLOBAL DEFINITIONS                    ===
// ================================================================

// =========================  LIBRARIES  ==========================

#include <Adafruit_NeoPixel.h>  //lib managing neopixels

// ===================  PINS & OUT DEFINITIONS  ===================

#define   NeoPin    0  //pin to which are connected the neopixels
#define   NeoNb     4  //number of neopixels to use
#define   BatPin    1  //Analog pin to which is connected the battery

//setting the different levels of charge for a 7.2V battery
const double    Full    =   8.4;
const double    High    =   8.1;
const double    Mid     =   7.2;
const double    Low     =   6.3;
//creating an instance of Neopixels 
Adafruit_NeoPixel Neo = Adafruit_NeoPixel(NeoNb, NeoPin, NEO_GRB + NEO_KHZ800);

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
  pinMode(BatPin, INPUT);
  Neo.begin();
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
  
  int   BatVal    =   readSensor();                             //Getting Battery's value
  double    BatVoltIn    =   mapf(BatVal, 0, 1023, 0, 3.3);     //Mapping it to 0. - 3.3 Volts
  double    BatVoltOut   =   mapf(BatVoltIn, 0, 3.3, 0, 8.5);   //Mapping it to 0. - 8.5 Volts

  //State indication
  if(BatVoltOut > Full){
    Neo.setPixelColor(0, Neo.Color(0,20,0)); 
    Neo.setPixelColor(1, Neo.Color(0,20,0)); 
    Neo.setPixelColor(2, Neo.Color(0,20,0)); 
    Neo.setPixelColor(3, Neo.Color(0,20,0)); 
  }
  else if((BatVoltOut >= High) && (BatVoltOut < Full)){
    Neo.setPixelColor(0, Neo.Color(0,0,20)); 
    Neo.setPixelColor(1, Neo.Color(0,0,20)); 
    Neo.setPixelColor(2, Neo.Color(0,0,20)); 
    Neo.setPixelColor(3, Neo.Color(0,0,0)); 
  }
  else if((BatVoltOut >= Mid) && (BatVoltOut < High)){
    Neo.setPixelColor(0, Neo.Color(20,5,0)); 
    Neo.setPixelColor(1, Neo.Color(20,5,0)); 
    Neo.setPixelColor(2, Neo.Color(0,0,0)); 
    Neo.setPixelColor(3, Neo.Color(0,0,0)); 
  }
  else if((BatVoltOut >= Low) && (BatVoltOut < Mid)){
    Neo.setPixelColor(0, Neo.Color(20,0,0)); 
    Neo.setPixelColor(1, Neo.Color(0,0,0)); 
    Neo.setPixelColor(2, Neo.Color(0,0,0)); 
    Neo.setPixelColor(3, Neo.Color(0,0,0)); 
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
//     ->  double in_min  : IN lower bound          
//     ->  double in_max  : IN upper bound
//     ->  double out_min : OUT lower bound          
//     ->  double out_max : OUT upper bound
// OUT ->  returns the converted double value 

double mapf(double val, double in_min, double in_max, double out_min, double out_max) {
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// ======================  int readSensor()  ======================
// Description : getting sensor's value making an average over a 30
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
