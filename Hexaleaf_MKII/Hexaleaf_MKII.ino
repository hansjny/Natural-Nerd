// Edit the following constants with your actual values on nanohex.h file  NOLEDS_IN_BOX ,  NUM_BOXES , LED_PIN 

#include <WiFi.h>
#include <WiFiUdp.h>
#include "Nanohex.h"

#include <time.h>

const char* ver = "1.0";
const char* ssid = "your_ssid";
const char* password = "your_password";


// NTP
const char *ntpServerName = "pool.ntp.org";
const int  ntpPort = 123;
byte packetBuffer[48];
WiFiUDP ntpUDP;
// Define the times when the output should turn on and off
String onTime = "09:30";
String offTime = "20:15";
// Converts a UNIX timestamp to a formatted string
String formatTime(time_t t) {
     char buffer[16];
     strftime(buffer, 16, "%H:%M:%S", localtime(&t));
     return String(buffer);
}
int ntpTimes = 0;
// Sends an NTP request to the specified NTP server and returns the UNIX timestamp
time_t getNtpTime() {
     IPAddress timeServerIP;
     WiFi.hostByName(ntpServerName, timeServerIP);
     memset(packetBuffer, 0, 48);
     packetBuffer[0] = 0b11100011;
     packetBuffer[1] = 0;
     packetBuffer[2] = 6;
     packetBuffer[3] = 0xEC;
     packetBuffer[12] = 49;
     packetBuffer[13] = 0x4E;
     packetBuffer[14] = 49;
     packetBuffer[15] = 52;
     ntpUDP.beginPacket(timeServerIP, 123);
     ntpUDP.write(packetBuffer, 48);
     ntpUDP.endPacket();
     int cb = ntpUDP.parsePacket();
     if (cb == 0) {
          return 0;
     }
     ntpUDP.read(packetBuffer, 48);
     unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
     unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
     unsigned long secsSince1900 = highWord << 16 | lowWord;
     const unsigned long seventyYears = 2208988800UL;
     time_t t = secsSince1900 - seventyYears;
     return t;
}
// END -NTP


WiFiUDP Udp;
unsigned int localUdpPort = 4578;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char *parts[10];

bool broadcastIP = true;
int cycles = 0;
uint8_t noOfBroadcasts;

uint8_t modeNo;
uint16_t minAniTime;
uint16_t maxAniTime;
uint8_t hueRand;

int primR, primB, primG, secR, secG, secB;

Nanohex *hexController;
// Arduino setup
void setup() {
     Serial.begin(115200);
     hexController = new Nanohex();
     WiFi.begin(ssid, password);
     while (WiFi.status() != WL_CONNECTED) {
          delay(1000);
          Serial.println("Connecting to WiFi...");
     }
     Serial.println("Connected to WiFi");
     Serial.println(WiFi.localIP());
     Udp.begin(localUdpPort);
     Serial.println("Please wait");
     if (broadcastIP) {
          broadcastIPAddress();
     }
     delay(500);

     // Start the UDP communication for NTP
     ntpUDP.begin(ntpPort);
     // Get the current time from the NTP server
     time_t t = getNtpTime();
     // Convert the UNIX timestamp to a formatted string
     String time = formatTime(t);
     // Print the time
     Serial.println("Current time: " + time);

     // Set default mode here:
     modeNo = 3;
     hueRand = 70;
     SelectMode();
     SetHueRandomness();
     Serial.println("The system is up and running");
}

void compareTimes()
{

     // Get the current time from the NTP server every minute
     // delay(60000);
     time_t t = getNtpTime();
     String time = formatTime(t);

     //time.substring(0,5) == onTime

     // Compare the current time to the on and off times
     if (time.substring(0, 5) == onTime) {
          //  digitalWrite(outputPin, HIGH);
          Serial.println("Mode on");

          // Set on mode here:
          modeNo = 2;
          primR = 150;
          primG = 60;
          primB = 0;
          secR = 0;
          secG = 100;
          secB = 180;
          minAniTime = 10;
          maxAniTime =  60;
          hueRand =  120;

          SelectMode( );
          SetPrimaryColor( );
          SetSecondaryColor( ) ;
          SetMinAnimationTime( );
          SetMaxAnimationTime( );
          SetHueRandomness( ) ;


     } else if (time.substring(0, 5) == offTime) {
          // digitalWrite(outputPin, LOW);
          Serial.println("Mode off");

          // Set off mode
          modeNo = 3;
          primR = 0;
          primG = 0;
          primB = 35;
          secR = 0;
          secG = 0;
          secB = 0;
          minAniTime = 5;
          maxAniTime =  60;
          hueRand =  10;

          SelectMode( );
          SetPrimaryColor( );
          SetSecondaryColor( ) ;
          SetMinAnimationTime( );
          SetMaxAnimationTime( );
          SetHueRandomness( ) ;

     } else {
          Serial.println("Got time: " + time.substring(0, 5));
          Serial.println("Waiting for " + onTime + " or " + offTime);
     }

}



void broadcastIPAddress() {
     IPAddress ip = WiFi.localIP();
     // Create a buffer to store the IP address
     char ipStr[20];
     snprintf(ipStr, sizeof(ipStr), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
     // Create a buffer to store the message
     char message[80];
     snprintf(message, sizeof(message), "HEXALEAF is up and running. Port 4578, IP:%s", ipStr);
     // Convert char* to uint8_t*
     uint8_t *data = reinterpret_cast<uint8_t *>(message);
     // Broadcast the message
     Udp.beginPacket("255.255.255.255", 4579);
     Udp.write( data, sizeof(message));
     Udp.endPacket();
}



void updateX() // This function is called to keep update the effects
{
     hexController->update();
}

void SelectMode( ) /* Get the main mode of operation */
{
     Serial.printf("Mode : %d\n", modeNo);
     hexController->set_mode(modeNo);
}

void SetPrimaryColor( ) /* Primary color */
{
     Serial.printf("Primary: ");
     Serial.printf("Red:%d ", primR);
     Serial.printf("Green:%d ", primG);
     Serial.printf("Blue:%d\n", primB);
     hexController->set_primary(CRGB(primR, primG, primB));
}


void SetSecondaryColor( ) /* Secondary color */
{
     Serial.printf("Secondary: ");
     Serial.printf("Red:%d ", secR);
     Serial.printf("Green:%d ", secG);
     Serial.printf("Blue:%d\n", secB);
     hexController->set_secondary(CRGB(secR, secG, secB));
}


void SetMinAnimationTime( ) /* Get the min animation time */
{
     Serial.printf("Min fade: %d\n", minAniTime);
     hexController->set_fadetime_min(minAniTime * 10);
}

void SetMaxAnimationTime( ) /* Get the min animation time */
{

     Serial.printf("Max fade: %d\n", maxAniTime);
     hexController->set_fadetime_max(maxAniTime * 10);

}

void SetHueRandomness( ) /* Hue randomness */
{
     Serial.printf("Hue randomness: %d\n", hueRand);
     hexController->set_hue_randomness(hueRand);
}

// Arduino loop
void loop() {
     int packetSize = Udp.parsePacket();
     if (packetSize) {
          // read the packet into packetBufffer
          int len = Udp.read(incomingPacket, 255);
          if (len > 0) {
               incomingPacket[len] = 0;
          }
          if (incomingPacket[0] == '$') {
               char * pch;
               pch = strtok (incomingPacket, ":, ");
               int i = 0;
               while (pch != NULL)
               {
                    parts[i] = pch;
                    i++;
                    pch = strtok (NULL, ":, ");
               }
               for (int j = 1; j < i; j++) {
                    char var[3];
                    var[0] = parts[j][0];
                    var[1] = parts[j][1];
                    var[2] = '\0';
                    char val[10];
                    for (int k = 2; k < strlen(parts[j]); k++) {
                         val[k - 2] = parts[j][k];
                    }
                    val[strlen(parts[j]) - 2] = '\0';
                    //     Serial.print("Var: ");
                    //      Serial.print(var);
                    //      Serial.print(" = ");
                    //    Serial.println(val);
                    if (strcmp(var, "V1") == 0) {
                         modeNo = (uint8_t) atoi(val);  // convert value to uint8_t and store in modeNo variable
                         SelectMode();
                    }
                    if (strcmp(var, "V2") == 0) {

                         char r[4], g[4], b[4];

                         for (int i = 0; i < 3; i++) {
                              r[i] = val[i];
                         }
                         primR = atoi(r);

                         for (int i = 0; i < 3; i++) {
                              g[i] = val[i + 3];
                         }
                         primB = atoi(g);

                         for (int i = 0; i < 3; i++) {
                              b[i] = val[i + 6];
                         }
                         primG = atoi(b);
                         SetPrimaryColor();
                    }
                    if (strcmp(var, "V3") == 0) {

                         char r[4], g[4], b[4];

                         for (int i = 0; i < 3; i++) {
                              r[i] = val[i];
                         }
                         secR = atoi(r);

                         for (int i = 0; i < 3; i++) {
                              g[i] = val[i + 3];
                         }
                         secB = atoi(g);

                         for (int i = 0; i < 3; i++) {
                              b[i] = val[i + 6];
                         }
                         secG = atoi(b);

                         SetSecondaryColor();
                    }

                    if (strcmp(var, "V4") == 0) {
                         char* endPtr;
                         minAniTime = (uint16_t) strtol(val, &endPtr, 10);  // convert value to uint16_t and store in minAniTime variable
                         SetMinAnimationTime();
                    }
                    if (strcmp(var, "V5") == 0) {
                         char* endPtr;
                         maxAniTime = (uint16_t) strtol(val, &endPtr, 10);  // convert value to uint16_t and store in maxAniTime variable
                         SetMaxAnimationTime();
                    }
                    if (strcmp(var, "V6") == 0) {
                         hueRand = (uint8_t) atoi(val);  // convert value to uint8_t and store in modeNo variable
                         SetHueRandomness();
                    }

                    if (strcmp(var, "SM") == 0) {
                         String checkVal = String(val);
                         if (checkVal == "IP" ) {
                              Serial.println("My IP sent to port 4579");
                              broadcastIPAddress() ;
                         }
                         if (checkVal == "STOP" ) {
                              Serial.println("IP broadcast STOP");
                              broadcastIP = false;
                         }
                         else if (checkVal == "MEM" ) {
                              Serial.println("Mem heap sent to port 4579");
                              // To be added

                         }
                         else if (checkVal == "VER" ) {
                              Serial.println("Version sent to port 4579");
                              // To be added
                         }
                    }
               }
          }
     }
     delay(2);
     updateX();
     // use cycles below to periodicaly run some functions
     if (cycles < 3000) {
          ++cycles ;
     }
     else
     {
          cycles = 0;
          if (noOfBroadcasts < 50) { // Broadcast IP address 50 times after power up
               ++noOfBroadcasts;
          }
          else
          {
               noOfBroadcasts = 0;
               broadcastIP = false; // Stop broadcasting
          }
          //  Serial.println("fire it");
          if (broadcastIP) {
               broadcastIPAddress();
          }
     }

     if (ntpTimes < 15000 ) {
          ++ntpTimes ;

     }

     else {

          ntpTimes = 0;
          compareTimes();


     }



}
