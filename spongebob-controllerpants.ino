#include <XInput.h>

const int LED_PIN           =  0;
const int SWITCH_PIN        =  1;
const int RESET_PIN         =  2;

const int DOWN_PIN          =  3;
const int RIGHT_PIN         =  4;
const int LEFT_PIN          =  5;
const int UP_PIN            =  6;

const int FIRE_PIN          =  7;

bool SWITCH_BOOL    =  HIGH;
bool RESET_BOOL     =  HIGH;

bool DOWN_BOOL      =  HIGH;
bool RIGHT_BOOL     =  HIGH;
bool LEFT_BOOL      =  HIGH;
bool UP_BOOL        =  HIGH;

bool FIRE_BOOL      =  HIGH;

bool RESET_BOOL_LAST     =  HIGH;

bool DOWN_BOOL_LAST      =  HIGH;
bool RIGHT_BOOL_LAST     =  HIGH;
bool LEFT_BOOL_LAST      =  HIGH;
bool UP_BOOL_LAST        =  HIGH;

bool FIRE_BOOL_LAST      =  HIGH;

const int BUTTON_LIST_SIZE = 16;
uint8_t BUTTON_LIST[BUTTON_LIST_SIZE] = {BUTTON_A, BUTTON_B, BUTTON_Y, BUTTON_RB, TRIGGER_RIGHT, BUTTON_R3, BUTTON_L3, DPAD_UP, DPAD_DOWN, DPAD_LEFT, DPAD_RIGHT, BUTTON_BACK, BUTTON_START, TRIGGER_LEFT, BUTTON_LB, BUTTON_X};
bool BUTTON_BOOL_LIST[BUTTON_LIST_SIZE] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};

bool CONFIG_MODE = false;
int BUTTON_LIST_POS = 0;

bool STICK_SWAP = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN,    OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(RESET_PIN,  INPUT_PULLUP);
  pinMode(DOWN_PIN,   INPUT_PULLUP);
  pinMode(RIGHT_PIN,  INPUT_PULLUP);
  pinMode(LEFT_PIN,   INPUT_PULLUP);
  pinMode(UP_PIN,     INPUT_PULLUP);
  pinMode(FIRE_PIN,   INPUT_PULLUP);

  XInput.begin();
  //Serial.begin(9600);
}

void loop() {
  SWITCH_BOOL    =  !digitalRead(SWITCH_PIN);
  RESET_BOOL     =  !digitalRead(RESET_PIN);
  
  DOWN_BOOL      =  !digitalRead(DOWN_PIN);
  RIGHT_BOOL     =  !digitalRead(RIGHT_PIN);
  LEFT_BOOL      =  !digitalRead(LEFT_PIN);
  UP_BOOL        =  !digitalRead(UP_PIN);
  
  FIRE_BOOL      =  !digitalRead(FIRE_PIN);

  // Configuration mode
  CONFIG_MODE = SWITCH_BOOL;

  if (CONFIG_MODE) {
    // If configuration mode is enabled
    // use up and down to change buttons...
    if (UP_BOOL && !UP_BOOL_LAST) {
      if (BUTTON_LIST_POS > 0) {
        BUTTON_LIST_POS--;
      } else {
        BUTTON_LIST_POS = BUTTON_LIST_SIZE - 1;
      }
      //Serial.println(BUTTON_LIST_POS);
    }
    if (DOWN_BOOL && !DOWN_BOOL_LAST) {
      if (BUTTON_LIST_POS < BUTTON_LIST_SIZE - 1) {
        BUTTON_LIST_POS++;
      } else {
        BUTTON_LIST_POS = 0;
      }
      //Serial.println(BUTTON_LIST_POS);
    }
    
    // use FIRE to toggle buttons...
    if (FIRE_BOOL && !FIRE_BOOL_LAST) {
      BUTTON_BOOL_LIST[BUTTON_LIST_POS] = !BUTTON_BOOL_LIST[BUTTON_LIST_POS];
    }

    // Send button press to the computer so the player
    // knows what button was activated.
    XInput.setButton(BUTTON_LIST[BUTTON_LIST_POS], FIRE_BOOL);
    XInput.send();
    
    // ...left and right to edit sequence.
    // If reset is also held...
    // ...up and down to save to memory...
    // ...left and right to select memory...
  } else {
    // If configuration mode is set to false...
    // reset swaps stick...
    if (RESET_BOOL && !RESET_BOOL_LAST) {
      STICK_SWAP = !STICK_SWAP;
    }
    // send controller inputs to the computer
    // Set XInput values
    if (FIRE_BOOL) {
      for (int i = 0; i <= BUTTON_LIST_SIZE-1; i++) {
        XInput.setButton(BUTTON_LIST[i], BUTTON_BOOL_LIST[i]);
      }
    } else if (FIRE_BOOL_LAST) {
      XInput.releaseAll();
    }
    if (!STICK_SWAP) {
      // Left stick is enabled
      XInput.setJoystick(JOY_LEFT, UP_BOOL, DOWN_BOOL, LEFT_BOOL, RIGHT_BOOL);
      XInput.setJoystick(JOY_RIGHT, false, false, false, false); // turn off old joy values after swap
    } else {
      // Right stick is enabled
      XInput.setJoystick(JOY_LEFT, false, false, false, false); // turn off old joy values after swap
      XInput.setJoystick(JOY_RIGHT, UP_BOOL, DOWN_BOOL, LEFT_BOOL, RIGHT_BOOL);
    }
  
    // Send control data to the computer
    XInput.send();

    // Reset button menu position
    BUTTON_LIST_POS = 0;
  }

  // Control the LED
  digitalWrite(LED_PIN, SWITCH_BOOL);

  // Store last values for debouncing.
  RESET_BOOL_LAST     =  RESET_BOOL;
  
  DOWN_BOOL_LAST      =  DOWN_BOOL;
  RIGHT_BOOL_LAST     =  RIGHT_BOOL;
  LEFT_BOOL_LAST      =  LEFT_BOOL_LAST;
  UP_BOOL_LAST        =  UP_BOOL;
  
  FIRE_BOOL_LAST      =  FIRE_BOOL;
  delay(10);
}
