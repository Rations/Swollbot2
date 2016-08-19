#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

const int REFRESH        = 10;    // Loop refresh rate
const int BUTTON_REFRESH = 200;   // Prevents unintended switching when button is depressed for up to t = BUTTON_REFRESH ms.
const int STICK_DEADZONE = 51;
const int STICK_ZERO     = 512;

// Assign Arduino pins to stick axes and buttons.
const int L_STICK = 2;     // Left-hand stick button
const int R_STICK = 3;     // Right-hand stick button
const int X       = A0;    // Center stick horizontal axis
const int Y       = A1;    // Center stick vertical axis
const int RX      = A2;    // Right-hand stick horizontal axis
const int RY      = A3;    // Right-hand stick vertical axis
const int AIR     = 13;
const int UP      = 12;
const int RIGHT   = 11;
const int DOWN    = 10;
const int LEFT    = 9;

// Declare motor shield objects.
Adafruit_MotorShield motorShield  = Adafruit_MotorShield();
Adafruit_DCMotor *vacuum          = motorShield.getMotor(4);

int vacuumState     = HIGH;     // Represents operating state of vacuum.
long lastTimeVacuum = 0;        // Stores last time vacuum was switched.

void setup() {
  motorShield.begin();
  pinMode(L_STICK, INPUT);
  pinMode(R_STICK, INPUT);
  pinMode(X, INPUT);
  pinMode(Y, INPUT);
  pinMode(AIR, OUTPUT);
  pinMode(UP, OUTPUT);
  pinMode(RIGHT, OUTPUT);
  pinMode(DOWN, OUTPUT);
  pinMode(LEFT, OUTPUT);
  vacuum -> run(RELEASE);
}

void loop() {
  int x  = stickPos(X);
  int y  = stickPos(Y);
  int rx = stickPos(RX);
  int ry = stickPos(RY);
  identifyInput(x, y);
  vacuumButton(L_STICK);
  piston(rx, ry);
  delay(REFRESH);
}

int stickPos(int axis) {
  int reading = analogRead(axis);
  if (reading > STICK_ZERO + STICK_DEADZONE) {
    return (1);
  }
  else if (reading < STICK_ZERO - STICK_DEADZONE) {
    return (-1);
  }
  else {
    return (0);
  }
}

void identifyInput(int x, int y) {
  if (x == 0 && y == 1) {
    controlJamming(0, 1, 1, 1);
  }
  if (x == 1 && y == 1) {
    controlJamming(0, 0, 1, 1);
  }
  if (x == 1 && y == 0) {
    controlJamming(1, 0, 1, 1);
  }
  if (x == 1 && y == -1) {
    controlJamming(1, 0, 0, 1);
  }
  if (x == 0 && y == -1) {
    controlJamming(1, 1, 0, 1);
  }
  if (x == -1 && y == -1) {
    controlJamming(1, 1, 0, 0);
  }
  if (x == -1 && y == 0) {
    controlJamming(1, 1, 1, 0);
  }
  if (x == -1 && y == 1) {
    controlJamming(0, 1, 1, 0);
  }
}

void controlJamming(int up, int right, int down, int left) {
  jam(up, UP);
  jam(right, RIGHT);
  jam(down, DOWN);
  jam(left, LEFT);
}

void jam(int state, int pin) {
  if (state == 1) {
    digitalWrite(pin, HIGH);
  } else {
    digitalWrite(pin, LOW);
  }
}

// Read vacuum button. If button is depressed and at least t = BUTTON_REFRESH ms has elapsed since last depression, switch vacuum on/off as necessary.
void vacuumButton(int pin) {
  long now = millis();
  if (digitalRead(pin) == LOW && now - lastTimeVacuum > BUTTON_REFRESH) {
    if (vacuumState == LOW) {
      vacuum -> run(FORWARD);
      vacuum -> setSpeed(255);
      vacuumState = HIGH;
    } else {
      vacuum -> run(RELEASE);
      vacuumState = LOW;
    }
  }
}

// Extend (retract) piston given upwards (downwards) stick input. Jam all cells given any horizontal stick input. 
void piston(int x, int y) {
  if (y == -1) {
    digitalWrite(AIR, HIGH);
  } else if (y == 1) {
    digitalWrite(AIR, LOW);
  }
  if (x != 0) {
    controlJamming(1, 1, 1, 1);
  }
}
