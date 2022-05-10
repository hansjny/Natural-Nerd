### Hexaleaf_Blynk 
This code should is created to work on an **ESP32** board and **WS2812B** based leds strips.
The color control should be done by **BLYNK** app.

Before uploading the sketch to your board you should change in ```nanohex.h``` the following parameters according to your creation (number of hexagons, leds per hexagon, ESP32 control pin):
```
/* Number of LEDs in each box/leaf */
#define LEDS_IN_BOX 18
/*The number of boxes */
#define NUM_BOXES 7
/*The pin the LED is connected to */
#define LED_PIN 27
```
Additionaly the following lines should be edited in ```Hexaleaf4.ino``` in order to get connected with your WiFi and communicate it with BLYNK app.

```
const char* ssid = "Your_SSID"; // The SSID (name) of the Wi-Fi network you want to connect to
const char* pass = "Your_Password"; // The password of the Wi-Fi network
const char* auth = "BlynkAuthenticationKey"; //The Blynk Authorization Token
```
The Hexagon lights 3d model based can be found on [Thingverse](https://www.thingiverse.com/thing:4615531). This model is based on Modern Hobbyist's project with reference to Nerd-Forge. For more info go to [Modern Hobbyist's video](https://www.youtube.com/watch?v=ERK9_q242q4) on youtube.

Original code by Nerd-Forge [Hexaleaf Aurora](https://github.com/hansjny/Natural-Nerd/tree/master/Hexaleaf)


_Archived folder contains all original codes from Nerd-Forge (which contains other projects as well), just for reference _

