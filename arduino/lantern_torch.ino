#include <FastLED.h>

/* Leds per addressable section. Normally this is 1, for me it was 3 */
#define LEDS_PER_SECTION 3

/* If LEDS_PER_SECTION is 1, the number of sections is your amount of LEDs */
#define SECTIONS 6

/* LED PIN */
#define DATA_PIN 6

/* BUTTON PIN for changing colors */
#define BUTTON_PIN 2

#define NUM_LEDS LEDS_PER_SECTION*SECTIONS
CRGB leds[NUM_LEDS];


CRGB colorsFire[] = {CRGB(255, 120, 3), CRGB(255, 80, 0), CRGB(255, 65, 0)};
CRGB colorsPurple[] = {CRGB(220, 120, 255), CRGB(170, 0, 255), CRGB(225, 0, 255)};
CRGB colorsGreen[] = {CRGB(50, 255, 5), CRGB(55, 255, 0), CRGB(140, 255, 0)};
CRGB colorsTeal[] = {CRGB(50, 255, 60), CRGB(0, 255, 90), CRGB(10, 255, 120)};
CRGB colorsBlue[] = {CRGB(50, 140, 140), CRGB(30, 200, 200), CRGB(20, 160, 255)};
CRGB colorsRed[] = {CRGB(120, 20, 0), CRGB(150, 0, 0), CRGB(255, 0, 0)};
CRGB colorsPink[] = {CRGB(255, 110, 120), CRGB(255, 0, 150), CRGB(255, 0, 70)};
CRGB colorsWhite[] = {CRGB(255, 255, 100), CRGB(200, 200, 60), CRGB(150, 150, 40)};
CRGB *colors[] = {colorsFire, colorsPurple, colorsGreen, colorsTeal, colorsBlue, colorsRed, colorsPink, colorsWhite};

/* Updates at 60hz */
float tickCount = 1000/60;

class LedSegment {
  private:
  int fadeSpeed, ledIndex;
  long lastFade, fadeTimeRemaining;
  bool fading;
  float fadeVector[3];
  CRGB currentColor;
  public:
  LedSegment(int idx, int f_speed) {
    fadeSpeed = f_speed;
    ledIndex = idx;
  }
  
  void setColor(CRGB color) {
    leds[ledIndex] = color;
    currentColor = color;
    fading = true;
    fadeVector[0] = (float)color.r / (float)fadeSpeed;
    fadeVector[1] = (float)color.g / (float)fadeSpeed;
    fadeVector[2] = (float)color.b / (float)fadeSpeed;
    lastFade = millis();
    fadeTimeRemaining = fadeSpeed;
  }

  
  void fade() {
    if (fading) {
      long millisSinceLast = millis() - lastFade;
      lastFade = millis();
      long fadeMultiplier = (millisSinceLast >= fadeTimeRemaining) ? fadeTimeRemaining : millisSinceLast;
      fadeTimeRemaining = max(fadeTimeRemaining - millisSinceLast, 0);
      currentColor = CRGB(
        currentColor.r - min(fadeVector[0] * fadeMultiplier, currentColor.r), 
        currentColor.g - min(fadeVector[1] * fadeMultiplier, currentColor.g),
        currentColor.b - min(fadeVector[2] * fadeMultiplier, currentColor.b)
      );        
      if (fadeTimeRemaining == 0)
        fading = false;
      leds[ledIndex] = currentColor;
    }
  }
  
};


class ColorRow {
  public:
  unsigned long last_check;
  LedSegment* myLeds[3];
  CRGB colors[3];
  
  ColorRow(int idx1, int idx2, int idx3, CRGB colorset[3]) {
    int fadeTime = random(500, 3000);
    updateColors(colorset);
    myLeds[0] = new LedSegment(idx1, fadeTime);
    myLeds[1] = new LedSegment(idx2, fadeTime);
    myLeds[2] = new LedSegment(idx3, fadeTime);
    last_check = millis();
    
  }

  void updateColors(CRGB colorset[3]) {
    colors[0] = colorset[0];
    colors[1] = colorset[1];
    colors[2] = colorset[2];
  }
  
  void updateLeds() {
    for (int i = 0; i < 3; i++) {
      myLeds[i]->fade();
    }
  }
  void setTop() {
    myLeds[2]->setColor(colors[2]);
  }
  void setMid() {
    myLeds[1]->setColor(colors[1]);
  }
  void setBase() {
    myLeds[0]->setColor(colors[0]);
  }
  
  void fire() {
    int i = random(0, 0.5 * 60);
    switch (i) {
      case 1:
        setTop();
        setBase();
        setMid();
        break;
      case 2:
      case 3:
        setBase();
        setMid();
        break;
      case 4:
      case 5:
        setBase();
      
    }
  }
  void tick() {
    if (millis() - last_check > tickCount) {
      last_check = millis();
      fire();
      FastLED.show();
      
    }

    updateLeds();

  }
};

ColorRow* first;
ColorRow* second;
ColorRow* third;
ColorRow* fourth;
ColorRow* fifth;
ColorRow* sixth;
ColorRow* allRows[SECTIONS];
int currentColor = 0;
CRGB* colorset = colors[currentColor];


void rowsTick() {
    for (int i = 0; i < SECTIONS; i++)
    {
      allRows[i]->tick();
    }
}


void setColor() {
    for (int i = 0; i < SECTIONS; i++)
    {
      allRows[i]->updateColors(colors[currentColor]);
    }
}

void setup() { 
  FastLED.addLeds<WS2812, DATA_PIN, BRG>(leds, NUM_LEDS);
  Serial.begin(9600);
  bool rising = true;
  int idx = 0;
  for (int i = 0; i < SECTIONS; i++)
  {
      /* This part here is here because I have soldered the led rows from top
         to the top of the next, then from the bottom to the bottom of the next, so the
         index of which led is at the "top" will be alternate each round
       */
      int idx = LEDS_PER_SECTION * i;
      if (rising) {
        allRows[i] = new ColorRow(idx, idx + 1, idx + 2, colorset);
      }
      else {
        allRows[i] = new ColorRow(idx + 2, idx + 1, idx, colorset);
      }
      rising = !rising;
  }
  pinMode(BUTTON_PIN, INPUT_PULLUP);

}

int buttonState = 0;
int state = 0;

void loop() {
  rowsTick();
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState != state) {
    state = buttonState;
    if (state == 0) {
         currentColor = (currentColor + 1) % (sizeof(colors)/sizeof(CRGB*));
         colorset = colors[currentColor];
         setColor();
    }
  }

  delay(5);
}
