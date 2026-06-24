#include <Arduino.h>

const int LIGHT_PIN = 4; 

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(LIGHT_PIN, OUTPUT);
    digitalWrite(LIGHT_PIN, LOW);

    Serial.println("--- ESP32 GPIO 4 Control ---");
    Serial.println("Type 'on' to turn the light ON.");
    Serial.println("Type 'off' to turn the light OFF.");
    Serial.println("----------------------------");
}

void loop() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();        
        input.toLowerCase(); 
        
        if (input == "on") {
            digitalWrite(LIGHT_PIN, HIGH);
            Serial.println("Success: GPIO 4 is now ON");
        } 
        else if (input == "off") {
            digitalWrite(LIGHT_PIN, LOW);
            Serial.println("Success: GPIO 4 is now OFF");
        } 
        else {
            Serial.println("Error: Unknown command. Type 'on' or 'off'.");
        }
    }
}