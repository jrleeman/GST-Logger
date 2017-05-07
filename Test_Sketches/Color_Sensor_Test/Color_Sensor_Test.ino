/******************************************************************************
 * Color_Sensor_Text.ino
 * Simplified example from the SparkFun ISL29125 RGB Sensor Library that
 * produces clean CSV output. Originally distributed as beerware by
 * Jordan McConnell at Sparkfun. Modified J.R. Leeman
******************************************************************************/

#include <Wire.h>
#include "SparkFunISL29125.h"

SFE_ISL29125 RGB_sensor;

void setup()
{
  Serial.begin(115200);
  if (RGB_sensor.init())
  {
    Serial.println("Red, Green, Blue");
  }
}

// Read sensor values for each color and print them to serial monitor
void loop()
{
  // Read sensor values (16 bit integers)
  unsigned int red = RGB_sensor.readRed();
  unsigned int green = RGB_sensor.readGreen();
  unsigned int blue = RGB_sensor.readBlue();

  // Print out to Serial
  Serial.print(red,DEC);
  Serial.print(", ");
  Serial.print(green,DEC);
  Serial.print(", ");
  Serial.println(blue,DEC);
  delay(200);
}
