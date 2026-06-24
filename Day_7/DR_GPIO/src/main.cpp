#include <Arduino.h>

#define LED_PIN 2

const uint32_t pin_mask = (1UL << LED_PIN);

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  while (true) {
        GPIO.out_w1ts = (pin_mask); // W1TS: Write 1 To Set (High)
        delay(100);
        GPIO.out_w1tc = (pin_mask); // W1TC: Write 1 To Clear (Low)
        delay(100);
  }
}

