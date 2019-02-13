#include <Keyboard.h>
#include <Mouse.h>

// pin logic define
#define inputPin_Active 0
#define outputPin_Active LOW
#define outputPin_Negative HIGH

// outputPin assign
#define output_a 2
#define output_b 3
#define output_c 4

// inputPin assign
#define input_a 7
#define input_b 8
#define input_c 9
#define input_d 10
#define input_e 11
#define input_f 12

// analogPad assign
#define STY A0
#define STX A1


// // output_Pin assign
// int output_a = 2;
// int output_b = 3;
// int output_c = 4;

// // input_Pin assign
// int input_a = 7;
// int input_b = 8;
// int input_c = 9;
// int input_d = 10;
// int input_e = 11;
// int input_f = 12;

// local variable
int inputMode = 0;
int outPort, inPort;
int padX, padY, padPush;

boolean showMatrix = false;
boolean inputNow = false;
boolean inputPad = false;
boolean inputOnce= false;

int preMode, preButton;

// keyMatrix shiftregister

byte keyMatrix[3][6] = {
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0}
};

byte preMatrix[3][6] = {
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0}
};

byte padMatrix[3][3] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};

// keyMap
const char* keyMap[3][16][5] = {
  { /*0*/
    { "a" , "i" , "u" , "e" , "o"  }, /*0*/
    { "ka", "ki", "ku", "ke", "ko" }, /*1*/
    { "sa", "si", "su", "se", "so" }, /*2*/
    { "ta", "ti", "tu", "de", "to" }, /*3*/
    { "na", "ni", "nu", "ne", "no" }, /*4*/
    { "ha", "hi", "hu", "he", "ho" }, /*5*/
    { "ma", "mi", "mu", "me", "mo" }, /*6*/
    { "ya", "()", "yu", "()", "yo" }, /*7*/
    { "ra", "ri", "ru", "re", "ro" }, /*8*/
    { "", "", "", "", "" }, /*9*/
    { "wa", "wo", "nn", "-" , "~"  }, /*10*/
    { "", "", "", "", "" }, /*11*/
    { "", "", "", "", "" }, /*12*/
    { "", "", "", "", "" }, /*13*/
    { "", "", "", "", "" }, /*14*/
    { "", "", "", "", "" }, /*15*/
  },
  { /*1*/
    { "@", "/", ":", "_", "1" },
    { "a", "b", "c", "" , "2" },
    { "d", "e", "f", "" , "3" },
    { "g", "h", "i", "" , "4" },
    { "j", "k", "l", "" , "5" },
    { "m", "n", "o", "" , "6" },
    { "p", "q", "r", "s", "7" },
    { "t", "u", "v", "" , "8" },
    { "w", "x", "y", "z", "9" },
    { "" , "" , "" , "" , ""  },
    { "'", "\"", "&", "-", "0" },
    { ",", ".", "?", "!", ";" }
  },
  {
    { "1", "", "", "", "" },
    { "2", "", "", "", "" },
    { "3", "", "", "", "" },
    { "4", "", "", "", "" },
    { "5", "", "", "", "" },
    { "6", "", "", "", "" },
    { "7", "", "", "", "" },
    { "8", "", "", "", "" },
    { "9", "", "", "", "" },
    { "", "", "", "", "" },
    { "0", "", "", "", "" },
    { "", "", "", "", "" }
  }
};

// bootTimer
int bootTimer = 0;

// inputFilter
byte keyMatrix_Mask   = 0b00000001;
// temp
byte keyMatrix_Filter = 0b00000000;

// setup
void setup()
{
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(output_a, INPUT_PULLUP);
  pinMode(output_b, INPUT_PULLUP);
  pinMode(output_c, INPUT_PULLUP);
  pinMode(input_a, INPUT_PULLUP);
  pinMode(input_b, INPUT_PULLUP);
  pinMode(input_c, INPUT_PULLUP);
  pinMode(input_d, INPUT_PULLUP);
  pinMode(input_e, INPUT_PULLUP);
  pinMode(input_f, INPUT_PULLUP);
  pinMode(STX, INPUT_PULLUP);
  pinMode(STY, INPUT_PULLUP);
  Keyboard.begin();
}

// the loop
void loop() {
  // boot timer
  if (bootTimer < 1) {
    delay(1000);
    bootTimer = bootTimer + 1;
    Keyboard.write(0xC1);
  }
  // after boot timer count
  else {
    // Serial.println(analogRead(STX));
    // Serial.println(analogRead(STY));
    // Serial.println("x,y");

    if (inputNow) {
          if (!inputOnce) {
            if (analogRead(STX) > 900) {
              padX = 4;
              inputPad = true;
            }
            else if (analogRead(STX) < 400) {
              padX = 2;
              inputPad = true;
            }

            if (analogRead(STY) > 900) {
              padX = 1;
              inputPad = true;
            }
            else if (analogRead(STY) < 400) {
              padX = 3;
              inputPad = true;
            }

            if (inputPad) {
              padPush = padX + padY;
              Keyboard.press(KEY_BACKSPACE);
              Keyboard.release(KEY_BACKSPACE);
              Keyboard.print(keyMap[preMode][preButton][padX]);
              inputPad = false;
              inputOnce = true;
            }
          }
        }
        else {
          inputOnce = false;
          // if (analogRead(STX) > 950) {
          //   Keyboard.press(KEY_DOWN_ARROW);
          // }
          // else {
          //   Keyboard.release(KEY_DOWN_ARROW);
          // }

          // if (analogRead(STX) < 400) {
          //   Keyboard.press(KEY_UP_ARROW);
          // }
          // else {
          //   Keyboard.release(KEY_UP_ARROW);
          // }

          // if (analogRead(STY) > 950) {
          //   Keyboard.press(KEY_LEFT_ARROW);
          // }
          // else {
          //   Keyboard.release(KEY_LEFT_ARROW);
          // }

          // if (analogRead(STY) < 400) {
          //   Keyboard.press(KEY_RIGHT_ARROW);
          // }
          // else {
          //   Keyboard.release(KEY_RIGHT_ARROW);
          // }
        }

    for (outPort = 0; outPort < 3; outPort++)
    {
      // out port
      if (outPort == 0) {
        pinMode(output_b, INPUT_PULLUP);
        pinMode(output_c, INPUT_PULLUP);
        pinMode(output_a, OUTPUT);
        digitalWrite(output_a, outputPin_Active);
      }
      else if (outPort == 1) {
        pinMode(output_a, INPUT_PULLUP);
        pinMode(output_c, INPUT_PULLUP);
        pinMode(output_b, OUTPUT);
        digitalWrite(output_b, outputPin_Active);
      }
      else if (outPort == 2) {
        pinMode(output_a, INPUT_PULLUP);
        pinMode(output_b, INPUT_PULLUP);
        pinMode(output_c, OUTPUT);
        digitalWrite(output_c, outputPin_Active);
      }
      delay(1);

      for (inPort = 0; inPort < 6; inPort++) {
        int buttonNo = outPort + (inPort * 3);

        // keyMatrix [outPort][inPort]のデータを1ビット左シフトする
        keyMatrix[outPort][inPort] = keyMatrix[outPort][inPort] << 1;

        // input_a(7)+inPort つまり現在マトリクス走査対象のスイッチが押されているか
        if (digitalRead(input_a + inPort) == inputPin_Active) {
          // 1ビット加算
          keyMatrix[outPort][inPort] = keyMatrix[outPort][inPort] + 1;
        }
        // 押されていない場合
        else {
          // 1ループ前にも押されていたか検証
          keyMatrix_Filter = preMatrix[outPort][inPort] & keyMatrix_Mask;
          if (keyMatrix_Filter == 1) {
            // 押下フラグ解除
            inputNow = false;
          }
        }
        preMatrix[outPort][inPort] = keyMatrix[outPort][inPort];

        // input Key
        if (keyMatrix [outPort][inPort] == 3) {
          Serial.println(buttonNo);
          if (buttonNo == 9) {
            Keyboard.write(KEY_BACKSPACE);
          }
          else {
            Keyboard.releaseAll();
          }

          if (buttonNo == 11) {
            Keyboard.write(KEY_RETURN);
          }
          else {
            Keyboard.releaseAll();
          }

          if (buttonNo == 15) {
            Keyboard.write(0x20);
          }
          else {
            Keyboard.releaseAll();
          }

          if (outPort == 0 && inPort == 4) {
            inputMode = inputMode + 1;
            if (inputMode == 1) {
              Keyboard.write(0xC1);
            }
            if (inputMode > 2) {
              inputMode = 0;
              Keyboard.write(0xC1);
            }
          }
          else {
            Keyboard.print(keyMap[inputMode][buttonNo][0]);

            // Keyboard.write(0xE38182);
            // // Keyboard.write(0x83);
            // // Keyboard.write(0x5C);
            Serial.print(keyMap[inputMode][buttonNo][0]);
            Serial.println(" : input!!");
            preMode = inputMode;
            preButton = buttonNo;
            inputNow = true;
          }
        }
        delay(1);
      }
    }
  }
}