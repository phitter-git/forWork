#ifndef FLICKEY_h
#define FLICKEY_h

#include <Keyboard.h>
#include <Mouse.h>
#include <StackArray.h>

#define INPUT_HIGH 0
#define INPUT_LOW 1
#define OUTPUT_HIGH LOW
#define OUTPUT_LOW HIGH

#define OUTPUT_A 2
#define OUTPUT_B 3
#define OUTPUT_C 4
#define INPUT_A 5
#define INPUT_B 6
#define INPUT_C 7
#define INPUT_D 8
#define INPUT_E 9
#define INPUT_F 10

#define STX A0
#define STY A1
#define LED_A 15
#define LED_B 14
#define LED_C 16
// #define LED_A 13
// #define LED_B 12
// #define LED_C 11

#define JAPANESE 1
#define ENGLISH 2
#define NUMBER 3
#define MOUSE 4
#define KEY_SPACE 0x20

const char* key_map[3][11][5] = {
  { /*0*/
    { "a" , "i" , "u" , "e" , "o"  }, /*0*/
    { "ta", "ti", "tu", "te", "to" }, /*1*/
    { "ma", "mi", "mu", "me", "mo" }, /*2*/
    { "ka", "ki", "ku", "ke", "ko" }, /*3*/
    { "na", "ni", "nu", "ne", "no" }, /*4*/
    { "ya", ",", "yu", ".", "yo" }, /*5*/
    { "sa", "si", "su", "se", "so" }, /*6*/
    { "ha", "hi", "hu", "he", "ho" }, /*7*/
    { "ra", "ri", "ru", "re", "ro" }, /*8*/
    { "", "", "", "", "" }, /*9*/
    { "wa", "!", "wo", "-" , "nn"  }, /*10*/
  },
  { /*1*/
    { "@", "/", ":", "_", "1" }, /*0*/
    { "g", "h", "i", "$", "4" }, /*3*/
    { "p", "q", "r", "s", "7" }, /*6*/
    { "a", "b", "c", "\"", "2" }, /*1*/
    { "j", "k", "l", "(", "5" }, /*4*/
    { "t", "u", "v", "#", "8" }, /*7*/
    { "d", "e", "f", ")", "3" }, /*2*/
    { "m", "n", "o", "", "6" }, /*5*/
    { "w", "x", "y", "z", "9" }, /*8*/
    { "" , "" , "" , "" , ""  }, /*9*/
    { "?", ".", ",", "!", "0" }, /*10*/
  },
  { /*2*/
    { "1", "@", ";", ":", "_" }, /*0*/
    { "4", "^", "", "", "" }, /*3*/
    { "7", "[", "]", "|", "~" }, /*6*/
    { "2", "<", "{", ">", "}" }, /*1*/
    { "5", "&", "$", "#", "`" }, /*4*/
    { "8", "(", '"', ")", "'" }, /*7*/
    { "3", "", "", "", "" }, /*2*/
    { "6", "", "", "", "" }, /*5*/
    { "9", "\"", "/", "", "" }, /*8*/
    { ".", "+", "-", "*", "/" }, /*9*/
    { "0", ",", ".", "!", "?" }, /*10*/
  }
};

const char* dark_point[4][5] = {
  {"ga","gi","gu","ge","go"},
  {"za","zi","zu","ze","zo"},
  {"da","di","du","de","do"},
  {"ba","bi","bu","be","bo"}
};

const char* vowel_point[5] = {"pa","pi","pu","pe","po"};

const char* small_letter[3][5] = {
  {"la","li","lu","le","lo"},
  {"lya","lya","lyu","lya","lyo"},
  {"ltu","lwa","","",""}
};

namespace Flickey {

  /**********************************************************/

  class Key_ {

    public:
    StackArray<int> pressed_keys;
    StackArray<int> long_pressed_keys;
    const int col_keys = 3;
    const int row_keys = 5;
    int pressed_direction;
    int pressed_key_number;
    int convert_number;
    int count;
    boolean is_pressed;
    boolean fn_enabled;
    boolean is_kana = false;
    boolean is_large = false;

    void stackInitialize();
    void directionKeyOutput();
    void press(int key_number);
    void release(int key_number);
    void longPress(int key_number);
    void mouseWheel(int distance);
    void charConvert(boolean designed_convert = false);
    void engConvert();
    boolean isPressed();
    boolean isFnPressed();
    boolean darkPoint();
    boolean vowelPoint();
    boolean smallLetter();
    boolean isLongPressed(int key_number);
    boolean isIntentionalPressed(int key_number);
    boolean waitRelease(int key_number);

    private:
    const byte accept_intentional = 0b00001111;
    byte key_register[15];
    void outputSwitching(int output_col_number);
  };

  /**********************************************************/

  class Pad_ {
    private:
    typedef struct {
      int x;
      int y;
      float x_raw;
      float y_raw;
      int direction;
      int pre_direction;
    } pad_state;

    typedef struct {
      int low_border;
      int high_border;
      int standard_measure_x;
      int standard_measure_y;
      int allowable_accidental;
      int max_measure;
      int min_measure;
      int sensitivity;
    } pad_config;

    public:
    pad_state state;
    pad_config config = {
      150,
      900,
      555,
      555,
      10,
      1023,
      25,
      3,
    };
    boolean is_moved = false;
    const int LEFT = 1;
    const int UP = 2;
    const int RIGHT = 3;
    const int DOWN = 4;

    boolean isMoved();
    void returnWait();
    void updateState();
    void directionUpdate(int direction);
    void changeSensitivity(int change_value);
  };

  /**********************************************************/

  class Led_ {
    public:
    void display();
    void displayErrorState();
  };

  /**********************************************************/

  Key_ Key = Key_();
  Pad_ Pad = Pad_();
  Led_ Led = Led_();

  int input_mode = JAPANESE;
  const int max_modes = 4;
  boolean is_outputted = false;
  boolean is_exchanged = false;
  boolean auto_switching_enabled = true;
  boolean setup_switching_enabled = true;

  void updateState();
  void mouseModeProcess();
  void otherModeProcess();
  void switchingFullAndHalf(boolean auto_enabled = true);
  void exchangeCharKey();
  void changeMode();
}

#endif