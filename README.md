# Spongebob Controllerpants

Spongebob Controllerpants is a conversion of a JAKKS Pacific Spongebob Squarepants Plug'n'Play to a functional XINPUT controller using a Teensy 3.2. 

## Using the controller

The original controller has one FIRE BUTTON, four buttons for directional input, a reset button, and a switch. It also has one LED.

### Morse version (preferred)

FIRE BUTTON behavior is selected by inputting various codes simlar to morse (i.e. DOTS and DASHES).

The codes are as follows:

```
A		DOT
B		DASH
X		DOT,   DOT
Y		DASH,  DASH
LEFT BUMPER	DASH,  DOT
RIGHT BUMPER	DOT,   DASH
LEFT TRIGGER	DASH,  DASH, DOT
RIGHT TRIGGER	DOT,   DASH, DASH
START		DASH,  DOT,  DASH
BACK		DASH,  DASH, DASH
L3		DASH,  DOT,  DOT,  DOT
R3		DOT,   DOT,  DOT,  DASH
DPAD UP		DOT,   DOT,  DOT,  DOT
DPAD DOWN	DASH,  DASH, DASH, DASH
DPAD LEFT	DASH,  DOT,  DASH, DOT
DPAD RIGH	DOT,   DASH, DOT,  DASH
```

The directional buttons control the two analog sticks. Toggling the switch, you can alternate between controlling the left and right sticks. The LED is used to indicate which analog is selected. `LED OFF = LEFT STICK` and `LED ON = RIGHT STICK`.

The reset button is used to hold presses. Normally, the presses only register as a tap after being entered, but by holding reset buttons will not be release until the reset button is released.

### Menu version

Under construction

## Credits

Under construction

