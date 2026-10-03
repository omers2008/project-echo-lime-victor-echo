#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Servo.h>

Servo servo;

const int SERVO_PIN = 12;
const int BUTTON_PIN = 27;

// YOUR servo calibration
const int SERVO_LEFT_US  = 1800;
const int SERVO_STOP_US  = 2145;
const int SERVO_RIGHT_US = 2500;

struct Message {
  char command;
};

volatile char receivedCommand = 'S';
volatile unsigned long lastMessageTime = 0;

char currentCommand = '?';

const unsigned long CONNECTION_TIMEOUT = 300;

void OnDataRecv(
  const esp_now_recv_info_t *info,
  const uint8_t *data,
  int len
) {
  if (len != sizeof(Message)) {
    return;
  }

  Message msg;
  memcpy(&msg, data, sizeof(msg));

  if (
    msg.command == 'L' ||
    msg.command == 'R' ||
    msg.command == 'S'
  ) {
    receivedCommand = msg.command;
    lastMessageTime = millis();
  }
}

void controlServo(char command) {

  if (command == currentCommand) {
    return;
  }

  currentCommand = command;

  if (command == 'L') {

    servo.writeMicroseconds(SERVO_LEFT_US);

    Serial.print("LEFT -> ");
    Serial.println(SERVO_LEFT_US);
  }

  else if (command == 'R') {

    servo.writeMicroseconds(SERVO_RIGHT_US);

    Serial.print("RIGHT -> ");
    Serial.println(SERVO_RIGHT_US);
  }

  else {

    servo.writeMicroseconds(SERVO_STOP_US);

    Serial.print("STOP -> ");
    Serial.println(SERVO_STOP_US);
  }
}

void setup() {

  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN, 500, 2500);

  // Start stopped
  servo.writeMicroseconds(SERVO_STOP_US);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Receiver ready");
  Serial.println("LEFT  = 1800");
  Serial.println("STOP  = 2145");
  Serial.println("RIGHT = 2500");
}

void loop() {

  char command;

  // Local button = RIGHT
  if (digitalRead(BUTTON_PIN) == LOW) {
    command = 'R';
  }

  // Stop if connection is lost
  else if (millis() - lastMessageTime > CONNECTION_TIMEOUT) {
    command = 'S';
  }

  else {
    command = receivedCommand;
  }

  controlServo(command);

  delay(2);
}