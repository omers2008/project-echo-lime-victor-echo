# Project Echo Lime Victor Echo

**Project ELVE** is an ESP32 elevator project using ESP-NOW and a continuous-rotation servo.

## Receiver

The working receiver firmware is in `firmware/elve_receiver/elve_receiver.ino`, copied from the tested Arduino sketch. Its startup message has been corrected to report the actual calibrated stop value.

- Board: ESP32 WROOM-32D.
- Servo signal: GPIO 12, 50 Hz PWM.
- Local button: GPIO 27 to GND, using the internal pull-up; holding it commands RIGHT.
- ESP-NOW payload: one character, `L`, `R`, or `S`.
- Servo pulses: LEFT 1800 us, STOP **2145 us**, RIGHT 2500 us.
- Starts stopped. Stops after 300 ms without a valid message unless the local button is held.
- Serial monitor: 115200 baud.

The 2145 us neutral value is specific to the tested servo. LEFT and RIGHT describe the software commands; physical direction depends on the installation.

## Build and upload

Open `firmware/elve_receiver/elve_receiver.ino` in Arduino IDE. Install the ESP32 board package and the ESP32Servo library, select the matching ESP32 board and serial port, then compile and upload. The receive callback uses the `esp_now_recv_info_t` API.

## Project status

The user confirmed that the receiver works with the 2145 us neutral calibration. The transmitter sketch has not been located and is not included in this initial snapshot.
