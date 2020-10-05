
#include "Mouse.h"
#include "math.h"
#include "micromouse.h"
#include "Vector.h"
#define DEG_TO_RAD(X) (M_PI*(X)/180)


#define WHITE 5
#define GREEN 9
#define RED 10
#define BLUE 11

#define BUTTON A0

#define DOWN 1
#define UP 0
#define LEFT 2
#define RIGHT 3

enum Mode { mouse, scroll };

volatile MathVector sense;

//movement parameters
MathVector scale(10, 10);
//keep a rolling sum of movement over the last _acceleration_ periods
MathVector history[32];
byte acceleration = 8;  // ammount of acceleration to apply - max 32
MathVector sum;

bool debug = true;

/*******************************************/
void setup() {

  pinMode(BUTTON, INPUT);
  pinMode(UP, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(WHITE, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(UP), upHandler, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DOWN), downHandler, CHANGE);
  attachInterrupt(digitalPinToInterrupt(LEFT), leftHandler, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RIGHT), rightHandler, CHANGE);

  Mouse.begin();
  Serial.begin(115200);
}


/*******************************************/
void loop() {

  static int frequency = 125;
  static int period = 1000 / frequency;
  static Mode mode = mouse;
  static long updateTime = millis() + period;


  //get movement from volatile variables
  MathVector distance;
  distance.x = sense.x;
  sense.x = 0;
  distance.y = sense.y;
  sense.y = 0;

  switch (mode) {

    case mouse :
      mouseMove(distance);
      ledWrite(animateMouse(period));
      break;

    case scroll :
      mouseScroll(distance);
      ledWrite(animateScroll(period));
      break;
  }

  modeButton(mode);

  //pause until the update period has elapsed
  while (millis() < updateTime) {
    ;
  }

  updateTime += period;

}
/*******************************************/

//switch between mouse and scroll mode on button press and release
void modeButton(Mode& mode) {

  static bool modePressed = false;

  // if the mouse button is pressed:
  if (!digitalRead(BUTTON)) {
    // if the mouse is not pressed, press it:
    if (!modePressed) {
      modePressed = true;
    }
  }
  // else the mouse button is not pressed:
  else {
    // if the mouse is pressed, release it and change modes:
    if (modePressed) {
      modePressed = false;
      if (mode == mouse)
        mode = scroll;
      else
        mode = mouse;
    }
  }
}


HSI animateMouse(int period) {
  static HSI color = {330, 0.67, 1 }; // pink
  static float top = 1;
  static float bottom = 0.6;
  static float increment = (top - bottom) / (1000 / period);
  static float dir = 1;

  color.intensity += increment * dir;

  if (color.intensity >= top)
    dir = -1;

  if (color.intensity <= bottom)
    dir = 1;

  return color;
}

HSI animateScroll(int period) {
  static HSI color = {10, .55, 1}; // rose gold
  static float top = 1;
  static float bottom = 0.6;
  static float increment = (top - bottom) / (2000 / period);
  static float dir = 1;

  color.intensity += increment * dir;

  if (color.intensity >= top)
    dir = -1;

  if (color.intensity <= bottom)
    dir = 1;

  return color;
}

void mouseMove(MathVector distance) {

  static uint8_t i = 0;


  sum -= history[i];
  sum += distance;
  history[i] = distance;
  i++;
  i %= acceleration;

  distance = sum *  scale;

  distance.x = bound(distance.x, 127);
  distance.y = bound(distance.y, 127);
  
  //send movement to OS if there is any
  if (!distance.isZero()) {
    Mouse.move(distance.x, distance.y, 0);
    if(debug) {
      Serial.print("\t\tDist:\t");
      Serial.print(distance);
      Serial.print("\t\tScale:\t");
      Serial.print(scale);
      Serial.print("\t\tSum:\t");
      Serial.println(sum);
    }
  }
}

int bound(int input, int limit) {
  if(input > limit)
    return limit;
 
  if(input < -limit)
    return -limit;

  return input;
 
}
void mouseScroll(MathVector distance) {
  if (distance.y != 0) {
    Mouse.move(0, 0, distance.y);
  }
}

void ledWrite(HSI color) {
  int r, g, b, w;
  float cos_h, cos_1047_h;
  float H = color.hue;
  float S = color.saturation;
  float I = color.intensity;

  H = fmod(H, 360); // cycle H around to 0-360 degrees
  H = 3.14159 * H / (float)180; // Convert to radians.
  S = S > 0 ? (S < 1 ? S : 1) : 0; // clamp S and I to interval [0,1]
  I = I > 0 ? (I < 1 ? I : 1) : 0;

  if (H < 2.09439) {
    cos_h = cos(H);
    cos_1047_h = cos(1.047196667 - H);
    r = S * 255 * I / 3 * (1 + cos_h / cos_1047_h);
    g = S * 255 * I / 3 * (1 + (1 - cos_h / cos_1047_h));
    b = 0;
    w = 255 * (1 - S) * I;
  } else if (H < 4.188787) {
    H = H - 2.09439;
    cos_h = cos(H);
    cos_1047_h = cos(1.047196667 - H);
    g = S * 255 * I / 3 * (1 + cos_h / cos_1047_h);
    b = S * 255 * I / 3 * (1 + (1 - cos_h / cos_1047_h));
    r = 0;
    w = 255 * (1 - S) * I;
  } else {
    H = H - 4.188787;
    cos_h = cos(H);
    cos_1047_h = cos(1.047196667 - H);
    b = S * 255 * I / 3 * (1 + cos_h / cos_1047_h);
    r = S * 255 * I / 3 * (1 + (1 - cos_h / cos_1047_h));
    g = 0;
    w = 255 * (1 - S) * I;
  }

  analogWrite(RED, gamma8[r]);
  analogWrite(GREEN, g);
  analogWrite(BLUE, gamma8[b]);
  analogWrite(WHITE, gamma8[w]);

}

void upHandler() {
  sense.y++;
}

void downHandler() {
  sense.y--;
}

void leftHandler() {
  sense.x--;
}

void rightHandler() {
  sense.x++;
}
