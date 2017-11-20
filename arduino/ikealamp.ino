#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <FastLED.h>

#define BLYNK_PRINT Serial
#define NUM_LEDS 26

char* token = "blynktoken";
char* ssid = "wifisdid";
char* pw = "wifipassword";

CRGB leds[NUM_LEDS];
CRGB color = CRGB(255, 255, 255);

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, 2>(leds, NUM_LEDS);
  for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(255, 255, 255);
  }
  FastLED.show();
  Blynk.begin(token, ssid, pw);

}

void updateColors() {
  for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = color;
  }
  FastLED.show();
}

BLYNK_WRITE(V0) {
  color.r = param.asInt(); 
  updateColors();
}

BLYNK_WRITE(V1) {
   color.g = param.asInt();
   updateColors(); 
}

BLYNK_WRITE(V2) {
   color.b = param.asInt(); 
   updateColors();
}

void Blynk_Delay(int milli){
   int end_time = millis() + milli;
   while(millis() < end_time){
       Blynk.run();
   }
}

void loop() {
    Blynk.run();
}


