#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

const int REFRESH         = 10;   // Loop update rate.

// Stick pins.
const int VACUUM_PIN = 2;
const int X     = A0;
const int Y     = A1;
const int UP    = 1;
const int RIGHT = 2;
const int DOWN  = 3;
const int LEFT  = 4;

const int STICK_DEADZONE  = 51;
const int STICK_ZERO      = 512; 

// Declare motor shield objects.
Adafruit_MotorShield motorShield  = Adafruit_MotorShield();
Adafruit_DCMotor *vacuum          = motorShield.getMotor(4);
int VACUUM_DEADZONE = 200;                                     // Prevents unintended switching when button is depressed for up to t = VACUUM_DEADZONE ms. 
int vacuumState     = HIGH;                                       // Allows vacuum to be switched on and off alternatingly. 
long lastTime       = 0;                                              // Stores last time vacuum switch (R2) was depressed.

void setup() {
  motorShield.begin();
  pinMode(X, INPUT);
  pinMode(Y, INPUT);
  pinMode(UP, OUTPUT);
  pinMode(RIGHT, OUTPUT);
  pinMode(DOWN, OUTPUT);
  pinMode(LEFT, OUTPUT);
  vacuum -> run(RELEASE);
}

void loop() {
  // Read stick inputs.
  int x = stickPos(X);
  int y = stickPos(Y);
  identifyInput(x, y);
  button();
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
    jam(up, 1);
    jam(right, 2);
    jam(down, 3);
    jam(left, 4);
}
 
void jam(int state, int pin) {
  if (state == 1) {
    digitalWrite(pin, HIGH);
  } else {
    digitalWrite(pin, LOW);
  }
}

// Read vacuum button. If vacuum button is depressed and at least t = VACUUM_DEADZONE ms has elapsed since last depression, switch vacuum on/off as necessary.
void button() {
  long currentTime = millis();
  int vacuumButtonState = digitalRead(VACUUM_PIN);
  if (vacuumButtonState == LOW && currentTime - lastTime > VACUUM_DEADZONE) {
    if (vacuumState == LOW) {
      vacuum -> run(FORWARD);                 
      vacuum -> setSpeed(255);
      vacuumState = HIGH;
    } else {
      vacuum -> run(RELEASE);
      vacuumState = LOW;
    }
    lastTime = millis();
  }
}

