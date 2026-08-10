#include <Arduino.h>

#define bootButton 0
#define buttonPin 15
#define redLED 16
#define blueLED 17

void readButtons();
int blinkingSpeedSwitch();
void blinkLEDs(int currentBlinkInterval);

unsigned long currentMillis = 0;

int extButtonState = LOW; // Connected button: Hardware pulled-down (LOW by default) via module resistor.
int bootButtonState = HIGH; // BOOT button: Hardware pulled-up (HIGH by default) via onboard resistor.

void setup() {
  pinMode(bootButton, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);

  digitalWrite(redLED, HIGH);
  digitalWrite(blueLED, LOW);
}

void loop() {
  currentMillis = millis();

  readButtons();
  int currentBlinkInterval = blinkingSpeedSwitch();
  blinkLEDs(currentBlinkInterval);
}

// Reads button states with non-blocking software debounce
void readButtons () {
  static int lastExtButtonState = HIGH;
  static int lastBootButtonState = HIGH;

  static unsigned long lastExtDebounceMillis = 0;
  static unsigned long lastBootDebounceMillis = 0;

  static int debounceDelay = 50;

  int extReading = digitalRead(buttonPin); // external button reading
  int bootReading = digitalRead(bootButton); // BOOT button reading

  if (extReading != lastExtButtonState) {
    lastExtDebounceMillis = currentMillis;
  }
  if (bootReading != lastBootButtonState) {
    lastBootDebounceMillis = currentMillis;
  }

  if ((currentMillis - lastExtDebounceMillis) > debounceDelay) {
    if (extReading != extButtonState) {
      extButtonState = extReading;
    }
  }
  if ((currentMillis - lastBootDebounceMillis) > debounceDelay) {
    if (bootReading != bootButtonState) {
      bootButtonState = bootReading;
    }
  }
  lastExtButtonState = extReading;
  lastBootButtonState = bootReading;
}

// Determines the blinking interval based on button states
int blinkingSpeedSwitch() {
  static int currentInterval = 1000;

  const int normalBlinkInterval = 1000;
  const int slowBlinkInterval = 2000;
  const int fastBlinkInterval = 300;

  if (extButtonState == HIGH && bootButtonState == LOW) {
    currentInterval = normalBlinkInterval;
  } else if (extButtonState == HIGH) {
    currentInterval = fastBlinkInterval;
  } else if (bootButtonState == LOW) {
    currentInterval = slowBlinkInterval;
  }
  return currentInterval;
}

// Toggles LEDs alternatively using non-blocking timer
void blinkLEDs (int currentBlinkInterval) {
  static unsigned long lastSwap = 0;

  if ((currentMillis - lastSwap) > currentBlinkInterval) {
  digitalWrite(redLED, !digitalRead(redLED));
  digitalWrite(blueLED, !digitalRead(blueLED));
  lastSwap = currentMillis;
  }
}