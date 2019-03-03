#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "Nanohex.h"

/* SETUP */
// Remember to setup in Nanohex.h as well. 

const char* ssid = ""; // The SSID (name) of the Wi-Fi network you want to connect to
const char* pass = ""; // The password of the Wi-Fi network
const char* auth = ""; //The Blynk Authorization Token

Nanohex *hexController;

void setup() {
  Serial.begin(9600);
  Serial.println("> Setup.");
  hexController = new Nanohex();
  Blynk.begin(auth, ssid, pass);
}

/* Get the main mode of operation */
BLYNK_WRITE(V1)
{
  uint8_t pinValue = param.asInt(); // Mode
  Serial.printf("Received mode %d \n", pinValue);
  hexController->set_mode(pinValue);
}

/* Primary color */
BLYNK_WRITE(V2)  
{
    int r = param[0].asInt();
    int g = param[1].asInt();
    int b = param[2].asInt();
    hexController->set_primary(CRGB(r,g,b));
    Serial.printf("Primary: (%d, %d, %d) \n", r, g, b);
}

/* Secondary color */
BLYNK_WRITE(V3)  
{
    int r = param[0].asInt();
    int g = param[1].asInt();
    int b = param[2].asInt();
    hexController->set_secondary(CRGB(r,g,b));
    Serial.printf("Secondary: (%d, %d, %d) \n", r, g, b);
}

/* Get the min animation time */
BLYNK_WRITE(V4)
{
  uint16_t pinValue = param.asInt();
  hexController->set_fadetime_min(pinValue * 10);
}

/* Get the animation time */
BLYNK_WRITE(V5)
{
  uint16_t pinValue = param.asInt();
  hexController->set_fadetime_max(pinValue * 10);
}

/* Hue randomness */
BLYNK_WRITE(V6)
{
  uint8_t pinValue = param.asInt(); 
  hexController->set_hue_randomness(pinValue);
}

long ctr = 0;
void loop() {
    Blynk.run();
    hexController->update();

}




























