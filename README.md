## This is the altered and updated version of Hexaleaf project
This project is made partially using the original ESP32 code but **breaking free from Blynk**. 
The control of the hexagons is switched to LAN based using a custom made windows program and with plans to add as much functionality is possible to make it work from any device.

Communication protocol is changed but the functions are the same with some additional stuff added. 

<img src="https://img.shields.io/badge/-ProjectForMakers-yellowgreen"> <img src="https://img.shields.io/badge/-Difficulty:EASY-purple"> 
## Hexaleaf Blynk
![enter image description here](https://github.com/limbo666/Hexaleaf_Blynk/blob/master_Hexaleaf_MKII/Pics/Hexaleaf_Hexagons.png?raw=true)


### Introduction
This is an Arduino sketch but with some additional files for controlling the Hexaleaf. The Arduino code created to work on an **ESP32** board with  **WS2812B** based leds strips.
The control should be done over LAN using the provided windows application or any other app to pass text commands over UDP communication scheme. 

### Block Diagram

<img src="https://raw.githubusercontent.com/limbo666/Hexaleaf_Blynk/master/Pics/Block%20Diagram.jpg" width="800" height="350">
<p> The electronic circuitry is quite simple for the device. you can use any ESP32 based board with WS8212 ledstrip (also known as Neopixel).
Be aware that each LED on the strip can draw up to 60mA of current when all three colors are ON (White color in full illumination). You must select a power source capable to cover the current needs of your hexaleaf.
<p>In one of my creations I have used 18 LEDs on each hexagon in 7 hexagons in total. The total current needed was above 7A (18*7*60 = 7560mA) in this case and I was forced to buy a special 5V power supply (a 5V 8A power brick).  


### Programming
The Arduino code for ESP32, can be compiled and uploaded to the board by using Arduino IDE, PlatformIO or Visual Studio with VisualMicro plugin.<p>
Support ESP32 based boards should be added to your IDE by using boards manager prior compiling and uploading. Also and FastLED and WiFiUdp libraries are needed. You must to add these libraries using the  library manager.
  
### Code changes 
The device is changes to accept text strings over LAN in UDP communication scheme in order to be controlled.
This can be done easily by using the provided controller for windows. Or by creating you own controller and send simple UDP commands on port 4578.
The device waits for a text string Starting with `$,` followed by six different parts to get the parameters V1, to V6.
V1 represents mode and accepts values of 1, 2 or 3  `V13`. Means mode 3
V2 represents the primary color and should be followed by three groups of three digit color values. `V2000250000` is Green 
V3  represents the primary color and should be followed by three groups of three digit color values. `V3000000250` is Blue
V4 is the minimum change time in milliseconds. So `V410000` is 10 seconds
V5 is the maximum change time in milliseconds. So `V4127000` is 127 seconds
V6 is the hue randomizer value and accepts values from 1 to 254. `V693` is a hue randomizer value of 93
  
The entire command structure is presented below `$,V1[Mode],V2[Primary_Color],V3[Secondary_Color],V4[Min_Time],V5[Max_Time],V6[Hue_Random]`

All the above mentioned values should be send in a string line splitted by commas.
Example string commands:
All the the above mentioned values could be combined in one command like the following:  `$,V13,V2000250000,V3000000250,V410000,V4127000,V693`
The board is able to recognize and change any value of them, in any order. So the strings `$,V13,V2000250000,V3000000250` , `$,V11,V2050250100` , `$,V12,V2050250100,V3000000250,V520000` and `$,V625,V12,V3100100100` are also valid.
 
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
Additionally, the following lines should be edited in ```Hexaleaf_MKII.ino``` to connect the device with your WiFi and communicate with BLYNK app.
```
const char* ssid = "your_ssid";
const char* password = "your_password";
```

  
### Recourses
The Hexagon lights 3D model can be found on [Thingverse](https://www.thingiverse.com/thing:4615531). The 3D model is a remix on Modern Hobbyist's creation with reference to Nerd-Forge's electronics and code. For more info go to [Modern Hobbyist's video](https://www.youtube.com/watch?v=ERK9_q242q4) on youtube. This spcific 3D model is improved to be stronger and fix the cover snap dimension error. 
  
Finally, another detailed video presentation on Youtbe is from ZDeanzo (click image to open video):<p>

Original effects code taken by Nerd-Forge [Hexaleaf Aurora](https://github.com/hansjny/Natural-Nerd/tree/master/Hexaleaf) project.
 

### Remarks
Although Blynk was a well designed and flexible platform their developers decided to move on with Blynk IoT which is great as well. From my point of view, this switch was not I was looking for. Although the pricing for their new app and service seems to be reasonable, I don't want to force others pay to enjoy making, so I decide to switch to LAN based control (with future plans to add more functionalities) for my own Hexaleaf.
