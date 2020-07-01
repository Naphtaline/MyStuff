#include <Servo.h>

Servo ESC;     // create servo object to control the ESC
int potValue;  // value from the analog pin

void setup() {
  // Attach the ESC on pin 9
  ESC.attach(7,1000,2000); // (pin, min pulse width, max pulse width in microseconds) 
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       
  ESC.write(0);    // Send the signal to the ESC
  delay(1000);                       
  ESC.write(40);
  delay(1000);
}
