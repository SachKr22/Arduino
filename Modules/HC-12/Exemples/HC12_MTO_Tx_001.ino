/*
  Version: 2.2
  Project is create a weather monitoring station, powered by a solar panel which also charges 4 AA
  batteries. Using a power saving mode (never tried), Arduino should run 24/7.
  Tried averaging readings, did not work as desired.
  Sleep mode works if at top of code.
  Replaced Xbee's with HC-12's, this offers longer range and less cost.
  Replaced DHT11 with BME280.
  Added a circuit to monitor battery voltage, its designed to disconnect output if battery falls < 11.4VDC.
*/

// Sleep mode lib
#include "LowPower.h"

// We'll use SoftwareSerial to communicate with the HC12:
#include <SoftwareSerial.h>

SoftwareSerial MySerial(2, 3); // RX, TX

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

float ff;
float addff;
float hh;
float addhh;
float addhin;
float f;
float h;
float p;
int x;
int y;
int z;

void setup() {
  Serial.begin(9600);
  MySerial.begin(9600);
  bme.begin(0x77);

} // end setup

void loop() {
  // location of sleep mode seems to have desired effect, must be at top of code.
  for (int i = 0; i < 65; i++) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);    // 8s is max for low.power lib, but with a 65 loops = 10mins
  }
  for (z = 0; z < 2; z++) {          // needed to set at least 2 runs, serial data does not always send on just one
    for (x = 0; x < 20; x++) {       // increased sample size to 20 to minimize spikes in data effecting average
      delay (500);                   // small delay to allow DHT to start and collect data
      int cc = (bme.readTemperature());
      ff = (cc * 9.0 / 5.0) + 25.0;
      addff = ff + addff;

      hh = (bme.readHumidity());
      addhh = hh + addhh;

      float hpa = (bme.readPressure() / 100.0F);
      float hin = hpa * 0.0295299830714;
      addhin = hin + addhin;

    } // end for x loop

    for (y = 0; y < 1; y ++) {
      f = addff / 20;
      h = addhh / 20;
      p = addhin / 20;
      Serial.print("Humdity: ");
      Serial.print(h);
      MySerial.write(h);
      Serial.print(" %");
      Serial.print("  Temp: ");
      Serial.print(f);
      MySerial.write(f);
      Serial.print(" deg/F");
      Serial.print("Baro: ");
      Serial.print(p);
      MySerial.write(p);
      Serial.print(" hpa");
      Serial.println("");
    }
    addff = 0;                   // clear old added data, otherwise number just keeps getting larger...
    addhh = 0;
    addhin = 0;
  }
} // end loop
