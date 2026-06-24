#include <Arduino.h>

const int LED_PIN = 4;

const int pwmChannel = 0;     
const int freq = 5000;         
const int resolution = 8;    

void setup() {
    ledcSetup(pwmChannel, freq, resolution);
    ledcAttachPin(LED_PIN, pwmChannel);
}

void loop() {
    for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++) {
        ledcWrite(pwmChannel, dutyCycle);
        delay(7); 
    }

    for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle--) {
        ledcWrite(pwmChannel, dutyCycle);
        delay(7);
    }
    
    delay(500); 
}