#include "SparkFunBME280.h"
//Library allows either I2C or SPI, so include both.
#include "Wire.h"
#include "SPI.h"
BME280 mySensor;

unsigned long sampleNumber = 0;

void setup() {

   Serial.begin(9600);
  while (!Serial) {
    // Attente de l'ouverture du port série pour Arduino LEONARDO
  }
  //configuration du capteur
  mySensor.settings.commInterface = I2C_MODE;
  mySensor.settings.I2CAddress = 0x76;
  mySensor.settings.runMode = 3;
  mySensor.settings.tStandby = 3;
  mySensor.settings.filter = 3;
  mySensor.settings.tempOverSample = 5;
  mySensor.settings.pressOverSample = 5;
  mySensor.settings.humidOverSample = 5;

  Serial.println("Starting BME280... ");
  delay(10);  // attente de la mise en route du capteur. 2 ms minimum
  // chargement de la configuration du capteur
  mySensor.begin();



  //Build a first-row of column headers
  Serial.print("SampleNumber,");
  Serial.print("Time(ms),");
  Serial.print("T(deg C),");
  Serial.print("T(deg F),");
  Serial.print("P(Pa),");
  Serial.print("Alt(m),");
  Serial.print("Alt(ft),");
  Serial.print("%RH");
  Serial.println("");
}

void loop()
{
  //Print each row in the loop
  //Start with temperature, as that data is needed for accurate compensation.
  //Reading the temperature updates the compensators of the other functions
  //in the background.
  Serial.print(sampleNumber);
  Serial.print(",");
  Serial.print(millis());
  Serial.print(",");
  Serial.print(mySensor.readTempC(), 1);
  Serial.print(",");
  Serial.print(mySensor.readTempF(), 1);
  Serial.print(",");
  Serial.print(mySensor.readFloatPressure(), 0);
  Serial.print(",");
  Serial.print(mySensor.readFloatAltitudeMeters(), 3);
  Serial.print(",");
  Serial.print(mySensor.readFloatAltitudeFeet(), 3);
  Serial.print(",");
  Serial.print(mySensor.readFloatHumidity(), 0);
  Serial.println();

  sampleNumber++;

  delay(50);
}
