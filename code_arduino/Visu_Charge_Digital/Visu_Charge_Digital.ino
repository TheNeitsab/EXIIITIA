#include <Adafruit_NeoPixel.h>

#define   NeoPin    6
#define   NeoNb     1

#define   BatPin    A0
const double    Full    =   4.13;
const double    High    =   3.99;
const double    Mid     =   3.85;
const double    Low     =   3.3;

#define   onSerial  true

Adafruit_NeoPixel Neo = Adafruit_NeoPixel(NeoNb, NeoPin, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  Neo.begin();
}

void loop() {
  
  int   BatVal    =   analogRead(BatPin);
  double    BatVolt   =   mapf(BatVal, 0, 1023, 0, 5);
  if(onSerial){
    Serial.print(BatVal);
    Serial.print(" | ");
    Serial.println(BatVolt);
  }

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

double mapf(double val, double in_min, double in_max, double out_min, double out_max) {
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
