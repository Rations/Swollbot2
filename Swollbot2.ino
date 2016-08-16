#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

const int REFRESH     = 10;
const int VACUUM_PIN  = 2;
const int MASTER_JAM  = 3;
const int X           = A0;
const int Y           = A1;
const int AIR         = 13;
const int UP          = 12;
const int RIGHT       = 11;
const int DOWN        = 10;
const int LEFT        = 9;

const int STICK_DEADZONE  = 51;
const int STICK_ZERO      = 512;

// Declare motor shield objects.
Adafruit_MotorShield motorShield  = Adafruit_MotorShield();
Adafruit_DCMotor *vacuum          = motorShield.getMotor(4);
Adafruit_DCMotor *vacuum2         = motorShield.getMotor(3);
int BUTTON_DEADZONE  = 200;        // Prevents unintended switching when button is depressed for up to t = BUTTON_DEADZONE ms.
int vacuumState      = HIGH;       // Allows vacuum to be switched on and off alternatingly.
int vacuum2State     = HIGH;       // Allows vacuum2 to be switched on and off alternatingly.
long lastTimeVacuum  = 0;          // Stores last time vacuum switch (R2) was depressed.
long lastTimeVacuum2 = 0;          // Stores last time vacuum switch (R2) was depressed.


void setup() {
  motorShield.begin();
  pinMode(MASTER_JAM, INPUT);
  pinMode(VACUUM_PIN, INPUT);
  pinMode(AIR, OUTPUT);
  pinMode(X, INPUT);
  pinMode(Y, INPUT);
  pinMode(UP, OUTPUT);
  pinMode(RIGHT, OUTPUT);
  pinMode(DOWN, OUTPUT);
  pinMode(LEFT, OUTPUT);
  vacuum  -> run(RELEASE);
  vacuum2 -> run(RELEASE);
}

void loop() {
  // Read stick inputs.
  int x = stickPos(X);
  int y = stickPos(Y);
  identifyInput(x, y);
  vacuumButton();
  vacuum2Button();
//  masterJam();
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

// Read vacuum button. If vacuum button is depressed and at least t = BUTTON_DEADZONE ms has elapsed since last depression, switch vacuum on/off as necessary.
void vacuumButton() {
  long now = millis();
  if (digitalRead(VACUUM_PIN) == LOW && now - lastTimeVacuum > BUTTON_DEADZONE) {
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

void vacuum2Button() {
  long now = millis();
  if (digitalRead(MASTER_JAM) == LOW && now - lastTimeVacuum2 > BUTTON_DEADZONE) {
    if (vacuum2State == LOW) {
      vacuum2 -> run(FORWARD);
      vacuum2 -> setSpeed(255);
      vacuum2State = HIGH;
    } else {
      vacuum2 -> run(RELEASE);
      vacuum2State = LOW;
    }
  }
}

//void masterJam() {
//  if (digitalRead(MASTER_JAM) == LOW) {
//      controlJamming(1, 1, 1, 1);
//  }
//}
