#include "ESP8266_ISR_Servo.h"
  
int servoIndex1  = -1;
 
void setup() 
{ 
  Serial.begin(115200);
  Serial.println("\nStarting");

  servoIndex1 = ISR_Servo.setupServo(D8, 800, 2000);
  if (servoIndex1 != -1)
    Serial.println("Setup Servo1 OK");
  else
    Serial.println("Setup Servo1 failed");

} 
int m_ReadValue = 0; 
int m_AnglePosition = 0;
int m_savedVal = -1;

void PrintPulseWidthVal()
{
  Serial.write("val = ");
  Serial.println(m_PulseWidthVal);
  Serial.write("\n");
  m_savedVal = m_PulseWidthVal;
}

void loop() 
{
  m_ReadValue = analogRead(0);               // reads potentiometer value (between 0 and 1023)
  m_AnglePosition = map(m_ReadValue, 0, 1023, 0, 170);
  
  if (m_savedVal != m_PulseWidthVal)
  {
    PrintPulseWidthVal();
    ISR_Servo.setPosition(servoIndex1, m_AnglePosition);
  }
}
