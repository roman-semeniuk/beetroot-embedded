#include <Arduino.h>

#define buttonPin 15
#define bounceTimeout 100

volatile int risesCount = 0;
volatile unsigned long riseMillis = 0;

int pressesCount = 0;
unsigned long currentMillis = 0;

void IRAM_ATTR riseDetection() {
  risesCount++;
  riseMillis = millis();
}

void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(115200);
  // Button module hardware pulled-down, signal rises when pressed.
  attachInterrupt(digitalPinToInterrupt(buttonPin), riseDetection, RISING);
}

void loop() {
  currentMillis = millis();
  // risesCount counts all RISING edges - both press bounce and release bounce.
  if (risesCount > 0 && currentMillis - riseMillis > bounceTimeout && digitalRead(buttonPin) == LOW) {
    pressesCount++;
    Serial.println("\nOn " + String(pressesCount) + " press detected " + String(risesCount) + " rises");
    risesCount = 0;
  }
}