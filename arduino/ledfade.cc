/******** Natural Nerd LED "fade-between" colors *************
 *  To use the code for your own projects, update the parameters 
 *  marked with CONFIG.
 */

#include "FastLED.h"
#include <math.h>

/*************************************
 * CONFIG
 * How many leds are you controlling? 
 * Change it!
 *************************************/
#define NUM_LEDS 1
#define LED_PIN 6

/*************************************
 * CONFIG
 * Color declarations, RGB style
 * Feel free to add more colors 
 * That way more fades can be created!
 *************************************/
int purple[3] = {210, 0, 255};
int peach[3]  = {255, 90, 30};
int lime[3]   = {50, 255, 30};

/** Variable and function declarations */
CRGB leds[NUM_LEDS];
int i;
void fade_between(int a[3], int b[3], CRGB leds[NUM_LEDS], int del);
void set_definite(int a[3], CRGB leds[NUM_LEDS]);

/* Initialize the LEDs */
void setup() { 
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    for (i = 0; i < NUM_LEDS; i++) 
      leds[i] = CRGB(purple[0], purple[1], purple[2]);
    FastLED.show(); 
  }

/************************************* 
 * CONFIG
 *  Add or remove fades to your own liking. 
 *  Change the delay, which is the last parameter 
 *  of the fade_between(..) function to reduce 
 *  or increase the speed of the fade. The time
 *  to "hold" a color, is decided by the delay
 *  between the function calls. 
 *************************************/
void loop() {
      fade_between(purple, peach, leds, 60);
      delay(1000);
      fade_between(peach, lime, leds, 60);
      delay(1000);
      fade_between(lime, purple, leds, 60);
      delay(1000);
}

/**
 * Params: fade_between(color from, color-to, led-array, delay)
 */
void fade_between(int a[3], int b[3], CRGB leds[NUM_LEDS], int del) {
  int i, j, steps = 100;
  double dsteps = 100.0;
  double s1, s2, s3, tmp1, tmp2, tmp3;
  s1 = double((b[0] - a[0])) / dsteps; 
  s2 = double((b[1] - a[1])) / dsteps; 
  s3 = double((b[2] - a[2])) / dsteps; 
  
  tmp1 = a[0], tmp2 = a[1], tmp3 = a[2];
  
  for (i = 0; i < steps; i++) { 
      tmp1 += s1;
      tmp2 += s2; 
      tmp3 += s3; 
      
    for (j = 0; j < NUM_LEDS; j++)
      leds[j] = CRGB((int)round(tmp1), (int)round(tmp2), (int)round(tmp3)); 
    FastLED.show(); 
    delay(del);
  }
 set_definite(b, leds);
}

void set_definite(int a[3], CRGB leds[NUM_LEDS]) {
 int i; 
  for (i = 0; i < NUM_LEDS; i++) 
    leds[i] = CRGB(a[0], a[1], a[2]); 
    
   FastLED.show(); 
}
