#include "FastLED.h"

volatile unsigned long last_interrupt;
volatile byte on = LOW;

#define BUTTON_PIN 3
#define RELAY_PIN 7
#define LED_PIN 4
#define INTERRUPT_DELAY 25

#define NUM_TOP_LEDS 6
#define NUM_CIRCLE_LEDS 12
#define NUM_LEDS (NUM_TOP_LEDS + NUM_CIRCLE_LEDS)

CRGB leds[NUM_LEDS];
unsigned long last_tick;
int led_delay_ms = 400;
int led_counter = 0;

CRGB start_color = CRGB(255, 0, 135);
CRGB end_color = CRGB(0, 127, 255);

void setup() {
  pinMode(BUTTON_PIN, INPUT); 
  pinMode(RELAY_PIN, OUTPUT);
  
  digitalWrite(RELAY_PIN, HIGH);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), click, CHANGE);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  
  for (int i = 0; i < NUM_TOP_LEDS + NUM_CIRCLE_LEDS; i++) 
      leds[i] = CRGB(0, 0, 0);
      
  FastLED.show(); 
  last_tick = millis();
 
}

void loop() {
  if (on)
    return;
    
  if (millis() - last_tick > led_delay_ms) {
    last_tick = millis();
    
    if (led_counter == NUM_LEDS) {
      clear_leds();
    }
    else {
      leds[led_counter] = get_color();
      led_counter++;
      FastLED.show();
    }
  }
}

void clear_leds() {
  led_counter = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(0,0,0);
  }
  FastLED.show();
}

void start_gun() {
  clear_leds();
  digitalWrite(RELAY_PIN, LOW);
}

void stop_gun() {
  digitalWrite(RELAY_PIN, HIGH);
}

CRGB get_color() {
  CRGB color = CRGB(0, 0, 0);
  color.r = start_color.r + ((end_color.r - start_color.r) / NUM_LEDS) * led_counter;
  color.g = start_color.g + ((end_color.g - start_color.g) / NUM_LEDS) * led_counter;
  color.b = start_color.b + ((end_color.b - start_color.b) / NUM_LEDS) * led_counter;
  return color;
}

void click() {
  if (millis() - last_interrupt > INTERRUPT_DELAY) {
    last_interrupt = millis();
    on = !on;
    if (on) {
       start_gun();
    }
    else {
      stop_gun();
    }
  }
}
