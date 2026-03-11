#include "Thing.h"
#include <Arduino.h>

bool useInternalLED = true;
const int tempPin = 2;     //analog input pin constant
int tempVal;    // temperature sensor raw readings
float volts;    // variable for storing voltage
float temp;     // actual temperature variable

void setup()
{
  Serial.begin(115200);
  Serial.printf("setup blink...\n");
  blink(1, 500);
}

void loop()
{
  //read the temp sensor and store it in tempVal
  tempVal = analogRead(tempPin);

  volts = tempVal / 1023.0;

  temp = (volts - 0.5) * 100 ;
  Serial.printf(" Temperature is:   ");
  Serial.print(temp);                  // in the same line print the temperature
  Serial.printf (" degrees C\n");       // still in the same line print degrees C, then go to next line.

  delay(1000);                         // wait for 1 second or 1000 milliseconds before taking the next reading.
}

void ledOn()  { digitalWrite(LED_BUILTIN, HIGH); }
void ledOff() { digitalWrite(LED_BUILTIN, LOW); }

// Blink the built-in LED a given number of times with a given delay
void blink(int times, int pause) {
  ledOff();
  for(int i = 0; i < times; i++) {
    ledOn();  delay(pause);
    ledOff(); delay(pause);
  }
}