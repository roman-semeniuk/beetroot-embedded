#include <Arduino.h>

constexpr uint8_t RED_LED_PIN = 14;
constexpr uint8_t BLUE_LED_PIN = 15;
constexpr uint8_t GREEN_LED_PIN = 16;

constexpr uint32_t RED_BLINK_INTERVAL = 200;
constexpr uint32_t BLUE_BLINK_INTERVAL = 500;
constexpr uint32_t GREEN_BLINK_INTERVAL = 1000;

class Led {
  private:
    const uint8_t ledPin;
    uint8_t currentState;
    uint32_t toggleInterval;
    uint32_t lastToggleMillis;

  public:
    Led (uint8_t pin, uint8_t state, uint32_t interval)
    : ledPin(pin),
      currentState(state),
      toggleInterval(interval),
      lastToggleMillis(0) {}
  
    void init () {
      pinMode(ledPin, OUTPUT);
      set(currentState);
    }

    void set (uint8_t state) {
      currentState = state;
      digitalWrite(ledPin, state);
    }
    
    void toggle(const uint32_t currentMillis) {
      if (currentMillis - lastToggleMillis >= toggleInterval) {
        uint8_t newState = (currentState == HIGH) ? LOW : HIGH;
        set(newState);
        lastToggleMillis = currentMillis;
      }
    }
};

static Led redLed{RED_LED_PIN, LOW, RED_BLINK_INTERVAL};
static Led blueLed{BLUE_LED_PIN, LOW, BLUE_BLINK_INTERVAL};
static Led greenLed{GREEN_LED_PIN, LOW, GREEN_BLINK_INTERVAL};

void setup() {
  redLed.init();
  blueLed.init();
  greenLed.init();
}

void loop() {
  const uint32_t currentMillis = millis();
  redLed.toggle(currentMillis);
  blueLed.toggle(currentMillis);
  greenLed.toggle(currentMillis);
}