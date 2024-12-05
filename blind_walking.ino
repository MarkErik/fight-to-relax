#include "Keyboard.h"

// variables to hold sensor pins on the arduino
const int leftFootPin = 2;
const int rightFootPin = 21;

// timing variables for determing walk/run, and how long to keep moving
const long walkingTime = 1600;
const long runningTime = 800;
const long timeOut = 3000;
const long holdKey = 1000;

int buttonPress = -1;  //-1 for not read button state yet, 1 for left, 2 for right

int moving = -1;      //not moving is -1, walking is 1, running is 2
int first_foot = -1;  //-1 for when no steps registered, 1 for left, 2 for right
long time_of_step = 0;
long time_since_step = 0;
long time_since_press_key = -1;

// if true, verbose logging to serial console
bool testing = false;

void setup() {
  // make the foot buttons an input and turn on the pull-up resistor so it goes high unless
  // connected to ground
  pinMode(leftFootPin, INPUT_PULLUP);
  pinMode(rightFootPin, INPUT_PULLUP);

  Keyboard.begin();

  if (testing) {
    Serial.begin(9600);
  }
}


void loop() {

  if (digitalRead(2) == LOW) {
    if (testing) {
      Serial.println("Left Foot");
    }
    if (first_foot == -1 || first_foot == 2) {
      time_of_step = millis();
    }
    buttonPress = 1;
  }
  if (digitalRead(21) == LOW) {
    if (testing) {
      Serial.println("Right Foot");
    }
    if (first_foot == -1 || first_foot == 1) {
      time_of_step = millis();
    }
    buttonPress = 2;
  }

  if (moving == -1 && first_foot == -1 && buttonPress != -1)  //not moving and haven't started tracking cadence
  {
    first_foot = buttonPress;
    time_since_step = time_of_step;
  } else if (moving == -1 && buttonPress != -1) {
    if (first_foot != buttonPress) {
      if (time_of_step - time_since_step < runningTime) {
        moving = 2;
      } else if (time_of_step - time_since_step < walkingTime) {
        moving = 1;
      }
      first_foot = buttonPress;
      time_since_step = time_of_step;
    }
  }

  if (testing) {
    Serial.print("Moving: ");
    Serial.println(moving);
    Serial.print("First Foot: ");
    Serial.println(first_foot);
    Serial.print("Time since step: ");
    Serial.println(time_since_step);
    Serial.print("Time of step: ");
    Serial.println(time_of_step);
    Serial.print("Time difference: ");
    Serial.println(time_of_step - time_since_step);
  }

  delay(25);

  switch (moving) {
    case 1: //walking
      if (time_since_press_key == -1) {
        if (testing) {
          Serial.println("Press Up");
        }
        Keyboard.press(KEY_UP_ARROW);
        time_since_press_key = millis();
      }
      break;
    case 2: //running
      if (time_since_press_key == -1) {
        if (testing) {
          Serial.println("Press Shift + Up");
        }
        Keyboard.press(KEY_LEFT_SHIFT);
        Keyboard.press(KEY_UP_ARROW);
        time_since_press_key = millis();
      }
      break;
    case -1:
      delay(10);
      break;
  }

  // keep moving for a moment after intiating walk / run sequence
  if (millis() - time_since_press_key > holdKey) {
    if (testing) {
      Serial.println("Release keys");
    }
    Keyboard.releaseAll();
    delay(10);
    time_since_press_key = -1;
  }

  // if only one button was pressed, or the time between steps was too long, reset everything
  if (millis() - time_since_step > timeOut) {
    //reset steps
    if (testing) {
      Serial.println("Reset");
    }
    moving = -1;
    first_foot = -1;
    time_since_step = 0;
    time_of_step = 0;
    time_since_press_key = -1;
    Keyboard.releaseAll();
    delay(1);
  }

  //reset the keyboard button for the next loop
  buttonPress = -1;
}  //end main loop
