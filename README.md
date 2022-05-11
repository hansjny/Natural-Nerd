## Hexaleaf Blynk 
![Hexaleaf blynk](https://raw.githubusercontent.com/limbo666/Hexaleaf_Blynk/master/Pics/hexagon.png?raw=true)
### Introduction
This is an Arduino sketch. This code created to work on an **ESP32** board and **WS2812B** based leds strips.
The control should be done using **BLYNK** app.

### Requirements
This is an Arduino IDE based code. You you can compile and upload this sketch using Arduino IDE, PlatformIO, or Visual Studio with VisualMicro plugin.<p>
ESP32 board support should be added to your IDE prior compiling and uploading and also and FastLED library is required.
  
### Code improvements to original
With the original sketch the device was disconnected from blynk server and ESP32 was entwring to an eternal color loop.
Following the guidence on BLYNK documentation, I've changed the code on ```hexaleaf4.ino``` by adding a timer to update the hexController, instead of calling the routine directly on ```loop()``` function.
This is enough to stabilize the code. 

The following lines of code were added:
  ```
  //Declaration
BlynkTimer timerUpdate; // Creating a blynk timer

//Timer setup in Setup
   timerUpdate.setInterval(10L, updateX); //Setup the timer to run every 10 millisec (quicker time may be ok too) 
   
//Update function
void updateX() // This function is called by the blynk timer
{
 hexController->update(); // This line moved here from loop() this is actuall update command
}

//Timer execution in Loop
timerUpdate.run();   // Fire the timer to update hexController
  ```
  
### Code customization before use
Before uploading the sketch to your board you should change the following parameters in ```nanohex.h``` to your creation (number of hexagons, leds per hexagon, ESP32 control pin):
```
/* Number of LEDs in each box/leaf */
#define LEDS_IN_BOX 18
/*The number of boxes */
#define NUM_BOXES 7
/*The pin the LED is connected to */
#define LED_PIN 27
```
Additionaly, the following lines should be edited in ```Hexaleaf4.ino``` to connect the device with your WiFi and communicate with BLYNK app.
```
const char* ssid = "Your_SSID"; // The SSID (name) of the Wi-Fi network you want to connect to
const char* pass = "Your_Password"; // The password of the Wi-Fi network
const char* auth = "BlynkAuthenticationKey"; //The Blynk Authorization Token
```
### BLYNK configuration
Download Blynk App: [Android](http://j.mp/blynk_Android) or [iOS](http://j.mp/blynk_iOS) <p>
Blynk configuration steps can be found on [Blynk Config folder](https://github.com/limbo666/Hexaleaf_Blynk/tree/master/Blynk%20Config). Follow the 7 provided images to create a similar BLYNK layout keeping the control values as illustrated.
  
  
### Resourses
The Hexagon lights 3D model can be found on [Thingverse](https://www.thingiverse.com/thing:4615531). This model is based on Modern Hobbyist's project with reference to Nerd-Forge. For more info go to [Modern Hobbyist's video](https://www.youtube.com/watch?v=ERK9_q242q4) on youtube. This spcific 3D model is improved to be stronger and fix the cover snap dimension. 
  
Finally, the most detailed video presentation on Youtbe is from ZDeanzo:<p>
  [![ZDeanzo video](https://img.youtube.com/vi/nUz_oQ4jfH8/0.jpg)](https://www.youtube.com/watch?v=nUz_oQ4jfH8)
<p>
  I highly suggest to view his video. He is eplaining in detail the making proccess.

Original code by Nerd-Forge [Hexaleaf Aurora](https://github.com/hansjny/Natural-Nerd/tree/master/Hexaleaf)
  

### Remarks
_"Archived" folder contains all original source codes from Nerd-Forge's Repository (which contains other projects as well not related to hexaleaf), just for reference_

