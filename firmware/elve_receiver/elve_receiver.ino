#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <ESP32Servo.h>

Servo servo;

// -----------------------------
// ESP-NOW
// -----------------------------

const uint8_t controllerMAC[6] = {
  0x00, 0x70, 0x07, 0xA5, 0xC0, 0xC0
};

// -----------------------------
// Servo
// -----------------------------

const int SERVO_PIN = 12;

// YOUR calibrated servo values
const int SERVO_LEFT_US  = 1800;
const int SERVO_STOP_US  = 2145;
const int SERVO_RIGHT_US = 2500;

const int SERVO_UP_US   = SERVO_RIGHT_US;
const int SERVO_DOWN_US = SERVO_LEFT_US;

bool servoAttached = false;

// -----------------------------
// Elevator
// -----------------------------

const int MIN_FLOOR = 1;
const int MAX_FLOOR = 10;

// Calibrate this
const unsigned long MS_PER_FLOOR = 400;

int currentFloor = 1;

volatile int requestedFloor = 0;

// -----------------------------
// Servo control
// -----------------------------

void attachServo() {
  if (!servoAttached) {
    servo.setPeriodHertz(50);

    servo.attach(
      SERVO_PIN,
      500,
      2500
    );

    servoAttached = true;

    delay(20);
  }
}

void moveUp() {
  attachServo();
  servo.writeMicroseconds(SERVO_UP_US);
  Serial.println("Servo: UP");
}

void moveDown() {
  attachServo();
  servo.writeMicroseconds(SERVO_DOWN_US);
  Serial.println("Servo: DOWN");
}

void stopServo() {
  if (!servoAttached) {
    return;
  }

  // First command the servo to stop
  servo.writeMicroseconds(SERVO_STOP_US);

  // Give it a moment to react
  delay(100);

  // Completely remove PWM signal
  servo.detach();
  servoAttached = false;

  // Signal pin no longer actively driven
  pinMode(SERVO_PIN, INPUT);

  Serial.println("Servo: STOPPED + SIGNAL DETACHED");
}

// -----------------------------
// ESP-NOW
// -----------------------------

void onFloor(
  const esp_now_recv_info_t *info,
  const uint8_t *data,
  int len
) {
  if (!info) {
    return;
  }

  if (memcmp(info->src_addr, controllerMAC, 6) != 0) {
    return;
  }

  if (len != 1) {
    return;
  }

  int floor = data[0];

  if (
    floor >= MIN_FLOOR &&
    floor <= MAX_FLOOR
  ) {
    requestedFloor = floor;
  }
}

// -----------------------------
// Elevator movement
// -----------------------------

void moveToFloor(int targetFloor) {
  if (targetFloor == currentFloor) {
    Serial.print("Already at floor ");
    Serial.println(currentFloor);

    stopServo();

    return;
  }

  int difference =
    targetFloor - currentFloor;

  int floorsToMove =
    abs(difference);

  unsigned long moveTime =
    floorsToMove * MS_PER_FLOOR;

  Serial.println();

  Serial.print("Current floor: ");
  Serial.println(currentFloor);

  Serial.print("Target floor: ");
  Serial.println(targetFloor);

  Serial.print("Floors: ");
  Serial.println(floorsToMove);

  if (difference > 0) {
    Serial.println("Direction: UP");
    moveUp();
  }

  else {
    Serial.println("Direction: DOWN");
    moveDown();
  }

  Serial.print("Moving for ");
  Serial.print(moveTime);
  Serial.println(" ms");

  delay(moveTime);

  // Stop AND remove signal
  stopServo();

  currentFloor = targetFloor;

  Serial.print("Arrived at floor ");
  Serial.println(currentFloor);

  Serial.println();
}

// -----------------------------
// Setup
// -----------------------------

void setup() {
  Serial.begin(115200);

  delay(500);

  // Start servo briefly at neutral
  attachServo();
  servo.writeMicroseconds(SERVO_STOP_US);
  delay(200);
  stopServo();

  // -----------------------------
  // ESP-NOW
  // -----------------------------

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);

  if (
    esp_wifi_set_channel(
      1,
      WIFI_SECOND_CHAN_NONE
    ) != ESP_OK
  ) {
    Serial.println("WiFi channel setup failed");
    return;
  }

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW setup failed");
    return;
  }

  esp_now_register_recv_cb(onFloor);

  Serial.println();
  Serial.println("===============================");
  Serial.println("ELEVATOR RECEIVER READY");
  Serial.println("===============================");

  Serial.print("Receiver MAC: ");
  Serial.println(WiFi.macAddress());

  Serial.print("Starting floor: ");
  Serial.println(currentFloor);

  Serial.println("Servo signal currently DETACHED");
  Serial.println();
}

// -----------------------------
// Loop
// -----------------------------

void loop() {
  if (requestedFloor != 0) {
    int targetFloor = requestedFloor;

    requestedFloor = 0;

    moveToFloor(targetFloor);
  }

  delay(10);
}
