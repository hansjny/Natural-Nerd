/* Written guide by bennyd93 (https://github.com/bennyd93): https://github.com/hansjny/Natural-Nerd/blob/master/arduino/readme.md */
#include <SPI.h>
#include <SD.h>
#include <FastLED.h>

#define DATA_PIN 6
#define BUTTON_PIN 2

/* If LEDS_PER_SECTION is 1, the number of sections is your amount of LEDs */
#define ROWS 12

/* LED PIN */
#define LEDS_PER_ROW 30

#define NUM_LEDS LEDS_PER_ROW*ROWS

#define BRIGHTNESS_DIVIDER 3

//The CS pin on the SDCARD
#define CHIP_SELECT SS

CRGB leds[NUM_LEDS];

uint8_t stepsInAnimation = 0;
uint8_t currentStep = 1;
uint16_t bytesRead = 0;


File root;
File readFile;

volatile bool change_animation = false;

unsigned long last_interrupt;
char fname[12];

bool selectFile() {
  File entry = root.openNextFile();
  while(!entry) {
    root.rewindDirectory();
    entry.close();
    entry = root.openNextFile();
  }

  bool found = false;
  memcpy(&fname, entry.name(), strlen(entry.name()) + 1);
  if (fname[strlen(fname) - 1] == 'N' && fname[strlen(fname) - 2] == 'I' && fname[strlen(fname) - 3] == 'B' && fname[strlen(fname) - 4] == '.') {
    found = true;

  }
  entry.close();
  return found;
}

void find_new_animation() {
  int i = 0;
  while(!selectFile()) {
    if (i++ == 100) {
      //This is ugly. It's here so it tries a few times before giving up. 
      break;
    }
  }
  readFile.close();
  readFile = SD.open(fname);
  bytesRead = 0;
  animationInit(readFile);
}

void button_click() 
{
 if (millis() - last_interrupt > 250) {
  last_interrupt = millis();
  change_animation = true;
 }
}


void readNextStep(File *file) 
{
  uint16_t numColors;
  file->read(&numColors, 2);
  bytesRead += 2;
  for (int i = 0; i < numColors; i++) 
  { 
    CRGB color;
    uint16_t num_leds;
    file->read(&color, 3);
    bytesRead+= 3;
    file->read(&num_leds, 2);
    bytesRead += 2;
    uint16_t led_idx;
    for (int j = 0; j < num_leds; j++) 
    {
      file->read(&led_idx, 2);
      bytesRead += 2;
      set_led(led_idx, CRGB(color.r / BRIGHTNESS_DIVIDER, color.g / BRIGHTNESS_DIVIDER, color.b / BRIGHTNESS_DIVIDER));
      
    }
  }
}

void animationInit(File dataFile) {
  if (dataFile) 
  {
    if (dataFile.available()) 
    {
      dataFile.read(&stepsInAnimation, 1);
      bytesRead++;
      readNextStep(&dataFile);
    }
  }
}

void led_clear() 
{
  for (int i = 0; i < NUM_LEDS; i++) 
  {
    leds[i] = CRGB(0, 0, 0);
  }
}

/* This function is here to fix the issue with led strips
 *  going in a snake pattern, and not all starting on the same side,
 *  as well as fixing that the "top" of the led matrix, is the 
 *  bottom when it was soldered. Basically flipping every second
 *  row horizontally, and flipping the entire thing vertically.
 */
void set_led(uint16_t i, CRGB color) {
  uint8_t x;
  uint8_t y;
  y = (ROWS - 1) - (i / LEDS_PER_ROW);
  x = i % LEDS_PER_ROW;  
  if (y % 2 == 0)
    leds[y * LEDS_PER_ROW + x] = color;
  else
    leds[y * LEDS_PER_ROW + (LEDS_PER_ROW - 1) - x] = color;
}

void setup() 
{
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  leds[0] = CRGB(255, 255, 255);

  while (!SD.begin(CHIP_SELECT)) 
  {
    delay(1000);
  }
  
  root = SD.open("/");
  find_new_animation();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button_click, FALLING);
}

void readNext(File f) 
{
  //Start from beginning
  if (currentStep >= stepsInAnimation) 
  {
    f.seek(1);
    currentStep = 1;
    bytesRead = 1;
  }
  else 
  {
    currentStep++;
  }
  readNextStep(&f);
}

void loop() {
   if (change_animation) {
    find_new_animation();
    change_animation = false;
   }
   
   led_clear();  
   readNext(readFile);
   FastLED.show();
   delay(100);
}
