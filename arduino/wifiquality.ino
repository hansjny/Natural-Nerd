#include <ESP8266WiFi.h>
#include <FastLED.h>

#define DATA_PIN 2
#define NUM_LEDS 20

const char* password = "pass";
const char* ssid = "ssid";

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
	pinMode(DATA_PIN, OUTPUT);

	WiFi.begin(ssid, password);
	connection_wait();	

	Serial.println();
	Serial.print("Connected, IP address: ");
	Serial.println(WiFi.localIP());
}

void loop() {
	while(WiFi.status() == WL_CONNECTED) {
		// Get RSSI and keep it within -100 and -50
		int rssi = constrain(WiFi.RSSI(), -100, -50);
		/*Map RSSI to quality according to 	
		https://stackoverflow.com/questions/15797920/how-to-convert-wifi-signal-strength-from-quality-percent-to-rssi-dbm */
		int quality = map(rssi, -100, -50, 0, 100);
		int light_modifier = map(constrain(quality, 0, 90), 0, 90, 0, 255);
		set_uniform_color(CRGB(255 - light_modifier, 0 + light_modifier, 5));
		Serial.print("Quality: ");
		Serial.print(quality);
		Serial.print(" % \n");
		
		delay(100);

	} 	
	connection_wait();	
}

void set_uniform_color(CRGB color) {
	for (int i = 0; i < NUM_LEDS; i++) {
		leds[i] = color;
	}
	FastLED.show();
}

void connection_wait() {
	while(WiFi.status() != WL_CONNECTED) {
		Serial.println("Waiting for connection... \n");
		light_pulse();
		delay(100);
	}
}

void light_pulse() {

	for (int i = 0; i < 255; i++) {
		set_uniform_color(CRGB(0, 0, i));
		delay(9);

	}
	delay(200);	
	for (int i = 255; i > 0; i--) {
		set_uniform_color(CRGB(0, 0, i));
		delay(4);
	}

	delay(200);	
}
