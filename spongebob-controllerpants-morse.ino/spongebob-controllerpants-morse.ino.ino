#include <DebounceInput.h>
#include <XInput.h>

// Pin declarations
const int LED_PIN           =  0;
const int SWITCH_PIN        =  1;
const int RESET_PIN         =  2;

const int DOWN_PIN          =  3;
const int RIGHT_PIN         =  4;
const int LEFT_PIN          =  5;
const int UP_PIN            =  6;

const int BUTTON_PIN        =  7;

DebouncedInput BUTTON(BUTTON_PIN);

// Assign DOT and DASH numbers so arrays are way simpler.
const int DOT = 1;
const int DASH = 2;

unsigned long DASH_LENGTH = 125; // Value in milliseconds. DOT_LENGTH would be any value < 125.
unsigned long RUN_TIME = 250;    // Value in milliseconds. Time after release until press is read and run.

int MORSE[100];
int MORSE_POS = 0;
int MORSE_MAX = 0;

const int CODE_MAX = 16;
const int CODE_LENGTH = 4;
int CODE[CODE_MAX][CODE_LENGTH] = {
  {DOT,   0,    0,    0},         // A
  {DASH,  0,    0,    0},         // B
  {DOT,   DOT,  0,    0},         // X
  {DASH,  DASH, 0,    0},         // Y
  {DASH,  DOT,  0,    0},         // LEFT BUMPER
  {DOT,   DASH, 0,    0},         // RIGHT BUMPER
  {DASH,  DASH, DOT,  0},         // LEFT TRIGGER
  {DOT,   DASH, DASH, 0},         // RIGHT TRIGGER
  {DASH,  DOT,  DASH, 0},         // START
  {DASH,  DASH, DASH, 0},         // BACK
  {DASH,  DOT,  DOT,  DOT},       // L3
  {DOT,   DOT,  DOT,  DASH},      // R3
  {DOT,   DOT,  DOT,  DOT},       // DPAD UP
  {DASH,  DASH, DASH, DASH},      // DPAD DOWN
  {DASH,  DOT,  DASH, DOT},       // DPAD LEFT
  {DOT,   DASH, DOT,  DASH}       // DPAD RIGHT
};

// Button variables
bool BUTTON_PRESSED, BUTTON_PRESSED_LAST;
unsigned long TIME, PRESS_TIME, RELEASE_TIME;
int DELAY_TIMES[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int DELAY_MAX = 2500;

// Booleans for stick.
bool LEFT_UP = false;
bool LEFT_DOWN = false;
bool LEFT_LEFT = false;
bool LEFT_RIGHT = false;
bool RIGHT_UP = false;
bool RIGHT_DOWN = false;
bool RIGHT_LEFT = false;
bool RIGHT_RIGHT = false;
bool POV_UP = false;
bool POV_DOWN = false;
bool POV_LEFT = false;
bool POV_RIGHT = false;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN,    OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(RESET_PIN,  INPUT_PULLUP);
  pinMode(DOWN_PIN,   INPUT_PULLUP);
  pinMode(RIGHT_PIN,  INPUT_PULLUP);
  pinMode(LEFT_PIN,   INPUT_PULLUP);
  pinMode(UP_PIN,     INPUT_PULLUP); 
  XInput.begin();
  Serial.begin(9600);
}

void loop() {
  TIME = millis();
  
  // Assign variables
  BUTTON.read();
  
  // Determine whether button press was dot or dash.
  if (BUTTON.falling()) {
    // Store the time when the button is pressed.
    PRESS_TIME = 0;
    PRESS_TIME = TIME;

    // Debug
    //Serial.println("BUTTON PRESSED");
  } else if (BUTTON.rising()) {
    // Store the time when the button is released.
    RELEASE_TIME = 0;
    RELEASE_TIME = TIME;
    // Store DOT or DASH in MORSE.
    MORSE[MORSE_POS] = DOT_OR_DASH();
    MORSE_POS++;

    // Debug
    //Serial.println("BUTTON RELEASED");
    DEBUG_MORSE();
  }

  // If time since release is greater than RUN_TIME, do the needful.
  if (TIME - RELEASE_TIME == RUN_TIME && BUTTON.high()) {
    // Store the current position of MORSE as MORSE_MAX.
    // Subtract one so we have the true max.
    MORSE_MAX = MORSE_POS;
    Serial.print("MORSE MAX: ");
    Serial.println(MORSE_MAX);

    // Compare MORSE to CODE to sort out what needs to be done.
    switch (FIND_BUTTON()) {
      case 1:
        XInput.setButton(BUTTON_A, true);
        Serial.println("A");
        break;
      case 2:
        XInput.setButton(BUTTON_B, true);
        Serial.println("B");
        break;
      case 3:
        XInput.setButton(BUTTON_X, true);
        Serial.println("X");
        break;
      case 4:
        XInput.setButton(BUTTON_Y, true);
        Serial.println("Y");
        break;
      case 5:
        XInput.setButton(BUTTON_LB, true);
        Serial.println("LB");
        break;
      case 6:
        XInput.setButton(BUTTON_RB, true);
        Serial.println("RB");
        break;
      case 7:
        XInput.setButton(TRIGGER_LEFT, true);
        Serial.println("LEFT TRIGGER");
        break;
      case 8:
        XInput.setButton(TRIGGER_RIGHT, true);
        Serial.println("RIGHT TRIGGER");
        break;
      case 9:
        XInput.setButton(BUTTON_START, true);
        Serial.println("START");
        break;
      case 10:
        XInput.setButton(BUTTON_BACK, true);
        Serial.println("BACK");
        break;
      case 11:
        XInput.setButton(BUTTON_L3, true);
        Serial.println("L3");
        break;
      case 12:
        XInput.setButton(BUTTON_R3, true);
        Serial.println("R3");
        break;
      case 13:
        POV_UP = true;
        Serial.println("DPAD UP");
        break;
      case 14:
        POV_DOWN = true;
        Serial.println("DPAD DOWN");
        break;
      case 15:
        POV_LEFT = true;
        Serial.println("DPAD LEFT");
        break;
      case 16:
        POV_RIGHT = true;
        Serial.println("DPAD RIGHT");
        break;
    }
    
    // Reset array.
    memset(MORSE, 0, sizeof(MORSE));
    MORSE_POS = 0;
    MORSE_MAX = 0;
    
    RELEASE_TIME = 0;

    // Debug
    Serial.println("RUN");
  }

  if (digitalRead(SWITCH_PIN) == LOW) {
    digitalWrite(LED_PIN, LOW);
    XInput.setJoystick(JOY_LEFT, !digitalRead(UP_PIN), !digitalRead(DOWN_PIN), !digitalRead(LEFT_PIN), !digitalRead(RIGHT_PIN));
  } else {
    digitalWrite(LED_PIN, HIGH);
    XInput.setJoystick(JOY_RIGHT, !digitalRead(UP_PIN), !digitalRead(DOWN_PIN), !digitalRead(LEFT_PIN), !digitalRead(RIGHT_PIN));
  }
  
  // Reset buttons after delay unless reset is held
  if (digitalRead(RESET_PIN)) {
    if (XInput.getButton(BUTTON_A)) {
      DELAY_TIMES[0]++;
      if (DELAY_TIMES[0] >= DELAY_MAX) {
        DELAY_TIMES[0] = 0;
        XInput.setButton(BUTTON_A, false);
      }
    }
    if (XInput.getButton(BUTTON_B)) {
      DELAY_TIMES[1]++;
      if (DELAY_TIMES[1] >= DELAY_MAX) {
        DELAY_TIMES[1] = 0;
        XInput.setButton(BUTTON_B, false);
      }
    }
    if (XInput.getButton(BUTTON_X)) {
      DELAY_TIMES[2]++;
      if (DELAY_TIMES[2] >= DELAY_MAX) {
        DELAY_TIMES[2] = 0;
        XInput.setButton(BUTTON_X, false);
      }
    }
    if (XInput.getButton(BUTTON_Y)) {
      DELAY_TIMES[3]++;
      if (DELAY_TIMES[3] >= DELAY_MAX) {
        DELAY_TIMES[3] = 0;
        XInput.setButton(BUTTON_Y, false);
      }
    }
    if (XInput.getButton(BUTTON_LB)) {
      DELAY_TIMES[4]++;
      if (DELAY_TIMES[4] >= DELAY_MAX) {
        DELAY_TIMES[4] = 0;
        XInput.setButton(BUTTON_LB, false);
      }
    }
    if (XInput.getButton(BUTTON_RB)) {
      DELAY_TIMES[5]++;
      if (DELAY_TIMES[5] >= DELAY_MAX) {
        DELAY_TIMES[5] = 0;
        XInput.setButton(BUTTON_RB, false);
      }
    }
    if (XInput.getTrigger(TRIGGER_LEFT) > 0) {
      DELAY_TIMES[6]++;
      if (DELAY_TIMES[6] >= DELAY_MAX) {
        DELAY_TIMES[6] = 0;
        XInput.setButton(TRIGGER_LEFT, false);
      }
    }
    if (XInput.getTrigger(TRIGGER_RIGHT) > 0) {
      DELAY_TIMES[7]++;
      if (DELAY_TIMES[7] >= DELAY_MAX) {
        DELAY_TIMES[7] = 0;
        XInput.setButton(TRIGGER_RIGHT, false);
      }
    }
    if (XInput.getButton(BUTTON_START)) {
      DELAY_TIMES[8]++;
      if (DELAY_TIMES[8] >= DELAY_MAX) {
        DELAY_TIMES[8] = 0;
        XInput.setButton(BUTTON_START, false);
      }
    }
    if (XInput.getButton(BUTTON_BACK)) {
      DELAY_TIMES[9]++;
      if (DELAY_TIMES[9] >= DELAY_MAX) {
        DELAY_TIMES[9] = 0;
        XInput.setButton(BUTTON_BACK, false);
      }
    }
    if (XInput.getButton(BUTTON_L3)) {
      DELAY_TIMES[10]++;
      if (DELAY_TIMES[10] >= DELAY_MAX) {
        DELAY_TIMES[10] = 0;
        XInput.setButton(BUTTON_L3, false);
      }
    }
    if (XInput.getButton(BUTTON_R3)) {
      DELAY_TIMES[11]++;
      if (DELAY_TIMES[11] >= DELAY_MAX) {
        DELAY_TIMES[11] = 0;
        XInput.setButton(BUTTON_R3, false);
      }
    }
    if (POV_UP) {
      DELAY_TIMES[12]++;
      if (DELAY_TIMES[12] >= DELAY_MAX) {
        DELAY_TIMES[12] = 0;
        POV_UP = false;
      }
    }
    if (POV_DOWN) {
      DELAY_TIMES[13]++;
      if (DELAY_TIMES[13] >= DELAY_MAX) {
        DELAY_TIMES[13] = 0;
        POV_DOWN = false;
      }
    }
    if (POV_LEFT) {
      DELAY_TIMES[14]++;
      if (DELAY_TIMES[14] >= DELAY_MAX) {
        DELAY_TIMES[14] = 0;
        POV_LEFT = false;
      }
    }
    if (POV_RIGHT) {
      DELAY_TIMES[15]++;
      if (DELAY_TIMES[15] >= DELAY_MAX) {
        DELAY_TIMES[15] = 0;
        POV_RIGHT = false;
      }
    }
  }
  
  XInput.setDpad(POV_UP, POV_DOWN, POV_LEFT, POV_RIGHT);
  
  XInput.send();
  
  // Store last BUTTON_PRESSED state for state change checking
  BUTTON_PRESSED_LAST = BUTTON_PRESSED;

 if (BUTTON_PRESSED) {
    RELEASE_TIME = 0;
 }
}

int DOT_OR_DASH() {
  unsigned long t = RELEASE_TIME - PRESS_TIME;
  if (t >= DASH_LENGTH) {
    return DASH;
  } else {
    return DOT;
  }
}

uint8_t FIND_BUTTON() {
  for (uint8_t i = 1; i <= CODE_MAX; i++) { // Loop through all the codes.
    int c = 0;
    for (int j = 0; j < MORSE_MAX; j++) {  // Loop through the current code.
      if (MORSE[j] == CODE[i-1][j]) {
        c++;
        Serial.print("LOOP COUNT: ");
        Serial.println(c);
        if (c == MORSE_MAX) {
          Serial.print("CASE: ");
          Serial.println(i);
          return i;
        }
      }
    }
  }
  return 0;
}

void DEBUG_MORSE() {
  for (int i = 0; i < 10; i++) {
    Serial.print(MORSE[i]);
    Serial.print(" ");
  }
  Serial.println();
}
