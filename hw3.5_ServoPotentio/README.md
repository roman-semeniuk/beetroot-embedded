# hw 3.5 - ServoPotentio

1:1 proportional servo shaft position control using a potentiometer.
Built as part of Module 3.5 Homework using **ESP-IDF** and **PlatformIO**.

## Demo

https://github.com/user-attachments/assets/ec4bb264-1626-44ea-b851-87d3ca4af894


## Implementation Details

* **1:1 Mapping & Range Clamping:**
  Since the potentiometer has a `270°` mechanical travel while the servo operates within `180°`,
  the outer edges (`0°–45°` and `225°–270°`) are clamped as deadzones.
  The active central sector (`45°–225°`) maps directly to `0°–180°` of servo rotation without scaling distortion.

* **Hardware PWM (LEDC):**
  Uses a 14-bit timer resolution at `50 Hz` (`500–2400 µs` pulse width).

* **Non-blocking Logging:**
  Logs the current angle of servo to the serial console once per second via `esp_timer`, keeping the main control loop running smoothly at `50 Hz`.

## Pinout

| Component | ESP32 Pin | Notes |
| :--- | :--- | :--- |
| **Servo (Signal)** | `GPIO 4` | PWM signal (LEDC Channel 0) |
| **Potentiometer (Wiper)** | `GPIO 5` | `ADC1_CHANNEL_4`, 12-bit, `12 dB` attenuation |
| **Power** | `3.3V` / `5V` / `GND` | `3.3V` for potentiometer, `5V` for servo, common ground |
