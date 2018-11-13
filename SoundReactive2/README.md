### Brief explanation


#### led_master.ino is for the controller with the microphone module. Needs to specify:
* Button pin to change mode
* Microphone pin to read audio levels
* How many lamps will connect

#### led_slave.ino is for the lamps themselves. Needs to specify:
* The pin the LED strip is connected to
* The number of diodes on the strip
* Which unique ID the current lamp has. Starts from 1 to n, where n is the amount of lamps. Wont work unless this is correct, and the same as the amount of lamps the controller expects.
