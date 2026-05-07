#include <Arduino.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <sys/socket.h>


#define NUM_LEDS 6
#define TEMP_PIN A0
#define ADC_MAX 4095.0
#define VREF 3.3

const char* SSID = "amelia_hotspot";
const char* Password = "megan2003";
const char* IpAddress = 

//button pin
const int BUTTON_PIN = 13;
//array of LED pins
const int ledPins[NUM_LEDS] = {5, 6, 9, 10, 11, 12};

//temp variables
const int tempPin = 16; //analog input pin constant - A2 on board
float volts;  // variable for storing voltage
#define VREF 3.3
#define ADC_MAX 4095.0
float initialTemp;
bool useTempControl = false;

//initial button mode
int mode = 0;

//time delay between each LED change (in milliseconds)
int initDelay = 200;
int delayTime = initDelay;

//define methods
void allOff();
void red();
void yellow();
void green();
void chaseSequence();
void rainbow();
void blink();
void tempControl();
void sendTemp();
void getPattern();



//LIGHT PATTERN MODES 

//turns all leds off
void allOff() {
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

//just red
void red() {
  digitalWrite(ledPins[0], HIGH);
  digitalWrite(ledPins[1], HIGH);
}

//just yellow
void yellow() {
  digitalWrite(ledPins[2], HIGH);
  digitalWrite(ledPins[3], HIGH);
}

//just green
void green() {
  digitalWrite(ledPins[4], HIGH);
  digitalWrite(ledPins[5], HIGH);
}

void chaseSequence() {
  //turn on each LED in sequence
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], HIGH); //turn on the LED
    delay(delayTime);               //wait
    digitalWrite(ledPins[i], LOW);  //turn off the LED
  }

  //turn on each LED in reverse sequence
  for (int i = NUM_LEDS - 1; i >= 0; i--) {
    digitalWrite(ledPins[i], HIGH); //turn on the LED
    delay(delayTime);               //wait
    digitalWrite(ledPins[i], LOW);  //turn off the LED
  }
}

//red then yellow then green then all off
void rainbow() {
  red();
  delay(delayTime);
  yellow();
  delay(delayTime);
  green();
  delay(delayTime);
  allOff();
}

//all lights flash at same time
void blink() {
  for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(ledPins[i], HIGH);
    }

    delay(delayTime);

    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(ledPins[i], LOW);
    }
}

//changes the delay time between each LED change based on the 
//temperature read from the sensor (higher temp = faster changes)
void tempControl() {
  int tempValue = analogRead(tempPin);
  volts = tempValue * VREF / ADC_MAX ;
  volts = tempValue / 1023.0;

  float currentTemp = (volts - 0.5) * 100.0 ;
  Serial.printf(" Temperature is:   ");
  Serial.print(currentTemp);
  Serial.printf (" degrees C\n");

  float delayCoeff = ((currentTemp / initialTemp));
  delayTime = initDelay * delayCoeff;
}

//sends current temperature data and button setting to the website
void sendTemp() {
  HTTPClient http;
  //CHANGE IP
  http.begin("http://192.168.240.251:5000/data");
  http.addHeader("Content-Type", "application/json");

  //read temperature from sensor
  int adcValue = analogRead(TEMP_PIN);
  float voltage = adcValue * VREF / ADC_MAX;
  float tempC = (voltage - 0.5) * 100.0;
  float temperature = tempC;
  Serial.print("Site temp: ");
  Serial.println(temperature);

  //send temperature to website
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST("{\"temperature\": " + String(temperature) + ", \"useTempControl\": " + String(useTempControl) + ", \"delayTime\": " + String(delayTime) + "}");

  http.end();
}

//gets the current pattern selected on the website to update the LEDs accordingly
void getPattern() {
  HTTPClient http;
  //CHANGE IP
  http.begin("http://192.168.240.251:5000/pattern");
  // This function can be used to get the current pattern from the server
  int code = http.GET();
  //interpret response from website
  if (code == 200) {
    String buttonResponse = http.getString();
    Serial.printf("Button response: %s\n", buttonResponse.c_str());
    if (buttonResponse == "Rainbow") { 
      rainbow();
    }
    else if (buttonResponse == "Blink") {
      blink();
    }
    else if (buttonResponse == "Chase") {
      chaseSequence();
    }
    else if (buttonResponse == "Off") {
      allOff();
    }
    else if (buttonResponse == "Red") {
      red();
    }
    else if (buttonResponse == "Yellow") {
      yellow();
    }
    else if (buttonResponse == "Green") {
      green();
    }
  }

  http.end();
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, Password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("Connected to WiFi");
  Serial.println("Server Address: http://192.168.240.251:5000/data");
  
  
  //initialize each pin as an output
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); //ensure all LEDs are off initially
  }
  //initialize button as an input
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  analogReadResolution(12); //12-bit ADC
  analogSetPinAttenuation(tempPin, ADC_11db);

  //read initial temp for temp controll mode
  int tempValue = analogRead(tempPin);
  float volts = tempValue * VREF /ADC_MAX ;
  volts = tempValue / 1023.0;

  initialTemp = (volts - 0.5) * 100.0 ;

  //this LED sequence lets you know the code is running
  //leds go red -> yellow -> green then off before loop starts
  red();
  delay(delayTime);
  yellow();
  delay(delayTime);
  green();
  delay(delayTime);
  allOff();
  delay(delayTime);

  Serial.print("Device IP address: ");
  Serial.println(WiFi.localIP());

}


void loop() {
  //define Clients
  WiFiClient client;
  HTTPClient http;

  //check if button is pressed to toggle temp control mode on/off
  if(digitalRead(BUTTON_PIN) == LOW) {
    Serial.printf("button is pressed");
    useTempControl = !useTempControl;
  }

  if(useTempControl) {
    tempControl();
  }

  //if temp control mode is off, reset delay time to initial value
  if(!useTempControl) {
    delayTime = initDelay;
  }

  //begin connection to website
  http.begin(client, "http://192.168.240.251:5000/send_pattern");
  //send current temperature data and button setting to website and get current pattern selected on website
  sendTemp();
  getPattern();

  Serial.print("delay: ");
  Serial.println(delayTime);
  delay(delayTime);
}