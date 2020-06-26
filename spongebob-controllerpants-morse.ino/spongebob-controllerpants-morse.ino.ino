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
unsigned long RUN_TIME = 150;    // Value in milliseconds. Time after release until press is read and run.
unsigned long HOLD_TIME = 125;

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
unsigned long DELAY_TIMES[CODE_MAX+1];

// Reset
bool RESET_BUTTON, RESET_BUTTON_LAST;

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
  RESET_BUTTON = digitalRead(RESET_PIN);
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
    HOLD_BUTTON_START(FIND_BUTTON_POS(FIND_BUTTON()), FIND_BUTTON());
    
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
  
  // Hold/release buttons long enough so they register
  for (uint8_t i = 1; i <= CODE_MAX; i++) {
    HOLD_BUTTON_CHECK(i);
  }

  // Check buttons on release of RESET_BUTTON
  if (RESET_BUTTON && RESET_BUTTON != RESET_BUTTON_LAST) {
    for (uint8_t i = 1; i <= CODE_MAX; i++) {
      XInput.setButton(FIND_BUTTON_POS(i), false);
    }
  }
  
  XInput.send();
  
  // Store last BUTTON_PRESSED state for state change checking
  BUTTON_PRESSED_LAST = BUTTON_PRESSED;

 if (BUTTON_PRESSED) {
    RELEASE_TIME = 0;
 }

 RESET_BUTTON_LAST = RESET_BUTTON;
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

void HOLD_BUTTON_START(uint8_t v, int p) {
  if (RESET_BUTTON == !XInput.getButton(v)) {
    DELAY_TIMES[p] = TIME;
  }
  switch (p) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 9:
    case 10:
    case 11:
    case 12:
      XInput.setButton(v, !XInput.getButton(v));
      break;
    case 7:
    case 8:
      if (XInput.getTrigger(v) > 0) {
        XInput.setButton(v, false);
      } else {
        XInput.setButton(v, true);
      }
      break;
    case 13:
    case 14:
    case 15:
    case 16:
      XInput.setDpad(v, !XInput.getDpad(v));
      break;
  }
}

void HOLD_BUTTON_CHECK(uint8_t i) {
  if (DELAY_TIMES[i] != 0 && TIME - DELAY_TIMES[i] == HOLD_TIME) {
    HOLD_BUTTON_START(FIND_BUTTON_POS(i), i);
    DELAY_TIMES[i] = 0;
  }
}

uint8_t FIND_BUTTON_POS(uint8_t b) {
  switch (b) {
    case 1:
      return BUTTON_A;
      Serial.println("A");
      break;
    case 2:
      return BUTTON_B;
      Serial.println("B");
      break;
    case 3:
      return BUTTON_X;
      Serial.println("X");
      break;
    case 4:
      return BUTTON_Y;
      Serial.println("Y");
      break;
    case 5:
      return BUTTON_LB;
      Serial.println("LB");
      break;
    case 6:
      return BUTTON_RB;
      Serial.println("RB");
      break;
    case 7:
      return TRIGGER_LEFT;
      Serial.println("LEFT TRIGGER");
      break;
    case 8:
      return TRIGGER_RIGHT;
      Serial.println("RIGHT TRIGGER");
      break;
    case 9:
      return BUTTON_START;
      Serial.println("START");
      break;
    case 10:
      return BUTTON_BACK;
      Serial.println("BACK");
      break;
    case 11:
      return BUTTON_L3;
      Serial.println("L3");
      break;
    case 12:
      return BUTTON_R3;
      Serial.println("R3");
      break;
    case 13:
      return DPAD_UP;
      Serial.println("DPAD UP");
      break;
    case 14:
      return DPAD_DOWN;
      Serial.println("DPAD DOWN");
      break;
    case 15:
      return DPAD_LEFT;
      Serial.println("DPAD LEFT");
      break;
    case 16:
      return DPAD_RIGHT;
      Serial.println("DPAD RIGHT");
      break;
  }
}

void DEBUG_MORSE() {
  for (int i = 0; i < 10; i++) {
    Serial.print(MORSE[i]);
    Serial.print(" ");
  }
  Serial.println();
}
