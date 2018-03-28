#include <Servo.h>

Servo myServo;

int const potPin  =  A0;
int const PBPin   =  2;

int potVal;
int angle;

bool state  =  false;

void setup() {
  myServo.attach(9);
  
  pinMode(PBPin, INPUT);
  
  Serial.begin(9600);
  while(!Serial);

}

void loop() {

  if(digitalRead(PBPin) == HIGH){
    Serial.println("Pushed");
    state = !state;
    
    delay(30);
    
    while(digitalRead(PBPin) == LOW){Serial.println("Blocked");}
    delay(30);
  }
  
  potVal = analogRead(potPin);
  Serial.print("potVal = ");
  Serial.print(potVal);

  angle = map(potVal, 0, 1023, 0, 179);
  Serial.print(" & angle = ");
  Serial.println(angle);

  myServo.write(angle);
  delay(15);
}
