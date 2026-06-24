#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Define the pins (Changed Pin 34 to Pin 4 because 34 is Input-Only)
#define PIN_CORE0 2   // Core 0 controls GPIO 2
#define PIN_CORE1 4   // Core 1 controls GPIO 4

// Task for Core 0 - Blink pin 2 at 1 second intervals (500ms ON, 500ms OFF)
void taskCore0(void *pvParameters) {
  pinMode(PIN_CORE0, OUTPUT);
  
  while(1) {
    // Set pin 2 HIGH using direct register write (pins 0-31 use out_w1ts)
    GPIO.out_w1ts = (1 << PIN_CORE0);
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // Set pin 2 LOW
    GPIO.out_w1tc = (1 << PIN_CORE0);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

// Task for Core 1 - Blink pin 4 at 1.2 second intervals (600ms ON, 600ms OFF)
void taskCore1(void *pvParameters) {
  pinMode(PIN_CORE1, OUTPUT);
  
  while(1) {
    // Because PIN_CORE1 (4) is less than 32, we use out_w1ts instead of out1_w1ts!
    GPIO.out_w1ts = (1 << PIN_CORE1);
    vTaskDelay(pdMS_TO_TICKS(600));
    
    GPIO.out_w1tc = (1 << PIN_CORE1);
    vTaskDelay(pdMS_TO_TICKS(600));
  }
}

void setup() {
  delay(1000);
  
  // Create task pinned to Core 0
  xTaskCreatePinnedToCore(
    taskCore0,        // Task function
    "Task_Core0",     // Task name
    2048,             // Stack size
    NULL,             // Parameters
    1,                // Priority
    NULL,             // Task handle
    0                 // Core ID (0 = PRO_CPU)
  );
  
  // Create task pinned to Core 1
  xTaskCreatePinnedToCore(
    taskCore1,        // Task function
    "Task_Core1",     // Task name
    2048,             // Stack size
    NULL,             // Parameters
    1,                // Priority
    NULL,             // Task handle
    1                 // Core ID (1 = APP_CPU)
  );
}

void loop() {
  // Instead of suspending and locking up the core, 
  // delay for a long time to let FreeRTOS background tasks breathe cleanly.
  vTaskDelay(pdMS_TO_TICKS(10000)); 
}