const int PIN_TO_SENSOR = 36;  // GPIO32 pin connected to INPUT sensor
const int OUTPUT_PIN = 27;     // GPIO27 pin connected to OUTPUT
const int fingerprintInputPin = 39; // Set the pin connected to the fingerprint sensor output on ESP32 (change as per your connection)

int pinStateCurrent2 = LOW;      // current state of pin
int pinStatePrevious2 = LOW;     // previous state of pin

int pinStateCurrent = LOW;      // current state of pin
int pinStatePrevious = LOW;     // previous state of pin

void setup() {
  pinMode(PIN_TO_SENSOR, INPUT); // set ESP32 pin to input mode to read value from sensor
  pinMode(OUTPUT_PIN, OUTPUT);   // set ESP32 pin to output mode to send value to another pin
  pinMode(fingerprintInputPin, INPUT);

}

void loop() {
  pinStatePrevious = pinStateCurrent; // store old state
  pinStateCurrent = digitalRead(PIN_TO_SENSOR);   // read new state

  pinStatePrevious2 = pinStateCurrent2; // store old state
  pinStateCurrent2 = digitalRead(fingerprintInputPin);   // read new state


  if (pinStateCurrent2 == HIGH) {
    digitalWrite(OUTPUT_PIN, LOW);  // send the input state to the output pin
    Serial.println("Valid fingerprint found!");
    Serial.println("Motion sensor is off");
    Serial.println("You can now enter");
    for(int i = 1; i<=20; i++){
      delay(1000); // Adjust delay as needed
    }
  } else if (pinStateCurrent2 == LOW){
    digitalWrite(OUTPUT_PIN, HIGH);  // send the input state to the output pin
    Serial.println("No valid fingerprint found.");
    Serial.println("Motion sensor is ON");
  }
  delay(200);

}
