#include "Flickey.h"

using namespace Flickey;

void loop() {
  updateState();
  if (input_mode == MOUSE) {
    mouseModeProcess();
    while (Key.long_pressed_keys.isEmpty() == false) {
      Key.longPress(Key.long_pressed_keys.pop());
    }
  } else {
    otherModeProcess();
  }

  Key.stackInitialize();
}

void setup() {
  Serial.begin(9600);
  pinMode(OUTPUT_A, INPUT_PULLUP);
  pinMode(OUTPUT_B, INPUT_PULLUP);
  pinMode(OUTPUT_C, INPUT_PULLUP);
  pinMode(INPUT_A, INPUT_PULLUP);
  pinMode(INPUT_B, INPUT_PULLUP);
  pinMode(INPUT_C, INPUT_PULLUP);
  pinMode(INPUT_D, INPUT_PULLUP);
  pinMode(INPUT_E, INPUT_PULLUP);
  pinMode(INPUT_F, INPUT_PULLUP);
  pinMode(STX, INPUT_PULLUP);
  pinMode(STY, INPUT_PULLUP);
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_C, OUTPUT);
  Keyboard.begin();
  delay(500);
  Led.display();
  switchingFullAndHalf(setup_switching_enabled);
}