/******************************************************************************
 * GPS_Text.ino
 * Simplified example from the Tiny GPS ++ Library that
 * produces clean CSV output. Modified J.R. Leeman
******************************************************************************/

#include <TinyGPS++.h>
//#include <SoftwareSerial.h> // Uncomment for software serial

//static const int RXPin = 4, TXPin = 3; // Uncomment for software serial
static const uint32_t GPSBaud = 4800;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
//SoftwareSerial ss(RXPin, TXPin); // Uncomment for software serial

void setup()
{
  Serial.begin(115200);
  Serial1.begin(GPSBaud);
  //ss.begin(GPSBaud); // Uncomment for software serial
  Serial.println("Lat, Lon");
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (Serial1.available() > 0) {
    if (gps.encode(Serial1.read())) {
      Serial.print(gps.location.lat(), 6);
      Serial.print(F(","));
      Serial.println(gps.location.lng(), 6);
    }
      
    if (millis() > 5000 && gps.charsProcessed() < 10) {
      Serial.println(F("No GPS detected: check wiring."));
      while(true);
    }
  }
}
