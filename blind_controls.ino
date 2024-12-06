/*

====Fight to Relax====

Fight to Relax is a set of prototype tangible input devices designed to play the game ‘A Blind Legend’, created by Dowino.

This code is designed to be run on an Arduino Pro Micro for its USB HID support.

This is the companion to the walking-code Arduino - this Arduino code is for all the discrete sensors: heart, shield, sword-hilt, and sword direction.

Other than the two left/right paddles which use a Digital Pin to read when values go low, all the other Inputs are Analog, and read the resulting values from voltage dividers.

The mechanics of the game are such that only one button is pressed at a time, so I was able to use just a long sequence of IF statements, which read the sensors, set a variable which holds which key to press. This is followed by a switch-case that takes the key-variable and then sends the corresponding keyboard command (with different timings based on how the game would interpret it).

Author: Mark Altosaar
https://github.com/MarkErik/fight-to-relax

Incorporates Libraries:
- Adruino Keyboard library

Used the built-in Keyboard tutorial as the starting point. (Heavily modified)

*/

#include "Keyboard.h"

// set up variables to hold the pin numbers
const int heartPin = 18;        //A0, breadboard position 35
const int leftPaddle = 2;       //Digital 2, breadboard position 25
const int rightPaddle = 21;     //Digital 21 (A3 on board), breadboard position 15
const int shieldPin = 10;       //A10, breadboard position 45
const int swordVoltagePin = 9;  //A9, breadboard position 20-25
const int swordHoldPin = 8;     //A8, breadboard position 20-25

//Ranges and targets
const int swordVoltageRange = 10;
const int heartVal = 800;
const int shieldVal = 100;  //covered is lower
const int swordHandleVal = 300;

//Logic for sword
bool swordGripped = false;
long swordTime = 0;
long swordDelay = 8000;

bool inRange(int reading, int target, int range) {
  return ((reading >= (target - range)) && (reading <= (target + range)));
}

int keyToPress = -1;
// keyToPress:
// 1 is Heart (spacebar)
// 2 is Left paddle (left arrow)
// 3 is Right paddle (right arrow)
// 4 is Sheild (f key)
// 5 is Grip Sword (return key)
// 6 is Sword Forward (up arrow)
// 7 is Sword Left (left arrow)
// 8 is Sword Right (right arrow)
// 9 is Sword Back (down arrow)

// turns on verbose prints to serial console
bool testing = false;
bool showSensorData = false;

void setup() {

  //set up the pins for reading the different buttons / sensors
  pinMode(heartPin, INPUT);
  pinMode(leftPaddle, INPUT_PULLUP);
  pinMode(rightPaddle, INPUT_PULLUP);
  pinMode(shieldPin, INPUT);
  pinMode(swordVoltagePin, INPUT);
  pinMode(swordHoldPin, INPUT);

  Keyboard.begin();

  if (testing) {
    Serial.begin(9600);
  }

}

void loop() {

  //logic for key press - assume that only one key at a time, since in the game you aren't pressing multiple at a time
  if (analogRead(heartPin) < heartVal) {
    if (testing) {
      Serial.println("Heart Press");
    }
    keyToPress = 1;
  }
  if (digitalRead(leftPaddle) == LOW) {
    if (testing) {
      Serial.println("Left Paddle");
    }
    keyToPress = 2;
  }
  if (digitalRead(rightPaddle) == LOW) {
    if (testing) {
      Serial.println("Right Paddle");
    }
    keyToPress = 3;
  }
  if (analogRead(shieldPin) > shieldVal) {
    if (testing) {
      Serial.println("Lifted Shield");
    }
    keyToPress = 4;
  }
  if (analogRead(swordHoldPin) < swordHandleVal) {
    if (swordGripped == false) {
      keyToPress = 5;
      swordGripped = true;
      swordTime = millis();
      if (testing) {
        Serial.println("Gripping Sword");
      }
    }
    else{
      swordTime = millis();
    }
  }
  if (inRange(analogRead(swordVoltagePin), 293, swordVoltageRange)) {
    if (testing) {
      Serial.println("Sword Front");
    }
    keyToPress = 6;
  }
  if (inRange(analogRead(swordVoltagePin), 443, swordVoltageRange)) {
    if (testing) {
      Serial.println("Sword Back");
    }
    keyToPress = 9;
  }
  if (inRange(analogRead(swordVoltagePin), 172, swordVoltageRange)) {
    if (testing) {
      Serial.println("Sword Left");
    }
    keyToPress = 7;
  }
  if (inRange(analogRead(swordVoltagePin), 362, swordVoltageRange)) {
    if (testing) {
      Serial.println("Sword Right");
    }
    keyToPress = 8;
  }


  if (testing && showSensorData) {
    Serial.print("Heart voltage divider reading: ");
    Serial.println(analogRead(heartPin));
    Serial.print("Shield voltage divider reading: ");
    Serial.println(analogRead(shieldPin));
    Serial.print("Sword handle divider reading: ");
    Serial.println(analogRead(swordHoldPin));
    Serial.print("Sword voltage reading: ");
    Serial.println(analogRead(swordVoltagePin));
  }

  // based on which sensor was triggered, activate the corrseponding key
  switch (keyToPress) {
    case 1:
      Keyboard.press(' ');
      delay(100);
      Keyboard.releaseAll();
      delay(800);
      break;
    case 2:
      Keyboard.press(KEY_LEFT_ARROW);
      delay(400);
      Keyboard.releaseAll();
      delay(400);
      break;
    case 3:
      Keyboard.press(KEY_RIGHT_ARROW);
      delay(400);
      Keyboard.releaseAll();
      delay(400);
      break;
    case 4:
      Keyboard.press('f');
      delay(100);
      // unlike all the other buttons which are 'single' press usage
      // the shield is active online while the key is pressed
      // don't release the 'f' key until the shield
      // this is blocking since a while, but in the game, you don't do anything but hold up the shield
      while (analogRead(shieldPin) > shieldVal) {
        delay(5);
      }
      Keyboard.releaseAll();
      delay(20);
      break;
    case 5:
      Keyboard.press(KEY_RETURN);
      delay(100);
      Keyboard.releaseAll();
      delay(500);
      break;
    case 6:
      Keyboard.press(KEY_UP_ARROW);
      delay(100);
      Keyboard.releaseAll();
      delay(400);
      break;
    case 7:
      Keyboard.press(KEY_LEFT_ARROW);
      delay(100);
      Keyboard.releaseAll();
      delay(400);
      break;
    case 8:
      Keyboard.press(KEY_RIGHT_ARROW);
      delay(100);
      Keyboard.releaseAll();
      delay(400);
      break;
    case 9:
      Keyboard.press(KEY_DOWN_ARROW);
      delay(100);
      Keyboard.releaseAll();
      delay(400);
      break;
    case -1:
      delay(5);
      break;
  }

  // reset the sword grip, if they haven't been holding the sword for a while
  // also works as a debounce for changing grip pressure
  if (millis() - swordTime > swordDelay && analogRead(swordHoldPin) > swordHandleVal)
  {
    swordGripped = false;
    swordTime = 0;
    if (testing) {
      Serial.println("Sword boolean set off");
      delay(200);
    }
  }

  // reset the variable to hold what keyboard button to press for the next loop
  // so that if no sensors are activated, no keyboard command  will be sent
  keyToPress = -1;
  delay(1);
}  //end main loop
