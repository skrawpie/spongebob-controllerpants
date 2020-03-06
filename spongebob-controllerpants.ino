#include <XInput.h>
#define LOOP_AFTER_LAST 1
#define REPEAT_AFTER_LAST 2

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

bool CONFIG_MODE = false;
bool CONFIG_MODE_LAST = false;
int BUTTON_LIST_POS = 0;

bool STICK_SWAP = false;

int SEQUENCE_POS = 0;
int SEQUENCE_SIZE = 0;                // Current size of the sequnce
const int SEQUENCE_MAX = 5;           // Maximum steps in the sequence
int SEQUENCE_BEHAVIOR = LOOP_AFTER_LAST;

bool SEQUENCE_OF_BUTTONS[BUTTON_LIST_SIZE][SEQUENCE_MAX] = {
  {false, false, false, false, false},
  {false, false, false, false, false},
  {false, false, false, false, false},
  {false, false, false, false, false},
  {false, false, false, false, false},
  {false, false, false, false, false},
  {false, false, false, false, false},
  {false, false, false, false, false},
  {false, false, false, false, false},
  {false, false, false, false, false},
  {false, false, false, false, false},
  {false, false, false, false, false},
  {false, false, false, false, false},
  {false, false, false, false, false},
  {false, false, false, false, false},
  {false, false, false, false, false},
};

int TIME_HELD = 0;

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

void reset() {
  SEQUENCE_POS = 0;
  SEQUENCE_SIZE = 0;                // Current size of the sequnce
  SEQUENCE_BEHAVIOR = REPEAT_AFTER_LAST; // it doesn't actually reset to this value idk why but hey it works
  TIME_HELD = 0;

  for (int i = 0; i < BUTTON_LIST_SIZE; i++) {
    for (int j = 0; j < SEQUENCE_MAX; j++) {
      SEQUENCE_OF_BUTTONS[i][j] = false;
    }
  }
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

  // Reset pertinent values when CONFIG_MODE toggles
  if (CONFIG_MODE != CONFIG_MODE_LAST){
    // Reset button menu position
    BUTTON_LIST_POS = 0;

    // Reset sequence menu position
    SEQUENCE_POS = 0;
  }

  // Allow editing of buttons while in CONFIG_MODE
  // make and send inputs to computer out of CONFIG_MODE
  if (CONFIG_MODE) {
    // If configuration mode is enabled
    // use up and down to change buttons...
    if (UP_BOOL && !UP_BOOL_LAST) {
      if (BUTTON_LIST_POS > 0) {
        BUTTON_LIST_POS--;
      } else {
        BUTTON_LIST_POS = BUTTON_LIST_SIZE - 1;
      }
    }
    if (DOWN_BOOL && !DOWN_BOOL_LAST) {
      if (BUTTON_LIST_POS < BUTTON_LIST_SIZE - 1) {
        BUTTON_LIST_POS++;
      } else {
        BUTTON_LIST_POS = 0;
      }
    }

    // left and right to edit sequence...
    if (LEFT_BOOL && !LEFT_BOOL_LAST){  // Cap sequence at zero instead of wrapping.
      if (SEQUENCE_POS > 0) {
        SEQUENCE_POS--;
        SEQUENCE_SIZE--;
      } else {
        SEQUENCE_POS = 0;
        SEQUENCE_SIZE = 0;
      }
    }
    if (RIGHT_BOOL && !RIGHT_BOOL_LAST){  // Cap sequence at max instead of wrapping.
      if (SEQUENCE_POS < SEQUENCE_MAX - 1) {
        SEQUENCE_POS++;
        SEQUENCE_SIZE++;
        BUTTON_LIST_POS = 0;
      } else {
        SEQUENCE_POS = SEQUENCE_MAX - 1;
        SEQUENCE_SIZE = SEQUENCE_MAX - 1;
        BUTTON_LIST_POS = 0;
      }
    }
    
    // use FIRE to toggle buttons in the appropriate sequence...
    if (FIRE_BOOL && !FIRE_BOOL_LAST) {
      SEQUENCE_OF_BUTTONS[BUTTON_LIST_POS][SEQUENCE_POS] = !SEQUENCE_OF_BUTTONS[BUTTON_LIST_POS][SEQUENCE_POS];
    }

    // Send button press to the computer so the player
    // knows what button was activated.
    XInput.setButton(BUTTON_LIST[BUTTON_LIST_POS], FIRE_BOOL);
    XInput.send();  // other send is called not in config mode so we repeat it here to avoid issues

    // Press reset to change sequence behavior 
    // hold reset to reset everything.
    // Incrememnt time while the button is held
    if (RESET_BOOL) {
      TIME_HELD++;
    }

    // If it wasn't held long enough
    // swap the sequence behavior
    if (RESET_BOOL != RESET_BOOL_LAST) {
      if (!RESET_BOOL) {
        if (TIME_HELD < 500) {
          switch (SEQUENCE_BEHAVIOR) {
            case LOOP_AFTER_LAST:
              SEQUENCE_BEHAVIOR = REPEAT_AFTER_LAST;
              break;
            case REPEAT_AFTER_LAST:
              SEQUENCE_BEHAVIOR = LOOP_AFTER_LAST;
              break;
          }
          TIME_HELD = 0;
        }
      }
    }

    // If it was held long enough
    // blink the LED five times and reset pertinent values
    if (TIME_HELD >= 500) {
      digitalWrite(LED_PIN, LOW);
      delay(100);
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
      digitalWrite(LED_PIN, HIGH);
      reset();
    }
  } else {
    // If configuration mode is set to false...
    // set XInput values...
    if (FIRE_BOOL) {
      for (int i = 0; i <= BUTTON_LIST_SIZE - 1; i++) {
        XInput.setButton(BUTTON_LIST[i], SEQUENCE_OF_BUTTONS[i][SEQUENCE_POS]);
      }
    } else if (FIRE_BOOL_LAST) {
      XInput.releaseAll();
    }
    
    // increment sequence...
    if (!FIRE_BOOL && FIRE_BOOL_LAST) {
      // Loop
      if (SEQUENCE_BEHAVIOR == LOOP_AFTER_LAST) {
        if (SEQUENCE_POS < SEQUENCE_SIZE) {
          SEQUENCE_POS++;
        } else {
          SEQUENCE_POS = 0;
        }
      }
      // Repeat last
      if (SEQUENCE_BEHAVIOR == REPEAT_AFTER_LAST) {
        if (SEQUENCE_POS < SEQUENCE_SIZE) {
          SEQUENCE_POS++;
        } else {
          SEQUENCE_POS = SEQUENCE_SIZE;
        }
      }
    }
    
    // reset swaps stick...
    if (RESET_BOOL && !RESET_BOOL_LAST) {
      STICK_SWAP = !STICK_SWAP;
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
  
    // send control data to the computer
    XInput.send();
  }

  // Control the LED
  digitalWrite(LED_PIN, SWITCH_BOOL);

  // Store last values for state change.
  RESET_BOOL_LAST     =  RESET_BOOL;
  
  DOWN_BOOL_LAST      =  DOWN_BOOL;
  RIGHT_BOOL_LAST     =  RIGHT_BOOL;
  LEFT_BOOL_LAST      =  LEFT_BOOL_LAST;
  UP_BOOL_LAST        =  UP_BOOL;
  
  FIRE_BOOL_LAST      =  FIRE_BOOL;

  CONFIG_MODE_LAST    = CONFIG_MODE;

  // Delay for debounce.
  delay(10);
}
