/*
 * SD_Card_Test.ino
 * Writes a simple text file to an SD card so you can test out your
 * wiring and card formatting. Modified from the Arduion Datalogger
 * example sketch.
 * J.R. Leeman
 */

#include <SPI.h>
#include <SD.h>

const int chipSelect = 16;

void setup() {
  Serial.begin(9600);

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");

  Serial.println("Writing test file");

  // Open the file
  File dataFile = SD.open("test.txt", FILE_WRITE);

  // Write a test string
  dataFile.println("You have setup your SD Card correctly!");

  // Close the file 
  dataFile.close();

  Serial.println("Complete!");
}

void loop() {
}









