#include <Arduino.h>

const int ledPin = 1;       // Output: Built-in blue LED
const int switchPin = 4;    // Input: GPIO 4 connected to the switch

void setup() {
    pinMode(ledPin, OUTPUT);
    
    // INPUT_PULLUP holds the pin at 3.3V (HIGH) when the switch is open
    pinMode(switchPin, INPUT_PULLUP); 
}

void loop() {
    int switchState = digitalRead(switchPin);
    
    // When the switch is turned ON/CLOSED, it connects GPIO 4 to GND (LOW)
    if (switchState == LOW) {
        digitalWrite(ledPin, LOW);  // Turn the LED OFF
    } else {
        digitalWrite(ledPin, HIGH); // Turn the LED ON
    }
}