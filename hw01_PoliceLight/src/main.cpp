#include <Arduino.h>

#define LED_RED_OUT 15
#define LED_BLUE_OUT 16

void setup() {
  pinMode(LED_RED_OUT, OUTPUT);
  pinMode(LED_BLUE_OUT, OUTPUT);
}

void loop() {
  digitalWrite(LED_BLUE_OUT, LOW);
  digitalWrite(LED_RED_OUT, HIGH);
  delay(500);
    
  digitalWrite(LED_RED_OUT, LOW);
  digitalWrite(LED_BLUE_OUT, HIGH);
  delay(500); 
}