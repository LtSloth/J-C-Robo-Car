/*
 * J & C Creations Arduino BLE Roaming Robot
 *
 * Designed for beginner robotics enthusiasts, this platform serves as a starting point into creating custom sketches to control the robot .
 *
 * This sketch allows for control of the J & C Roaming Robot platform via Bluetooth, using a Arduino Nano 33 BLE.
 * The Arduino acts as a Host device to receive commands from a phone or tablet, to control the Robot with manual controls or hit the autonomous button.
 * This allows the robot to roam around on its own and avoid obstacles.
*/


#include "RoboMotorDriver.h"
#include <NewPing.h>
#include <ArduinoBLE.h>

//** Motor Set Up */
ROBO_Motor motorR(9, 10);
ROBO_Motor motorL(5, 6);

//** HC-SR04 Sensor Set Up */
#define TRIGGER_PIN  7
#define ECHO_PIN     8
#define MAX_DISTANCE 200

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
unsigned int distance = 0;

// LED for debugging
#define LED_PIN 13

//**Bluetooth Low Energy Set Up */
//Service and Characteristic UUIDs
BLEService roboService("8aedd07b-4a7e-43fb-8716-d3eb9ead1805");
BLECharacteristic controlChar("8aedd07c-4a7e-43fb-8716-d3eb9ead1805", BLEWrite | BLERead, 1);

//State Variables
char currentCommand = 'T'; //Manual mode once connected
bool autonomousMode = false;

void setup() {
  // Setup LED for debugging
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, LOW);
  
  // Initialize motors first
  motorR.release();
  motorL.release();
  
  // Seed random
  randomSeed(analogRead(0));

  // Initialize BLE - don't wait for Serial
  if(!BLE.begin()) {
    // Blink LED rapidly if BLE fails
    while(1) {
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDG, LOW);
      digitalWrite(LEDB, LOW);
      delay(100);
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDB, HIGH);
      delay(100);
    }
  }

  // BLE started successfully - quick blink
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);

  //Set Device Name
  BLE.setLocalName("RoboControllo");
  BLE.setAdvertisedService(roboService);

  //Add characteristic to Service
  roboService.addCharacteristic(controlChar);

  //Add Service
  BLE.addService(roboService);

  //Initialize Advertising
  BLE.advertise();
}

void loop() {
  //Wait for connection
  BLEDevice central = BLE.central();

  if(central) {
    //While connected to central check if data was written to the characteristic
    while(central.connected()) {
      if(controlChar.written()) {
        char cmd = controlChar.value()[0];
        
        // Blink LED when command received
        digitalWrite(LEDR, LOW);
        digitalWrite(LEDG, LOW);
        digitalWrite(LEDB, LOW);
        delay(50);
        digitalWrite(LEDR, HIGH);
        digitalWrite(LEDG, HIGH);
        digitalWrite(LEDB, HIGH);
        
        // Check mode FIRST before processing commands
        if(cmd == 'A') {
          // Switch to autonomous mode
          autonomousMode = true;
          // Double blink for autonomous mode
          digitalWrite(LED_PIN, HIGH);
          delay(100);
          digitalWrite(LED_PIN, LOW);
          delay(100);
          digitalWrite(LED_PIN, HIGH);
          delay(100);
          digitalWrite(LED_PIN, LOW);
        } 
        else if(cmd == 'T' || cmd == 'S') {
          // Switch to manual mode - always allow this
          autonomousMode = false;
          motorR.release();
          motorL.release();
          // Triple blink for manual mode
          for(int i = 0; i < 3; i++) {
            digitalWrite(LED_PIN, HIGH);
            delay(100);
            digitalWrite(LED_PIN, LOW);
            delay(100);
          }
        }
        else if(!autonomousMode) {
          // Only process other commands if in manual mode
          processCommand(cmd);
        }
      }

      // Run autonomous behavior if in autonomous mode
      if(autonomousMode) {
        RunautonomousMode();
      }
      
      // Keep BLE connection alive
      BLE.poll();
    }
  }
}

void processCommand(char cmd) {
  currentCommand = cmd;

  switch(cmd) {
    case 'F':
      Forward();
      break;

    case 'B':
      Backward();
      break;

    case 'R':
      Right();
      break;

    case 'L':
      Left();
      break;

    case 'S':
      motorR.release();
      motorL.release();
      break;

    default:
      break;
  }
}

void RunautonomousMode() {
  // Use ping_cm() which is faster and non-blocking
  distance = sonar.ping_cm();
  
  // If no echo (0), treat as far away
  if (distance == 0) {
    distance = MAX_DISTANCE;
  }

  if (distance < 8) {
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDB, HIGH);
    //  Danger zone: stop & avoid
    motorR.release();
    motorL.release();
    delay(100);

    // Back up (random duration)
    motorR.setSpeed(255);
    motorL.setSpeed(255);
    motorR.backward();
    motorL.backward();
    delay(random(100, 400));

    // Randomize turn direction & duration
    int turnDuration = random(100, 800);
    if (random(2) == 0) {
      // Turn right
      motorR.release();
      motorL.release();
      delayMicroseconds(10);
      motorR.setSpeed(255);
      motorL.setSpeed(255);
      motorR.backward();
      motorL.forward();
      delay(turnDuration);
    } else {
      // Turn left
      motorR.release();
      motorL.release();
      delayMicroseconds(10);
      motorR.setSpeed(255);
      motorL.setSpeed(255);
      motorR.forward();
      motorL.backward();
      delay(turnDuration);
    }
  }
  else if (distance <= 50) {
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDG, LOW);
    digitalWrite(LEDB, HIGH);
    motorR.release();
    motorL.release();
    delayMicroseconds(10);
    motorR.setSpeed(127);
    motorL.setSpeed(127);
    motorR.forward();
    motorL.forward();
  }
  else {
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDB, LOW);
    motorR.release();
    motorL.release();
    delayMicroseconds(10);
    motorR.setSpeed(255);
    motorL.setSpeed(255);
    motorR.forward();
    motorL.forward();
  }

  // Shorter delay to keep BLE responsive
  delay(5);
}

void Forward() {
  motorR.release();
  motorL.release();
  delay(3);

  motorR.setSpeed(255);
  motorL.setSpeed(255);

  motorR.forward();
  motorL.forward();
}

void Backward() {
  motorR.release();
  motorL.release();
  delay(3);

  motorR.setSpeed(255);
  motorL.setSpeed(255);

  motorR.backward();
  motorL.backward();
}

void Right() {
  motorR.release();
  motorL.release();
  delay(3);

  motorR.setSpeed(175);
  motorL.setSpeed(175);

  motorR.backward();
  motorL.forward();
}

void Left() {
  motorR.release();
  motorL.release();
  delay(3);

  motorR.setSpeed(175);
  motorL.setSpeed(175);

  motorR.forward();
  motorL.backward();
}