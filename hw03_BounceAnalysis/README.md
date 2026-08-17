# ESP32 Bounce Analysis

A simple ESP32-S3 project for button contact bounce analysis.

## Connection
To capture bounce signal waveforms 24 MHz Logic analyzer connected in parallel with button module to ESP32-S3 GPIO pin 15.
To view and analyze received signals waveforms Saleae Logic was used.

## Code logic
An interrupt function on `RISING` edges increments `risesCount`.
When the button is released and signal stays quiet for `bounceTimeout`, total `risesCount` of a press (press + release bounces) is printed to Serial.

## Results
Button pressed 10 times with different speed and force:
| Press # | MCU Detected Rises | Logic Analyzer Rises |
|:-------:|:------------------:|:--------------------:|
| 1 | 2 | 2 |
| 2 | 3 | 2 |
| 3 | 4 | 3 |
| 4 | 5 | 4 |
| 5 | 5 | 4 |
| 6 | 1 | 1 |
| 7 | 1 | 1 |
| 8 | 1 | 1 |
| 9 | 1 | 1 |
| 10 | 1 | 1 |

**Received serial info**

![received_serial](https://github.com/user-attachments/assets/31bf91d7-46b0-46fe-896d-4df40f024eed)

**Recorded button presses signal waveform**

![presses_waveform](https://github.com/user-attachments/assets/11a0a708-8ee9-4153-9443-51a1d60a555b)

Serial and analyzer outputs show that the microcontroller detects slightly more pulses than the logic analyzer on faster, stronger presses.
Based on the Saleae Logic 2 measurements, the contact bounce duration is, on average, 240–250 µs, so a minimal software debounce delay of 5–10 ms is sufficient.
