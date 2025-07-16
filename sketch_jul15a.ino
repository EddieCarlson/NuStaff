#include <arduino.h>
#include <FastLED.h>

#define NUM_LEDS 75
#define PIN_0 2
#define PIN_1 3
#define PIN_2 4
#define PIN_3 5
#define PIN_4 6
#define PIN_5 8
#define PIN_6 7
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

void setup() {
  FastLED.addLeds<WS2812B, PIN_0, RGB>(strip0, NUM_LEDS);
  FastLED.addLeds<WS2812B, PIN_1, RGB>(strip1, NUM_LEDS);
  FastLED.addLeds<WS2812B, PIN_2, RGB>(strip2, NUM_LEDS);
  FastLED.addLeds<WS2812B, PIN_3, RGB>(strip3, NUM_LEDS);
  FastLED.addLeds<WS2812B, PIN_4, RGB>(strip4, NUM_LEDS);
  FastLED.addLeds<WS2812B, PIN_5, RGB>(strip5, NUM_LEDS);
  FastLED.addLeds<WS2812B, PIN_6, RGB>(strip6, NUM_LEDS);
  FastLED.addLeds<WS2812B, PIN_7, RGB>(strip7, NUM_LEDS);
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
  setNextWalkPixel(CRGB::Cyan);
  FastLED.show();
  delay(10);
}
