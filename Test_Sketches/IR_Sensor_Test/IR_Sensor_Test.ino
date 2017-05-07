/**************************************************************************
 *  IR_Sensor_Test
 *  Modified from Adafruit Libary Example (license below)
 *  Simplified and sane output in CSV format
 *  Modified: J.R. Leeman
 *  
 *  This is a library example for the MLX90614 Temp Sensor
 *  
 *  Designed specifically to work with the MLX90614 sensors in the
 *  adafruit shop
 *  ----> https://www.adafruit.com/products/1748
 *  ----> https://www.adafruit.com/products/1749
 *  
 *  These sensors use I2C to communicate, 2 pins are required to  
 *  interface
 *  Adafruit invests time and resources providing this open source code, 
 *  please support Adafruit and open-source hardware by purchasing 
 *  products from Adafruit!
 *  
 *  Written by Limor Fried/Ladyada for Adafruit Industries.  
 *  BSD license, all text above must be included in any redistribution
 **************************************************************************/

#include <Wire.h>
#include <Adafruit_MLX90614.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {
  Serial.begin(115200);
  Serial.println("AmbientTempC, ObjectTempC");
  mlx.begin();  
}

void loop() {
  Serial.print(mlx.readAmbientTempC()); 
  Serial.print(", ");
  Serial.println(mlx.readObjectTempC());
  delay(200);
}
