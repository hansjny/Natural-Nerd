### LED Facemask writeup
Written by [bennyd93](https://github.com/bennyd93)

I have successfully completed the wiring and done a more thorough diagram, so that anyone can see it. In my project I took power and ground for the LEDs off of the 5v and gnd pins as i was only using 256 Leds (8x32 LEDS)
**Note: you will have to change the variables in the ardiuno code and face mask webapp code if you use a different amount to the ones in the video** 

Power for the project comes in via inbuilt usb port

**Momentary Button** (Pins should be used on the same side)
Button pin 1 = Arduino Pin GND (Black wire in diagram)
Button pin 2 = Arduino Pin 2 (Green wire in diagram)

**SD Card Pins** (you must use all connecters)
CS = Arduino Pin SS (Purple wire in diagram)
SCK = Arduino Pin SCK (Yellow wire in diagram)
MOSI = Arduino Pin MOSI (Brown wire in diagram)
MISO = Arduino Pin MISO (Light Blue wire in diagram)
VCC = Arduino Pin 5V (Red wire in diagram)
GND = Arduino Pin GND (Black wire in diagram)

**LED Panel Pins**
5v = Arduino Pin 5v (Red wire in diagram)
GND = Arduino Pin GND (Black wire in diagram)
DATA = Arduino Pin DATA (Green wire in diagram)


### Parts List

Arduino Micro
https://www.amazon.co.uk/gp/product/B019SXN8QM/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1 

SD Card Reader 
https://www.amazon.co.uk/gp/product/B07SBR9NL1/ref=ppx_yo_dt_b_asin_title_o05_s00?ie=UTF8&psc=1

LED Panel (note I used this as to save time soldering and for ease)
https://www.amazon.co.uk/gp/product/B07KT1H481/ref=ppx_yo_dt_b_asin_title_o02_s00?ie=UTF8&psc=1

To bring all this together I did use some protoype board so it was more permanant 
https://www.amazon.co.uk/gp/product/B082KYCJXJ/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1

Power Bank (Like the video)
Button & wires I had from previous project 

Hope this helps you and anyone else wanting to do this project :)

![Facemask with button](https://cdn.discordapp.com/attachments/564756333841350656/771074475054858250/unknown.png)



