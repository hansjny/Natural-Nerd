#include <FastLED.h>

#define LED_PIN 3
#define NUM_LEDS 105
#define NUM_ROWS 7
#define LEDS_IN_ROW 15
#define ANIMATION_STEPS 15

CRGB leds[NUM_LEDS];
uint8_t pattern[ANIMATION_STEPS] = {0, 30, 60, 90, 120, 150, 180, 200, 180, 160, 120, 90, 60, 30, 0};
float brightness = 0.6;
uint8_t offset = 0; 
void setup() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 255, 0);
  }
  Serial.begin(9600);
  FastLED.show();
}


void loop() {

  for (int j = 0; j < NUM_ROWS; j++) {
    uint8_t value = pattern[(j + offset) % ANIMATION_STEPS];
    for (int i = 0; i < LEDS_IN_ROW; i++) {
        leds[LEDS_IN_ROW * j + i] = CRGB(value * brightness, 0, value * brightness);
    }
  }
  offset++;
  FastLED.show();
  delay(75);
}
