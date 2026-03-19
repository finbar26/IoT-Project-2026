#include <Arduino.h>

// Define the number of LEDs
#define NUM_LEDS 6

// Array of LED pins
int ledPins[NUM_LEDS] = {5, 6, 9, 10, 11, 12};

// Time delay between each LED change (in milliseconds)
int delayTime = 100;

void setup() {
    Serial.begin(115200);
  // Initialize each pin as an output
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // Ensure all LEDs are off initially
  }
}

void loop() {
    Serial.printf("hello loop");
  // Turn on each LED in sequence
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], HIGH); // Turn on the LED
    delay(delayTime);               // Wait
    digitalWrite(ledPins[i], LOW);  // Turn off the LED
  }

  // Turn on each LED in reverse sequence
  for (int i = NUM_LEDS - 1; i >= 0; i--) {
    digitalWrite(ledPins[i], HIGH); // Turn on the LED
    delay(delayTime);               // Wait
    digitalWrite(ledPins[i], LOW);  // Turn off the LED
  }
}