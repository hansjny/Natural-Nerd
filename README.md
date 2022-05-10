## Hexaleaf Blynk 
### Introduction
This code created to work on an **ESP32** board and **WS2812B** based leds strips.
The color control should be done by **BLYNK** app.
### Requirements
This is an Arduino IDE based code. You you can upload this sketch using Arduino software, PlatformIO, or Visual Studio with VisualMicro plugin.<p>
ESP32 board support should be added prior compiling and uploading and also and FastLED library is required.
  
### Code Improvements
Following the guidence on BLYNK documentation changed the code on ```.ino``` by adding a timer to update the hexController.
The following lines of code were added:
  ```
  //Declaration
BlynkTimer timerUpdate; // Creating a blynk timer

//Timer setup in Setup
   timerUpdate.setInterval(10L, updateX); //Setup the timer to run every 10 millisec 
   
//Update function
void updateX() // This function is called by the blynk timer
{
 hexController->update();
}

//Timer execution in Loop
timerUpdate.run();   // Fire the timer to update hexController
  ```
  
### Customization
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
### BLYNK configuration
Download Blynk App: [Android](http://j.mp/blynk_Android) or [iOS](http://j.mp/blynk_iOS) <p>
Configuration can be found on [Blynk Config folder](https://github.com/limbo666/Hexaleaf_Blynk/tree/master/Blynk%20Config). Follow the 7 provided images to create a similar BLYNK layout.
  
  
### Resourses
The Hexagon lights 3d model based can be found on [Thingverse](https://www.thingiverse.com/thing:4615531). This model is based on Modern Hobbyist's project with reference to Nerd-Forge. For more info go to [Modern Hobbyist's video](https://www.youtube.com/watch?v=ERK9_q242q4) on youtube.

Original code by Nerd-Forge [Hexaleaf Aurora](https://github.com/hansjny/Natural-Nerd/tree/master/Hexaleaf)

### Remarks
_Archived folder contains all original codes from Nerd-Forge (which contains other projects as well), just for reference _

