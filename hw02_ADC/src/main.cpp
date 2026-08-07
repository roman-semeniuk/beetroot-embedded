#include <Arduino.h>

const int adcPin = 4;

int rawValue = 0;
float calcVoltage = 0.0;
int factPinVoltage = 0;
float calcError = 0.0;

unsigned long lastADCReading = 0;
const int readingInterval = 100;

unsigned long lastPrint = 0;
const int printInterval = 1000;

void setup() {
  Serial.begin(115200);
  analogSetAttenuation(ADC_11db);
  delay(2000);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if ( currentMillis - lastADCReading > readingInterval) {
    rawValue = analogRead(adcPin);
    calcVoltage = rawValue * (3.3 / 4095.0);
    factPinVoltage = analogReadMilliVolts(adcPin);

    if (factPinVoltage > 0) {
      calcError = (abs((calcVoltage * 1000) - factPinVoltage) / (float)factPinVoltage) * 100.0;
    } else {
      calcError = 0.0;
    }
    lastADCReading = millis();
  }

  if ( currentMillis - lastPrint > printInterval) {
    Serial.printf("Raw: %d | CalcVoltage: %.2f V | FactVoltage: %d mV | CalcError: %.1f %%\n",
    rawValue, calcVoltage, factPinVoltage, calcError);
    lastPrint = millis();
  }
}