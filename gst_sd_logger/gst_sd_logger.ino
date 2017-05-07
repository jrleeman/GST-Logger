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
  
  
  int n = 0;
  snprintf(filename, sizeof(filename), "data%03d.txt", n); // includes a three-digit sequence number in the file name
  while(SD.exists(filename)) {
    n++;
    snprintf(filename, sizeof(filename), "data%03d.txt", n);
  }
  Serial.print("Filename: ");
  Serial.println(filename);

}

void loop() {
  while (Serial1.available() > 0){
    if (gps.encode(Serial1.read())){
      read_IR_sensor();
      read_color_sensor();
      log_data();
    }
  }

}

void log_data() {
  digitalWrite(LED_BUILTIN, HIGH);
  logfile = SD.open(filename,FILE_WRITE);
  
  // Format and write out the time stamp
  char sz[32];
  sprintf(sz, "%04d-%02d-%02dT%02d:%02d:%02d.%02d,",
  gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(),
  gps.time.minute(), gps.time.second(), gps.time.centisecond());

  if (logfile) {
    
    logfile.write(sz);
    Serial.print("Writing data...");
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
    
    char szz[32];
    sprintf(szz, "%d,%d,%d",
    color_red, color_green, color_blue);
    logfile.write(szz);
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


