#include <Arduino.h>

// 1. DECLARE VARIABLES FIRST 
// (Now the compiler knows they exist before anyone tries to use them)
volatile uint32_t isr_counter = 0;
volatile bool isr_running = true; // Moved this up here!
hw_timer_t * timer = NULL;

// 2. THE ISR CAN NOW SAFELY USE THE VARIABLE
void IRAM_ATTR onTimer() {
  // If you are modifying or reading 'isr_running' inside here,
  // the compiler is now totally fine with it.
  if (isr_running) {
    isr_counter++;
  }
}

void setup() {
  Serial.begin(115200);
  
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000, true); 
  timerAlarmEnable(timer);
}

void loop() {
  // 3. THE MAIN LOOP CAN ALSO SAFELY USE IT
  if (isr_running && millis() >= 5000) {
    timerAlarmDisable(timer); 
    isr_running = false;
    Serial.println("Timer alarm disabled. ISR stopped execution.");
  }
}