# hw 2.2 - RelaySwitchTime

Measures the mechanical turn-on/turn-off delay and contact bounce of a relay.
Built as part of Module 2.2 Homework using **ESP-IDF** and **PlatformIO**.

## Demo

https://github.com/user-attachments/assets/450b5d04-f58b-4dcb-83d7-a8096a5d841c

## Implementation Details

* **Interrupt-based Timing:**
  The control pin is set and the timestamp is taken with `esp_timer_get_time()` (`1 µs` resolution).
  A GPIO interrupt on any edge of the contact pin records the time of the **first** and **last** edge.
  Delay = first edge − control signal; bounce = last edge − first edge.

* **Bounce Handling:**
  Bounce is not filtered out but measured: the ISR only stores timestamps,
  and a `500 ms` settle time after each switch guarantees the contact has stabilized before reading results.

* **BC547 Transistor:**
  The `5V` relay module cannot be switched off by a `3.3V` GPIO, so an NPN transistor (`BC547`) sits between them.
  It pulls the module input to `GND` (relay on) or releases it (relay off).

* **Statistics:**
  `10` on/off cycles, per-measurement results and averages are printed to the serial console.

## Pinout

| Component | Pin | Connected to / Notes |
| :--- | :--- | :--- |
| **ESP32-S3** | `GPIO 4` | Control output, via `1 kΩ` to `BC547` base |
| **ESP32-S3** | `GPIO 5` | Contact input, `INPUT_PULLUP`, interrupt on any edge |
| **BC547** | `Emitter` | `GND` |
| **BC547** | `Collector` | Relay module `IN` |
| **Relay module** | `VCC` | `5V` |
| **Relay module** | `GND` | Common ground with ESP32 |
| **Relay module** | `COM` | `GND` |
| **Relay module** | `NO` | `GPIO 5` |
| **Relay module** | `NC` | Not connected |

## Results

<img width="500" alt="Serial output" src="https://github.com/user-attachments/assets/e11f2a83-d192-4fa9-bc38-d76632959a37" />

No bounce on switch-off was observed without a load; under load it may vary.
