#include "Flickey.h"

using namespace Flickey;

void Flickey::updateState() {
  Key.fn_enabled = Key.isFnPressed();
  Key.is_pressed = Key.isPressed();
  Pad.is_moved = Pad.isMoved();
  Led.display();
}

void Flickey::changeMode() {
  input_mode++;
  if (input_mode > max_modes) {
    input_mode = JAPANESE;
    Mouse.end();
  }
  switch (input_mode) {
    case JAPANESE:
    case ENGLISH:
      switchingFullAndHalf(auto_switching_enabled);
      return;
    case MOUSE:
      Mouse.begin();
      return;
    default:
      return;
  }
}

void Flickey::otherModeProcess() {
  if (Key.is_pressed) {

    if (Pad.is_moved) {
      exchangeCharKey();
    } else {
      while (Key.pressed_keys.isEmpty() == false) {
        int press_key = Key.pressed_keys.pop();
        Key.press(press_key);
        // if (press_key == 0) {
        //   while (Key.pressed_keys.isEmpty() == false) {
        //     Key.pressed_keys.pop();
        //   }
        //   return;
        // }
      }
    }
  } else {
    if (Pad.is_moved) {
      if (is_exchanged) {
        Pad.returnWait();
        is_exchanged = false;
      }
    }
    Key.directionKeyOutput();
  }
}

void Flickey::mouseModeProcess() {
  Mouse.move((int)(Pad.state.x_raw), (int)(Pad.state.y_raw), 0);
  if (Key.is_pressed) {
    while (Key.pressed_keys.isEmpty() == false) {
      Key.press(Key.pressed_keys.pop());
    }
  } else {
    Mouse.release(MOUSE_LEFT);
    Mouse.release(MOUSE_RIGHT);
    Mouse.release(MOUSE_MIDDLE);
  }
}

void Flickey::exchangeCharKey() {
    if (is_exchanged) {
      if (Pad.state.pre_direction != Pad.state.direction) {
        if (key_map[input_mode-1][Key.pressed_key_number-3][Pad.state.direction] != "") {
          Key.pressed_direction = Pad.state.direction;
          Keyboard.write(KEY_BACKSPACE);
          Keyboard.print(key_map[input_mode-1][Key.pressed_key_number-3][Pad.state.direction]);
        }
      }
    } else {
      if (key_map[input_mode-1][Key.pressed_key_number-3][Pad.state.direction] != "") {
        Serial.println(input_mode);
        Serial.println(Key.pressed_key_number);
        Serial.println(Pad.state.direction);
        is_exchanged = true;
        Key.pressed_direction = Pad.state.direction;
        Keyboard.write(KEY_BACKSPACE);
        Keyboard.print(key_map[input_mode-1][Key.pressed_key_number-3][Pad.state.direction]);
      }
    }
}


void Key_::directionKeyOutput() {
  // Serial.println(Pad.state.x);
  if (Pad.state.x == 0) {
    Keyboard.release(KEY_RIGHT_ARROW);
    Keyboard.release(KEY_LEFT_ARROW);
  } else if (Pad.state.x == Pad.RIGHT) {
    Keyboard.press(KEY_RIGHT_ARROW);
  } else if (Pad.state.x == Pad.LEFT) {
    Keyboard.press(KEY_LEFT_ARROW);
  }
  if (Pad.state.y == 0) {
    Keyboard.release(KEY_UP_ARROW);
    Keyboard.release(KEY_DOWN_ARROW);
  } else if (Pad.state.y == Pad.UP) {
    Keyboard.press(KEY_UP_ARROW);
  } else if (Pad.state.y == Pad.DOWN) {
    Keyboard.press(KEY_DOWN_ARROW);
  }
}

void Flickey::switchingFullAndHalf(boolean auto_enabled = true) {
  if (auto_enabled == false) {
    return;
  }
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.write(0x60);
  Keyboard.release(KEY_LEFT_ALT);
}

void Key_::stackInitialize() {
  while (long_pressed_keys.isEmpty() == false) {
    long_pressed_keys.pop();
  }
  while (pressed_keys.isEmpty() == false) {
    pressed_keys.pop();
  }
}

boolean Key_::waitRelease(int const key_number) {
  stackInitialize();
  boolean return_wait = false;
  delay(10);
  if (isPressed()) {
    while (long_pressed_keys.isEmpty() == false) {
      if (long_pressed_keys.pop() == key_number) {
        if (Pad.isMoved()) {
          return_wait = true;
        } else {
          if (waitRelease(key_number)) {
            return_wait = true;
          }
        }
      }
    }
  }
  stackInitialize();
  return return_wait;
}

boolean Key_::isPressed() {
  for (int col = 0; col < col_keys; col++) {
    outputSwitching(col);
    for (int row = 0; row < row_keys; row++) {
      int key_number = col + (row * col_keys);
      if (digitalRead(INPUT_A + row) == INPUT_HIGH) {
        if (isIntentionalPressed(key_number)) {
          pressed_keys.push(key_number);
        }
        if (isLongPressed(key_number)) {
          long_pressed_keys.push(key_number);
        }
      } else {
        release(key_number);
      }
    }
  }

  if (long_pressed_keys.isEmpty() == false) {
    return true;
  } else {
    return false;
  }
}

boolean Key_::isFnPressed() {
  outputSwitching(OUTPUT_A);
  if (digitalRead(INPUT_F) == INPUT_HIGH) {
    return true;
  } else {
    return false;
  }
}

boolean Key_::isIntentionalPressed(int const key_number) {
  key_register[key_number] = key_register[key_number] << 1;
  key_register[key_number] = key_register[key_number] + 1;
  return key_register[key_number] == accept_intentional ? true : false;
}

boolean Key_::isLongPressed(int const key_number) {
  byte long_pressed = key_register[key_number] & accept_intentional;
  return long_pressed == accept_intentional ? true : false;
}

void Key_::press(int const key_number) {
  if (Key.fn_enabled) {
    if (input_mode == MOUSE) {
      switch (key_number) {
        case 0:
          Pad.changeSensitivity(1);
          return;
        case 1:
          Pad.changeSensitivity(-1);
          return;
        case 2:
          Keyboard.write(KEY_TAB);
          return;
        case 3:
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.press(KEY_LEFT_SHIFT);
          Keyboard.write(KEY_TAB);
          Keyboard.release(KEY_LEFT_SHIFT);
          Keyboard.release(KEY_LEFT_CTRL);
          return;
        case 4:
          Mouse.press(MOUSE_LEFT);
          return;
        case 5:
          Keyboard.write(KEY_HOME);
          return;
        case 6:
          Keyboard.press(KEY_LEFT_CTRL);
          Mouse.move(0,0,1);
          Keyboard.release(KEY_LEFT_CTRL);
          return;
        case 7:
          Keyboard.press(KEY_LEFT_CTRL);
          Mouse.move(0,0,-1);
          Keyboard.release(KEY_LEFT_CTRL);
          return;
        case 8:
          Keyboard.write(KEY_LEFT_GUI);
          return;
        case 9:
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.write(KEY_TAB);
          Keyboard.release(KEY_LEFT_CTRL);
          return;
        case 10:
          Mouse.press(MOUSE_RIGHT);
          return;
        case 11:
          Keyboard.write(KEY_END);
          return;
        case 12:
          Keyboard.press(KEY_LEFT_ALT);
          Keyboard.write(KEY_TAB);
          Keyboard.release(KEY_LEFT_ALT);
          return;
        case 13:
          Keyboard.press(KEY_LEFT_ALT);
          Keyboard.press(KEY_LEFT_SHIFT);
          Keyboard.write(KEY_TAB);
          Keyboard.release(KEY_LEFT_SHIFT);
          Keyboard.release(KEY_LEFT_ALT);
          return;
        default:
          return;
      }
    } else if (input_mode == NUMBER) {
      switch (key_number) {
        case 0:
          Keyboard.write(KEY_F11);
          return;
        case 1:
          Keyboard.write(KEY_F12);
          return;
        case 2:
          Keyboard.write(KEY_TAB);
          return;
        case 3:
          Keyboard.write(KEY_F1);
          return;
        case 4:
          Keyboard.write(KEY_F4);
          return;
        case 5:
          Keyboard.write(KEY_F7);
          return;
        case 6:
          Keyboard.write(KEY_F2);
          return;
        case 7:
          Keyboard.write(KEY_F5);
          return;
        case 8:
          Keyboard.write(KEY_F8);
          return;
        case 9:
          Keyboard.write(KEY_F3);
          return;
        case 10:
          Keyboard.write(KEY_F6);
          return;
        case 11:
          Keyboard.write(KEY_F9);
          return;
        case 12:
          Keyboard.write(KEY_ESC);
          return;
        case 13:
          Keyboard.write(KEY_F10);
          return;
        case 14:
          Keyboard.write(KEY_RETURN);
          return;
        default:
          return;
      }
    } else {
      switch (key_number) {
        case 0:
          switchingFullAndHalf();
          return;
        case 1:
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.write(0x66);
          Keyboard.release(KEY_LEFT_CTRL);
        case 2:
          Keyboard.write(KEY_TAB);
          return;
        case 3:
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.write(0x61);
          Keyboard.release(KEY_LEFT_CTRL);
          return;
        case 4:
          Keyboard.press(KEY_LEFT_SHIFT);
          Keyboard.write(KEY_LEFT_ARROW);
          Keyboard.release(KEY_LEFT_SHIFT);
          return;
        case 5:
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.write(0x78);
          Keyboard.release(KEY_LEFT_CTRL);
          return;
        case 6:
          Keyboard.press(KEY_LEFT_SHIFT);
          Keyboard.write(KEY_UP_ARROW);
          Keyboard.release(KEY_LEFT_SHIFT);
          return;
        case 7:
          Keyboard.press(KEY_LEFT_SHIFT);
          Keyboard.write(KEY_DOWN_ARROW);
          Keyboard.release(KEY_LEFT_SHIFT);
          return;
        case 8:
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.write(0x63);
          Keyboard.release(KEY_LEFT_CTRL);
          return;
        case 9:
          Keyboard.press(KEY_LEFT_SHIFT);
          Keyboard.write(0x73);
          Keyboard.release(KEY_LEFT_SHIFT);
          return;
        case 10:
          Keyboard.press(KEY_LEFT_SHIFT);
          Keyboard.write(KEY_RIGHT_ARROW);
          Keyboard.release(KEY_LEFT_SHIFT);
          return;
        case 11:
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.write(0x76);
          Keyboard.release(KEY_LEFT_CTRL);
          return;
        case 12:
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.write(0x7A);
          Keyboard.release(KEY_LEFT_CTRL);
          return;
        case 13:
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.write(0x79);
          Keyboard.release(KEY_LEFT_CTRL);
          return;
        case 14:
          Keyboard.write(KEY_RETURN);
          return;
        default:
          return;
      }
    }
  } else {
    if (input_mode == MOUSE) {
      switch (key_number) {
        case 0:
          if (waitRelease(key_number)) {
            if (Pad.state.direction != 0) {
              input_mode = Pad.state.direction;
            }
            Pad.returnWait();
          } else {
            changeMode();
          }
          return;
        case 1:
          Keyboard.write(KEY_F5);
          return;
        case 2:
          Keyboard.write(0x20);
          return;
        case 3:
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.write(0x77);
          Keyboard.release(KEY_LEFT_CTRL);
          return;
        case 4:
          Mouse.press(MOUSE_LEFT);
          return;
        case 5:
          Keyboard.write(KEY_LEFT_ARROW);
          return;
        case 6:
          mouseWheel(1);
          return;
        case 7:
          mouseWheel(-1);
          return;
        case 8:
          Mouse.press(MOUSE_MIDDLE);
          return;
        case 9:
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.write(0x74);
          Keyboard.release(KEY_LEFT_CTRL);
          return;
        case 10:
          Mouse.press(MOUSE_RIGHT);
          return;
        case 11:
          Keyboard.write(KEY_RIGHT_ARROW);
          return;
        case 12:
          Keyboard.press(KEY_LEFT_ALT);
          Keyboard.write(KEY_LEFT_ARROW);
          Keyboard.release(KEY_LEFT_ALT);
          return;
        case 13:
          Keyboard.press(KEY_LEFT_ALT);
          Keyboard.write(KEY_RIGHT_ARROW);
          Keyboard.release(KEY_LEFT_ALT);
          return;
        case 14:
          Keyboard.write(KEY_RETURN);
          return;
        default:
          return;
      }
    } else if (input_mode == NUMBER) {
      switch (key_number) {
        case 0:
          if (waitRelease(key_number)) {
            if (Pad.state.direction != 0) {
              input_mode = Pad.state.direction;
            }
            Pad.returnWait();
          } else {
            changeMode();
          }
          return;
        case 1:
          Keyboard.print(key_map[input_mode-1][9][0]);
          pressed_key_number = 12;
          pressed_direction = 0;
          convert_number = 0;
          is_exchanged = false;
          return;
        case 2:
          Keyboard.press(KEY_SPACE);
          return;
        case 12:
          Keyboard.press(KEY_BACKSPACE);
          return;
        case 14:
          Keyboard.press(KEY_RETURN);
          return;
        default:
          Serial.println(key_number);
          Keyboard.print(key_map[input_mode-1][key_number - 3][0]);
          pressed_key_number = key_number;
          pressed_direction = 0;
          convert_number = 0;
          is_exchanged = false;
          return;
      }
    } else {
      switch (key_number) {
        case 0:
          if (waitRelease(key_number)) {
            if (Pad.state.direction != 0) {
              input_mode = Pad.state.direction;
            }
            Pad.returnWait();
          } else {
            changeMode();
          }
          return;
        case 1:
          if (waitRelease(key_number)) {
            if (Pad.state.direction != 0) {
              charConvert(true);
            }
            Pad.returnWait();
          } else {
            if (input_mode == ENGLISH) {
              engConvert();
            } else if (input_mode == JAPANESE) {
              charConvert();
            }
          }
          return;
        case 2:
          Keyboard.press(KEY_SPACE);
          return;
        case 12:
          Keyboard.press(KEY_BACKSPACE);
          return;
        case 14:
          Keyboard.press(KEY_RETURN);
          return;
        default:
          Serial.println(key_number);
          Keyboard.print(key_map[input_mode-1][key_number - 3][0]);
          pressed_key_number = key_number;
          pressed_direction = 0;
          convert_number = 0;
          is_large = false;
          is_exchanged = false;
          return;
      }
    }
  }
}

void Key_::longPress(int const key_number) {
  switch (key_number) {
    case 6:
      mouseWheel(1);
      return;
    case 7:
      mouseWheel(-1);
      return;
    default:
      return;
  }
}

void Key_::release(int const key_number) {
  key_register[key_number] = 0b00000000;
  switch (key_number) {
    case 2:
      Keyboard.release(KEY_SPACE);
      return;
    case 12:
      Keyboard.release(KEY_BACKSPACE);
      return;
    case 14:
      Keyboard.release(KEY_RETURN);
      return;
    default:
      return;
  }
}

void Key_::mouseWheel(int const distance) {
  int const delay_count = 15;
  count++;
  if (count > 15) {
    Mouse.move(0,0, distance);
    count = 0;
  }
}

void Key_::charConvert(boolean designed_convert = false) {
  convert_number++;
  if (designed_convert) {
    convert_number = Pad.state.direction;
  }
  switch (convert_number) {
    case 1:
      if (smallLetter()) {
        return;
      } else {
        charConvert();
      }
      return;
    case 2:
      if (darkPoint()) {
        return;
      } else {
        charConvert();
      }
      return;
    case 3:
      if (vowelPoint()) {
        return;
      } else {
        charConvert();
      }
      return;
    case 4:
      Keyboard.write(KEY_F7);
      return;
    default:
      Keyboard.write(KEY_BACKSPACE);
      Keyboard.print(key_map[input_mode-1][pressed_key_number - 3][pressed_direction]);
      return;
  }
  if (convert_number > 4) {
    convert_number = 0;
  }
}

void Key_::engConvert() {
  if (is_large) {
    Keyboard.write(KEY_BACKSPACE);
    Keyboard.print(key_map[input_mode-1][pressed_key_number-3][pressed_direction]);
    is_large = false;
  } else {
    Keyboard.write(KEY_BACKSPACE);
    Keyboard.press(KEY_LEFT_SHIFT);
    Keyboard.print(key_map[input_mode-1][pressed_key_number-3][pressed_direction]);
    Keyboard.release(KEY_LEFT_SHIFT);
    is_large = true;
  }
}

boolean Key_::darkPoint() {
  switch (pressed_key_number) {
    case 4: //ta
      Keyboard.write(KEY_BACKSPACE);
      Keyboard.print(dark_point[2][pressed_direction]);
      return true;

    case 6: //ka
      Keyboard.write(KEY_BACKSPACE);
      Keyboard.print(dark_point[0][pressed_direction]);
      return true;

    case 9: //sa
      Keyboard.write(KEY_BACKSPACE);
      Keyboard.print(dark_point[1][pressed_direction]);
      return true;

    case 10: //ha
      Keyboard.write(KEY_BACKSPACE);
      Keyboard.print(dark_point[3][pressed_direction]);
      return true;

    default:
      return false;
  }
}

boolean Key_::vowelPoint() {
  if (pressed_key_number == 10) {
    Keyboard.write(KEY_BACKSPACE);
    Keyboard.print(vowel_point[pressed_direction]);
    return true;
  }
  return false;
}

boolean Key_::smallLetter() {
  switch (pressed_key_number) {
    case 3: //a
      Keyboard.write(KEY_BACKSPACE);
      Keyboard.print(small_letter[0][pressed_direction]);
      return true;
    case 4: //ta
      if (pressed_direction == 2) {
        Keyboard.write(KEY_BACKSPACE);
        Keyboard.print(small_letter[2][0]);
        return true;
      } else {
        return false;
      }
    case 8: //ya
      Keyboard.write(KEY_BACKSPACE);
      Keyboard.print(small_letter[1][pressed_direction]);
      return true;
    case 13: //wa
      if (pressed_direction == 0) {
        Keyboard.write(KEY_BACKSPACE);
        Keyboard.print(small_letter[2][1]);
        return true;
      } else {
        return false;
      }
  }
  return false;
}

void Key_::outputSwitching(int const output_col_number) {
  switch (output_col_number) {
    case 0:
      pinMode(OUTPUT_B, INPUT_PULLUP);
      pinMode(OUTPUT_C, INPUT_PULLUP);
      pinMode(OUTPUT_A, OUTPUT);
      digitalWrite(OUTPUT_A, OUTPUT_HIGH);
      return;
    case 1:
      pinMode(OUTPUT_A, INPUT_PULLUP);
      pinMode(OUTPUT_C, INPUT_PULLUP);
      pinMode(OUTPUT_B, OUTPUT);
      digitalWrite(OUTPUT_B, OUTPUT_HIGH);
      return;
    case 2:
      pinMode(OUTPUT_A, INPUT_PULLUP);
      pinMode(OUTPUT_B, INPUT_PULLUP);
      pinMode(OUTPUT_C, OUTPUT);
      digitalWrite(OUTPUT_C, OUTPUT_HIGH);
      return;
    default:
    return;
  }
}

boolean Pad_::isMoved() {
  Pad.is_moved = false;
  int readed_x = analogRead(STX);
  int readed_y = analogRead(STY);

  if (readed_x < Pad.config.low_border) {
    Pad.state.x = Pad.RIGHT;
    directionUpdate(Pad.RIGHT);
  }
  if (Pad.config.high_border < readed_x) {
    Pad.state.x = Pad.LEFT;
    directionUpdate(Pad.LEFT);
  }

  if (readed_y < Pad.config.low_border) {
    Pad.state.y = Pad.UP;
    directionUpdate(Pad.UP);
  }
  if (Pad.config.high_border < readed_y) {
    Pad.state.y = Pad.DOWN;
    directionUpdate(Pad.DOWN);
  }

  if (Pad.is_moved == false) {
    Pad.state.x = 0;
    Pad.state.y = 0;
    Pad.state.direction = 0;
  }

  Pad.state.x_raw = (map(readed_x, Pad.config.min_measure, Pad.config.max_measure,3,-3) / 3) * Pad.config.sensitivity;
  Pad.state.y_raw = (map(readed_y, Pad.config.min_measure, Pad.config.max_measure,-3,3) / 3) * Pad.config.sensitivity;

  return Pad.is_moved;
}

void Pad_::returnWait() {
  if (isMoved() == true) {
    returnWait();
  } else {
    return;
  }
}

void Pad_::directionUpdate(int direction) {
  Pad.state.pre_direction = Pad.state.direction;
  Pad.state.direction = direction;
  Pad.is_moved = true;
}

void Pad_::changeSensitivity(int change_Value) {
  Pad.config.sensitivity = Pad.config.sensitivity + change_Value;

  if (Pad.config.sensitivity > 6) {
    Pad.config.sensitivity = 5;
  }
  if (Pad.config.sensitivity < 1) {
    Pad.config.sensitivity = 1;
  }
}

void Led_::display() {
  switch (input_mode) {
    case JAPANESE:
      digitalWrite(LED_A, HIGH);
      digitalWrite(LED_B, LOW);
      digitalWrite(LED_C, LOW);
    return;
    case ENGLISH:
      digitalWrite(LED_A, LOW);
      digitalWrite(LED_B, HIGH);
      digitalWrite(LED_C, LOW);
    return;
    case NUMBER:
      digitalWrite(LED_A, LOW);
      digitalWrite(LED_B, LOW);
      digitalWrite(LED_C, HIGH);
    return;
    case MOUSE:
      digitalWrite(LED_A, HIGH);
      digitalWrite(LED_B, HIGH);
      digitalWrite(LED_C, HIGH);
    return;
    default:
    return;
  }
}