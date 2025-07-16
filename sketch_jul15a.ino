#include <arduino.h>
#include <FastLED.h>

#define NUM_LEDS 75
#define PIN_0 14
#define PIN_1 10
#define PIN_2 19
#define PIN_3 15
#define PIN_4 6
#define PIN_5 7
#define PIN_6 8
#define PIN_7 9
#define BUTTON_PIN 12

CRGB strip0[NUM_LEDS];
CRGB strip1[NUM_LEDS];
CRGB strip2[NUM_LEDS];
CRGB strip3[NUM_LEDS];
CRGB strip4[NUM_LEDS];
CRGB strip5[NUM_LEDS];
CRGB strip6[NUM_LEDS];
CRGB strip7[NUM_LEDS];

const int RAINBOW_HUES = 2560;
CRGB rainbow[RAINBOW_HUES];

void setup() {
  FastLED.addLeds<WS2812B, PIN_0, RGB>(strip0, NUM_LEDS);
  FastLED.addLeds<WS2812B, PIN_1, RGB>(strip1, NUM_LEDS);
  FastLED.addLeds<WS2812B, PIN_2, RGB>(strip2, NUM_LEDS);
  FastLED.addLeds<WS2812B, PIN_3, RGB>(strip3, NUM_LEDS);
  FastLED.addLeds<WS2812B, PIN_4, RGB>(strip4, NUM_LEDS);
  FastLED.addLeds<WS2812B, PIN_5, RGB>(strip5, NUM_LEDS);
  FastLED.addLeds<WS2812B, PIN_6, RGB>(strip6, NUM_LEDS);
  FastLED.addLeds<WS2812B, PIN_7, RGB>(strip7, NUM_LEDS);

  fill_rainbow(rainbow, RAINBOW_HUES, 0, 1);
}

int curHue = 0;

void advanceHue() {
  curHue = (curHue + 1) % RAINBOW_HUES;
}

double cSpeed = 20;
double rSpeed = 3;
bool cSpeedUp = true;
bool rSpeedUp = true;
int cSpeedSameDirTimes = 0;
int rSpeedSameDirTimes = 0;
double cSpeedIncrement = 0.01;
double rSpeedIncrement = 0.002;

void setSpeeds() {
  double cSpeedSameDirChance = pow(0.995, cSpeedSameDirTimes);
  if (rand01() > cSpeedSameDirChance) {
    cSpeedUp = !cSpeedUp;
  }
  double rSpeedSameDirChance = pow(0.995, rSpeedSameDirTimes);
  if (rand01() > rSpeedSameDirChance) {
    rSpeedUp = !rSpeedUp;
  }
  if (cSpeedUp) {
    cSpeed += cSpeedIncrement;
  } else {
    cSpeed -= cSpeedIncrement;
  }
  if (rSpeedUp) {
    rSpeed += rSpeedIncrement;
  } else {
    rSpeed -= rSpeedIncrement;
  }
}

double cStartFactor = 17.0;
double rStartFactor = 1.2;
double cFactor = cStartFactor;
double rFactor = rStartFactor;
int speedUpCTimesMax = 800;
int curCTimes = (speedUpCTimesMax * 0.8);
bool cUp = true;
int speedUpRTimesMax = 519;
int curRTimes = 0;
bool rUp = true;


void setSpeeds2() {
  if (curCTimes > speedUpCTimesMax) {
    cUp = !cUp;
    curCTimes = 0;
  } else {
    curCTimes++;
  }
  if (curRTimes > speedUpRTimesMax) {
    rUp = !rUp;
    curRTimes = 0;
  } else {
    curRTimes++;
  }
  double baseCMod = (cStartFactor / ((double) speedUpCTimesMax)) * 2.0;
  double cMod = baseCMod + ((1.0 - rand01()) * baseCMod * 0.04);
  if (cUp) {
    cFactor += cMod;
  } else {
    cFactor -= cMod;
  }
  double baseRMod = (rStartFactor / ((double) speedUpRTimesMax)) * 0.65;
  double rMod = baseRMod + ((1.0 - rand01()) * baseRMod * 0.04);
  if (rUp) {
    rFactor += rMod;
  } else {
    rFactor -= rMod;
  }
}

void advanceRainbow() {
  setSpeeds2();
  for (int c = 0; c < 8; c++) {
    for (int r = 0; r < NUM_LEDS; r++) {
      int colorIndex = ((int) (((double) curHue) + (cFactor * c) + (rFactor * r)) + RAINBOW_HUES) % RAINBOW_HUES;
      // int colorIndex = max(min(round(fmod(((double) curHue) + (cSpeed * c) + (rSpeed * r), RAINBOW_HUES)), RAINBOW_HUES), 0);
      // int colorIndex = (curHue + (r) + (c * 6)) % RAINBOW_HUES; 
      setPixel(c, r, rainbow[colorIndex]);
    }
  }
  advanceHue();
}

CRGB* strip(int index) { 
  index = (index + 8) % 8;
  if (index == 0) {
    return strip0;
  } else if (index == 1) {
    return strip1;
  } else if (index == 2) {
    return strip2;
  } else if (index == 3) {
    return strip3;
  } else if (index == 4) {
    return strip4;
  } else if (index == 5) {
    return strip5;
  } else if (index == 6) {
    return strip6;
  } else if (index == 7) {
    return strip7;
  } else {
    return strip7;
  }
}

void setPixel(int col, int row, CRGB color) {
  strip(col)[row] = color;
}

void fadePixel(int col, int row, double factor) {
  strip(col)[row] = strip(col)[row].nscale8(factor * 256);
}

void fadeAll(double factor) {
  for (int i = 0; i < NUM_LEDS; i++) {
    for (int s = 0; s < 8; s++) {
      fadePixel(s, i, factor);
    }
  }
}

// double lateralChances[8] = {0.6, 0.8, 0.75, 0.5, 0.25, -1, -1};
double lateralChances[4] = {0.7, 0.6, 0.3, 0};
int curWalkPixel[2] = {0, 0};
int curWalkDir = 0;
int numUps = 1;
int prevLaterals = 0;
double walkFade = 0.91;

double rand01() {
  return (double) rand() / (double) RAND_MAX;
}

// -1 = left, 0 = up, 1 = right
void setNextWalkPixel(CRGB color) {
  fadePixel(curWalkPixel[0], curWalkPixel[1], 0.5);
  double randVal = (double) rand() / (double) RAND_MAX;
  if (numUps > 1 && randVal < lateralChances[prevLaterals]) {
    if (curWalkDir == 0) {
        double dirChoice = (double) rand() / (double) RAND_MAX;
        if (dirChoice > 0.5) {
          curWalkDir = -1;
        } else {
          curWalkDir = 1;
        }
    }
    numUps = 0;
    prevLaterals = prevLaterals + 1;
    curWalkPixel[0] = ((curWalkPixel[0] + curWalkDir) + 4) % 4;
  } else {
    numUps = numUps + 1;  
    curWalkDir = 0;
    prevLaterals = 0;
    curWalkPixel[1] = (curWalkPixel[1] + 1) % NUM_LEDS;
  }
  fadeAll(walkFade);
  setPixel(curWalkPixel[0], curWalkPixel[1], color);
}
 
void loop() {
  // setNextWalkPixel(CRGB::Cyan);
  advanceRainbow();
  // setPixel(0, 10, CRGB::Blue);
  // setPixel(1, 10, CRGB::Red);
  // setPixel(2, 10, CRGB::Green);
  // setPixel(3, 10, CRGB::Purple);
  // setPixel(4, 10, CRGB::Blue);
  // setPixel(5, 10, CRGB::Red);
  // setPixel(6, 10, CRGB::Green);
  // setPixel(7, 10, CRGB::Purple);
  fadeAll(0.18);
  delay(80);
  FastLED.show();
}
