/*
 * GST_SD_Logger.ino
 *
 * Log data from an IR temperature sensor and color sensor to and SD
 * Card with geotagged information.
 * 
 * Code Repo: https://github.com/jrleeman/GST-Logger
 * 
 * Necessary Libraries:
 * https://github.com/mikalhart/TinyGPSPlus
 * https://github.com/sparkfun/SparkFun_ISL29125_Breakout_Arduino_Library
 * https://github.com/adafruit/Adafruit-MLX90614-Library
 * 
*/

#include <SD.h>
//#include <SoftwareSerial.h> // Uncomment if you will use software serial (i.e. Uno)
#include <SPI.h>
#include <Wire.h>

#include <Adafruit_MLX90614.h>
#include <SparkFunISL29125.h>
#include <TinyGPS++.h>

// Variables and defines
#define chipSelect 16 // Change for your setup/board

// GPS connections
//static const int RXPin = 4, TXPin = 3; // Uncomment for software serial
static const uint32_t GPSBaud = 4800;

// Variable Declarations
unsigned int color_red;
unsigned int color_green;
unsigned int color_blue;
float IR_ambient_temperature;
float IR_object_temperature;
char filename[16];
int previous_second;

// Create object instances
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
SFE_ISL29125 RGB_sensor;
TinyGPSPlus gps;
File logfile;
//SoftwareSerial ss(RXPin, TXPin); // Uncomment for software serial 


void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // Set built-in LED to be output
  Serial.begin(115200); // Startup debug serial
  Serial1.begin(GPSBaud); // Startup GPS serial
  //ss.begin(GPSBaud); // Uncomment for software serial
  
  // Setup SD Card
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present... Halting");
    return;
  }
  Serial.println("SD Card Initialization Successful\n\r");

  // Setup IR Sensor
  if (mlx.begin())
  {
    Serial.println("IR Sensor Initialization Successful\n\r");
  }
   
  
  // Setup Color Sensor
  if (RGB_sensor.init())
  {
    Serial.println("Color Sensor Initialization Successful\n\r");
  }
  
  // Find the next available filename to log to
  // dataXXX.txt format
  int n = 0;
  snprintf(filename, sizeof(filename), "data%03d.txt", n);
  while(SD.exists(filename)) {
    n++;
    snprintf(filename, sizeof(filename), "data%03d.txt", n);
  }
  Serial.print("Filename: ");
  Serial.println(filename);

}

void loop() {
  // When we get a valid GPS message, read the sensors
  // and call the log function to write it all out
  while (Serial1.available() > 0){
    if (gps.encode(Serial1.read())){
      read_IR_sensor();
      read_color_sensor();

      if (gps.time.second() != previous_second){
        log_data();
        previous_second = gps.time.second();
      }
     
    }
  }
}

void log_data() {
  // Log the data out to the logfile
  
  digitalWrite(LED_BUILTIN, HIGH);
  
  logfile = SD.open(filename,FILE_WRITE);
  
  // Format the time stamp
  char ts[32];
  sprintf(ts, "%04d-%02d-%02dT%02d:%02d:%02d.%02d,",
  gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(),
  gps.time.minute(), gps.time.second(), gps.time.centisecond());

  // Format RGB values
  char rgb[32];
  sprintf(rgb, "%d,%d,%d", color_red, color_green, color_blue);

  if (logfile) {
    Serial.print("Writing data...");

    // Log timestamp
    logfile.write(ts);
    
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
    logfile.write(rgb);
    logfile.println("");
    Serial.println("Done");
  }
  logfile.close();
  
  digitalWrite(LED_BUILTIN, LOW);
}

void read_IR_sensor() {
  // Get the ambient (air) temperature around the drone
  IR_ambient_temperature = mlx.readAmbientTempC();
  
  // Get the temperature the IR sensor sees
  IR_object_temperature = mlx.readObjectTempC();
}

void read_color_sensor(){
  // Get the RGB sensor values
  color_red = RGB_sensor.readRed();
  color_green = RGB_sensor.readGreen();
  color_blue = RGB_sensor.readBlue();
}

static void log_float(float val, float invalid, int len, int prec) {
  // Logs a float value
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

