#include <Arduino.h>

struct Config {
  static constexpr uint8_t RED_LED_PIN = 15;
  static constexpr uint32_t BLINKING_INTERVAL = 300;

  static constexpr uint8_t BUTTON_PIN = 16;
  static constexpr uint16_t DEBOUNCE_DELAY = 200;
};

volatile bool buttonPressed = false;
volatile uint32_t lastInterruptTime = 0;

enum class LedState : uint8_t {
    Off = LOW,
    On = HIGH
};

enum class LedMode : uint8_t {
    Blinking,
    AlwaysOn,
    AlwaysOff
};

class Led {
  private:
    const uint8_t ledPin;
    LedState currentState;

  public:
    Led (uint8_t pin)
    : ledPin(pin),
      currentState(LedState::Off) {}
  
    void init () {
      pinMode(ledPin, OUTPUT);
      set(currentState);
    }

    void set (LedState state) {
      currentState = state;
      digitalWrite(ledPin, static_cast<uint8_t>(state));
    }
    
    void toggle() {
      LedState newState = (currentState == LedState::On) ? LedState::Off : LedState::On;
      set(newState);
    }
};

static Led led{Config::RED_LED_PIN};

void IRAM_ATTR buttonPressISR() {
  const uint32_t currentMillis = millis();
  if (currentMillis - lastInterruptTime > Config::DEBOUNCE_DELAY) {
    buttonPressed = true;
    lastInterruptTime = currentMillis;
  }
}

void setup() {  
  led.init();

  pinMode(Config::BUTTON_PIN, INPUT);
  attachInterrupt(Config::BUTTON_PIN, buttonPressISR, RISING);
}

void loop() {
  const uint32_t currentMillis = millis();
  static uint32_t lastToggleMillis = 0;

  static LedMode currentMode = LedMode::Blinking;

 if (buttonPressed) {
    buttonPressed = false;
    switch (currentMode) {
      case LedMode::Blinking:
        currentMode = LedMode::AlwaysOn;
        led.set(LedState::On);
        break;
      case LedMode::AlwaysOn:
        currentMode = LedMode::AlwaysOff;
        led.set(LedState::Off);
        break;
      case LedMode::AlwaysOff:
        currentMode = LedMode::Blinking;
        break;
      default:
        break;
    }
  }
  if (currentMode == LedMode::Blinking && currentMillis - lastToggleMillis > Config::BLINKING_INTERVAL) {
    led.toggle();
    lastToggleMillis = currentMillis;
  }
}