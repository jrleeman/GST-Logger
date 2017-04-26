/*
* SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
*/

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <SparkFunISL29125.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

const int chipSelect = 4;
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 4800;

unsigned int color_red
unsigned int color_green
unsigned int color_blue

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
SFE_ISL29125 RGB_sensor;
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);
  // Setup SD Card
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  // Setup IR Sensor
  mlx.begin(); 
  
  // Setup Color Sensor
  if (RGB_sensor.init())
  {
    Serial.println("Sensor Initialization Successful\n\r");
  }

}

void loop() {
  while (ss.available() > 0){
    if (gps.encode(ss.read())){
      read_IR_sensor();
      read_color_sensor();
      log_data();
    }
  }

}

void log_data() {

  // Format and write out the time stamp
  char sz[32];
  sprintf(sz, "%04d-%02d-%02dT%02d:%02d:%02d.%02d,",
  gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(),
  gps.time.minute(), gps.time.second(), gps.time.centisecond());
  logfile.write(sz);

  // Log latitude
  log_float(gps.location.lat(), -999.99, 10, 6);
  logfile.write(",");

  // Log longitude
  log_float(gps.location.lng(), -999.99, 10, 6);
  logfile.write(",");

  // Log altitude
  log_float(gps.altitude.meters(), -999.99, 10, 6);
  logfile.write(",");

  // Log speed
  log_float(gps.speed.kmph(), -999.99, 10, 6);
  logfile.write(",");

  // Log heading
  log_float(gps.course.deg(), -999.99, 10, 6);
  logfile.write(",");

  // Log number of satellites
  log_float(gps.satellites.value(), -999.99, 10, 6);
  logfile.write(",");
  
  // Log ambient temperature
  log_float(IR_ambient_temperature, -999.99, 10, 6);
  logfile.write(",");

  // Log object temperature
  log_float(IR_object_temperature, -999.99, 10, 6);
  logfile.write(",");

  // Log RGB values
  char sz[32];
  sprintf(sz, "%d,%d,%d,",
  color_red, color_green, color_blue);
  logfile.write(sz);
  
  
}

void read_IR_sensor() {
  // Get the ambient (air) temperature around the drone
  IR_ambient_temperature = mlx.readAmbientTempC();
  
  // Get the temperature the IR sensor sees
  IR_object_temperature = mlx.readObjectTempC();
}

void read_color_sensor(){
  color_red = RGB_sensor.readRed();
  color_green = RGB_sensor.readGreen();
  color_blue = RGB_sensor.readBlue();
}

static void log_float(float val, float invalid, int len, int prec) {
  /*
  Logs a float value
  */
  if (val == invalid)
  {
    while (len-- > 1)
      logfile.write('*');
  }
  else
  {
    logfile.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
  }
}


