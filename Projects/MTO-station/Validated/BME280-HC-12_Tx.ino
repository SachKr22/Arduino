/*    Arduino Long Range Wireless Communication using HC-12
      Tx = Arduino Micro - Rx = Arduino Uno
*/

#include <SoftwareSerial.h>
#include "SparkFunBME280.h"
BME280 bme280;
SoftwareSerial HC12(10, 11); // HC-12 TX Pin 10, HC-12 RX Pin 11

void setup() {
  Serial.begin(115200);             // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12
  //configuration du capteur
  bme280.settings.commInterface = I2C_MODE;
  bme280.settings.I2CAddress = 0x76;
  bme280.settings.runMode = 3;
  bme280.settings.tStandby = 3;
  bme280.settings.filter = 3;
  bme280.settings.tempOverSample = 5;
  bme280.settings.pressOverSample = 5;
  bme280.settings.humidOverSample = 5;
  Serial.println("Starting BME280... ");
  delay(10);  // attente de la mise en route du capteur. 2 ms minimum
  // chargement de la configuration du capteur
  bme280.begin();

}

void loop() {

  // DEBUT boucle HC-12
  while (HC12.available()) {        // If HC-12 has data
    Serial.write(HC12.read());      // Send the data to Serial monitor
  }
  while (Serial.available()) {      // If Serial monitor has data
    HC12.write(Serial.read());      // Send that data to HC-12
  }
  // FIN boucle HC-12

  //Affichages :
  HC12.print("Température: ");
  HC12.print(bme280.readTempC(), 1);
  HC12.print(" °C");
  HC12.print("\t Pression: ");
  HC12.print(bme280.readFloatPressure() / 100.0F, 1);
  HC12.print(" HPa");
  HC12.print("\t humidité relative : ");
  HC12.print(bme280.readFloatHumidity(), 1);
  HC12.println(" %");
  delay(1000);

}
