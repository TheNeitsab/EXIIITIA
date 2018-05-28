#include <ExtiaCounter.h>

ExtiaCounter ex;

// ======== Various useless callbacks for tests ======
void printHello()
{
  Serial.println("Hello world !");
}
void printGb()
{
  Serial.println("Goodbye world !");
}
void ltbl()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}
// ===================== End of cb def =====================

void setup() {
  Serial.begin(9600);
  
  ex.setCounter(0, 1000, ltbl);
  ex.setCounter(1, 500, printHello);
  ex.setCounter(2, 2000, printGb);
}

void loop() {
  // Nothing to do here.
}