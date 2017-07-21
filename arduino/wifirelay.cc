#include <ESP8266WiFi.h>
#define RELAY_PIN D1
const char* password = "wifipassword";
const char* ssid = "wifiname";
const int port = 8181;

WiFiServer server(port);

void setup() {
	pinMode(RELAY_PIN, OUTPUT);
	Serial.begin(9600);
	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println();
	Serial.print("Connected, IP address: ");
	Serial.println(WiFi.localIP());

	server.begin();
}

void loop() {
	WiFiClient client = server.available();
	while(client.connected()) {
		if (client.available()) {
			uint8_t buf;
			size_t length = 1;
			client.read(&buf, length);
			client.write(handleCmd(buf));
		}
	}
}

char* handleCmd(uint8_t cmd) {
	Serial.println(cmd);
	switch(cmd) {
		case 49:
				digitalWrite(RELAY_PIN, HIGH);
				return "Relay pin set to high\n";
		case 48:
				digitalWrite(RELAY_PIN, LOW);
				return "Relay pin set to low\n";
		default:
				return "Send ASCII 1 for on, and 0 for off.\n";
	}
}
