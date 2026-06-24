#include <Arduino.h>

bool stopCounting = false;
//volatile bool stopCounting = false;

unsigned long counter = 0;

// Core 0: Increments a counter as fast as humanly possible
void taskCore0(void *pvParameters) {
  Serial.println("Core 0: High-speed counter started...");
  
  // A tight, blazing-fast loop with NO delays or functions inside
  while (!stopCounting) {
    counter++;
  }

  Serial.print("Core 0: Successfully stopped! Final Count: ");
  Serial.println(counter);
  vTaskDelete(NULL);
}

// Core 1: Waits 2 seconds, then tries to pull the emergency brake
void taskCore1(void *pvParameters) {
  vTaskDelay(pdMS_TO_TICKS(2000)); // Sleep for 2 seconds
  
  Serial.println("\n--------------------------------------------------");
  Serial.println("Core 1: Time's up! Changing stopCounting to TRUE...");
  Serial.println("--------------------------------------------------\n");
  
  stopCounting = true; 
  
  // Let's wait a bit to see if Core 0 actually listened to us
  vTaskDelay(pdMS_TO_TICKS(1000));
  
  Serial.print("Core 1: Checked counter status. Current value: ");
  Serial.println(counter);
  Serial.println("Core 1: If the count above is still increasing, Core 0 is stuck forever!");
  
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  xTaskCreatePinnedToCore(taskCore0, "Core0_Task", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(taskCore1, "Core1_Task", 2048, NULL, 1, NULL, 1);
}

void loop() {
  vTaskSuspend(NULL);
}