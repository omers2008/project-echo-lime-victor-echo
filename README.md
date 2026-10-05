# Project Echo Lime Victor Echo

**Project ELVE** is an ESP32 elevator project using ESP-NOW and a continuous-rotation servo.

## Receiver

The receiver firmware is in `firmware/elve_receiver/elve_receiver.ino` and the transmitter code is in my pocket arcade esp project`https://github.com/omers2008/esp32-pocket-arcade' as its one of its apps. It receives a floor number over ESP-NOW and moves the elevator for a calibrated amount of time per floor.

- Board: ESP32 WROOM-32D.
- Servo signal: GPIO 12, 50 Hz PWM.
- ESP-NOW channel: 1.
- Accepted sender: controller MAC `00:70:07:A5:C0:C0`.
- ESP-NOW payload: one byte containing a floor number from 1 through 10.
- Servo pulses: LEFT 1800 us, STOP **2145 us**, RIGHT 2500 us.
- Direction mapping: UP uses 2500 us; DOWN uses 1800 us.
- Travel calibration: 400 ms per floor.
- Starting floor: 1.
- After stopping, the receiver detaches the servo signal to prevent unwanted movement.
- Serial monitor: 115200 baud.

The 2145 us neutral value is specific to the tested servo. LEFT and RIGHT describe the software commands; physical direction depends on the installation.

## Build and upload

Open `firmware/elve_receiver/elve_receiver.ino` in Arduino IDE. Install the ESP32 board package and the ESP32Servo library, select the matching ESP32 board and serial port, then compile and upload. The receive callback uses the `esp_now_recv_info_t` API.

## Project status

The receiver tracks its current floor in memory and accepts requests for floors 1–10. The 2145 us neutral calibration remains the tested stop value. The transmitter sketch has not been located and is not included in this repository.
