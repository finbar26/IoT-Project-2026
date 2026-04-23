#include <Arduino.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


#define NUM_LEDS 6
#define TEMP_PIN A0
#define ADC_MAX 4095.0
#define VREF 3.3

const char* SSID = "amelia_hotspot";
const char* Password = "megan2003";
//button pin
const int BUTTON_PIN = 13;
// Array of LED pins
const int ledPins[NUM_LEDS] = {5, 6, 9, 10, 11, 12};

//temp variables
const int temp_pin = 16;     //analog input pin constant - A2 on board
float volts;    // variable for storing voltage
#define VREF 3.3
#define ADC_MAX 4095.0
float initial_temp;

//these are all used for the button mode
int mode;
int prev_mode = 0;
int button_state;
int last_button_state = 1;
unsigned long last_debounce_millis = 0;
unsigned long prev_millis = 0;
const unsigned long debounce_delay = 10;
//use this to toggle wether the leds are controlled by button or not
bool button_control = false;

// Time delay between each LED change (in milliseconds)
// can be changed
int delay_time = 200;

//define methods
void all_off();
void red();
void yellow();
void green();
void chase_sequence();
void rainbow();
void blink();
void controlled_by_temp(float initial_temp);
int check_mode_button(int prev_mode);
void controlled_by_button();
void sendTemp();
void getPattern();



//LIGHT PATTERN MODES 

//turns all leds off
void all_off() {
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], LOW); // Ensure all LEDs are off initially
  }
}

//just red
void red() {
  Serial.printf(" red ");
  digitalWrite(ledPins[0], HIGH);
  digitalWrite(ledPins[1], HIGH);
}

//just yellow
void yellow() {
  Serial.printf(" yellow ");
  digitalWrite(ledPins[2], HIGH);
  digitalWrite(ledPins[3], HIGH);
}

//just green
void green() {
  Serial.printf(" green ");
  digitalWrite(ledPins[4], HIGH);
  digitalWrite(ledPins[5], HIGH);
}

void chase_sequence() {
  Serial.printf("chase sequence\n");
  // Turn on each LED in sequence
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], HIGH); // Turn on the LED
    delay(delay_time);               // Wait
    digitalWrite(ledPins[i], LOW);  // Turn off the LED
  }

  // Turn on each LED in reverse sequence
  for (int i = NUM_LEDS - 1; i >= 0; i--) {
    digitalWrite(ledPins[i], HIGH); // Turn on the LED
    delay(delay_time);               // Wait
    digitalWrite(ledPins[i], LOW);  // Turn off the LED
  }
}

//red then yellow then green then off
void rainbow() {
  Serial.printf("rainbow\n");
  red();
  delay(delay_time * 5);
  yellow();
  delay(delay_time * 5);
  green();
  delay(delay_time * 5);
  all_off();
  delay(delay_time);
}

//all lights flash at same time
void blink() {
  Serial.printf("blink\n");
  for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(ledPins[i], HIGH);
    }

    delay(delay_time);

    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(ledPins[i], LOW);
    }

    delay(delay_time);
}

// this makes the lights blink faster if temp is higher than
// initial temp. can be changed to different modes
void controlled_by_temp(float initial_temp) {
  int temp_val = analogRead(temp_pin);
  volts = temp_val * VREF /ADC_MAX ;
  volts = temp_val / 1023.0;

  float temp_celcius = (volts - 0.5) * 100.0 ;
  Serial.printf(" Temperature is:   ");
  Serial.print(temp_celcius);
  Serial.printf (" degrees C\n");

  if (temp_celcius > initial_temp) {
    delay_time = 90;
    Serial.printf("higher temp\n");
  } else if (temp_celcius < initial_temp) {
    delay_time = 350;
    Serial.printf("lower temp\n");
  }
  blink();
  delay(delay_time); 
}

//used to change led mode via button
//have to hold the button down for a while and it doesnt always respond but its not a requirement anyway
int check_mode_button(int prevMode) {
  int button_reading = digitalRead(BUTTON_PIN);
  if (button_reading != last_button_state) {
    last_debounce_millis = millis();
  }
  if ((millis() - last_debounce_millis) > debounce_delay) {
    if (button_reading != button_state) {
      button_state = button_reading;
      if (button_state == LOW) {
        int last_mode = prev_mode;
        Serial.print("\n\nlast mode: ");
        Serial.println(last_mode);
        mode++;
        Serial.print("button pressed: ");
        Serial.print("current mode = ");
        Serial.println(mode);
        Serial.println();
      }
      if (mode >= 7) mode = 0;
      else if (mode == 0) Serial.println("\nreset to mode 0");
      all_off();
    }
  }
  last_button_state = button_reading;
  return mode;
}

//method to run in loop to controll the led mode by physical button
void controlled_by_button() {
  delay(delay_time * 3);
  int current_mode = check_mode_button(prev_mode);

  switch (current_mode) {
    case 0:
      all_off();
      break;
    case 1:
      chase_sequence();
      break;
    case 2:
      rainbow();
      break;
    case 3:
      blink();
      break;
    case 4:
      red();
      break;
    case 5:
      yellow();
      break;
    case 6:
      green();
      break;
    default:
      break;
  }
  prev_mode = mode;
}
void sendTemp() {
  HTTPClient http;
  http.begin("http://192.168.240.235:5000/data");
  http.addHeader("Content-Type", "application/json");

  //read temperature from sensor
  int adcValue = analogRead(TEMP_PIN);
  float voltage = adcValue * VREF / ADC_MAX;
  float tempC = (voltage - 0.5) * 100.0;
  float temperature = tempC;

  //send temperature to website
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST("{\"temperature\": " + String(temperature) + ", \"useTempControl\": " + String(useTempControl) + ", \"delayTime\": " + String(delay_time) + "}");

  http.end();
}

void getPattern() {
  HTTPClient http;
  http.begin("http://192.168.240.235:5000/pattern");
  // This function can be used to get the current pattern from the server
  int code = http.GET();
  //interpret response from website
  if (code == 200) {
    String buttonResponse = http.getString();
    Serial.printf("Button response: %s\n", buttonResponse.c_str());
    if (buttonResponse == "Rainbow") { 
      rainbow();
    }
    if (buttonResponse == "Blink") {
      blink();
    }
    if (buttonResponse == "Chase") {
      chase_sequence();
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
  Serial.println("Server Address: http://192.168.240.235:5000/data");
  
  
  // Initialize each pin as an output
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // Ensure all LEDs are off initially
  }
  // Initialize button as an input
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  mode = 0;

  analogReadResolution(12); // 12-bit ADC
  analogSetPinAttenuation(temp_pin, ADC_11db);

  //read initial temp for temp controll mode
  int temp_val = analogRead(temp_pin);
  volts = temp_val * VREF /ADC_MAX ;
  volts = temp_val / 1023.0;

  initial_temp = (volts - 0.5) * 100.0 ;

  //this sequence lets you know the code is running
  //leds go red -> yellow -> green then off before loop starts
  red();
  delay(delay_time);
  yellow();
  delay(delay_time);
  green();
  delay(delay_time);
  all_off();
  delay(delay_time);


}


void loop() {
  //define Clients
  WiFiClient client;
  HTTPClient http;

  //beggining connection to website
  http.begin(client, "http://192.168.240.235:5000/send_pattern");
  sendTemp();
  getPattern();

  
  delay(2000);

}