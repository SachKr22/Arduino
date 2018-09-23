/*
Le capteur dispose de plusieurs paramètres de configuration (dans la boucle "void setup" ). Il faut se référer à la documentation du BME280 pour avoir les details de fonctionnement.
mySensor.settings.commInterface = ..
mySensor.settings.commInterface = I2C_MODE	communication sur bus I2C
mySensor.settings.commInterface = SPI_MODE	communication sur bus SPI

mySensor.settings.I2CAddress = ...
mySensor.settings.I2CAddress = 0x76	adresse du capteur si SD0=0
mySensor.settings.I2CAddress = 0x77	adresse du capteur si SD0=1

mySensor.settings.runMode = ...
mySensor.settings.runMode = 0	mode "sleep"
mySensor.settings.runMode = 1 ou 2	mode "forced".Lorsque une mesure est effectuée, le capteur retourne en mode "sleep". Pour réaliser une autre mesure, il faut activer à nouveau le mode "Forced".
mySensor.settings.runMode = 3	mode "Normal". Le capteur passe en mode "sleep" entre 2 mesures. Permet d’économiser la consommation

mySensor.settings.tStandby = ...
mySensor.settings.tStandby = 0	durée du standby entre 2 mesures 0.5ms
mySensor.settings.tStandby = 1	durée du standby entre 2 mesures 62.5ms
mySensor.settings.tStandby = 2	durée du standby entre 2 mesures 125ms
mySensor.settings.tStandby = 3	durée du standby entre 2 mesures 250ms
mySensor.settings.tStandby = 4	durée du standby entre 2 mesures 500ms
mySensor.settings.tStandby = 5	durée du standby entre 2 mesures 1000ms
mySensor.settings.tStandby = 6	durée du standby entre 2 mesures 10ms
mySensor.settings.tStandby = 7	durée du standby entre 2 mesures 20ms

mySensor.settings.filter = ... (permet de filtrer et supprimer les variations brusques (courant d’air ...))
mySensor.settings.filter = 0	pas de filtrage
mySensor.settings.filter = 1	coefficient de filtrage 2
mySensor.settings.filter = 2	coefficient de filtrage 4
mySensor.settings.filter = 3	coefficient de filtrage 8
mySensor.settings.filter = 4	coefficient de filtrage 16

mySensor.settings.tempOverSample = ...
mySensor.settings.tempOverSample=0	pas de mesure de la température.
mySensor.settings.tempOverSample=1	sur-échantillonnage x1. Résolution : 16 bit / 0.0050°C
mySensor.settings.tempOverSample=2	sur-échantillonnage x2. Résolution : 17 bit / 0.0025°C
mySensor.settings.tempOverSample=3	sur-échantillonnage x4. Résolution : 18 bit / 0.0012°C
mySensor.settings.tempOverSample=4	sur-échantillonnage x8. Résolution : 19 bit / 0.0006°C
mySensor.settings.tempOverSample=5	sur-échantillonnage x16. Résolution : 17 bit / 0.0003°C

mySensor.settings.pressOverSample =...
mySensor.settings.pressOverSample =0	pas de mesure de pression
mySensor.settings.pressOverSample =1	sur-échantillonnage x 1. Résolution 16 bit / 2.62 Pa
mySensor.settings.pressOverSample =2	sur-échantillonnage x 2. Résolution 17 bit / 1.31 Pa
mySensor.settings.pressOverSample =3	sur-échantillonnage x 4. Résolution 18 bit / 0.66 Pa
mySensor.settings.pressOverSample =4	sur-échantillonnage x 8. Résolution 19 bit / 0.33 Pa
mySensor.settings.pressOverSample =5	sur-échantillonnage x 16. Résolution 20 bit / 0.16 Pa

mySensor.settings.humidOverSample = ...
mySensor.settings.humidOverSample =0	pas de mesure d’humidité
mySensor.settings.humidOverSample =1	sur-échantillonnage x 1.
mySensor.settings.humidOverSample =2	sur-échantillonnage x 2.
mySensor.settings.humidOverSample =3	sur-échantillonnage x 4.
mySensor.settings.humidOverSample =4	sur-échantillonnage x 8.
mySensor.settings.humidOverSample =1	sur-échantillonnage x 16.
*/

/*
Le capteur dispose de plusieurs paramètres de configuration (dans la boucle "void setup" ). Il faut se référer à la documentation du BME280 pour avoir les details de fonctionnement.
mySensor.settings.commInterface = ..
mySensor.settings.commInterface = I2C_MODE  communication sur bus I2C
mySensor.settings.commInterface = SPI_MODE  communication sur bus SPI

mySensor.settings.I2CAddress = ...
mySensor.settings.I2CAddress = 0x76 adresse du capteur si SD0=0
mySensor.settings.I2CAddress = 0x77 adresse du capteur si SD0=1

mySensor.settings.runMode = ...
mySensor.settings.runMode = 0 mode "sleep"
mySensor.settings.runMode = 1 ou 2  mode "forced".Lorsque une mesure est effectuée, le capteur retourne en mode "sleep". Pour réaliser une autre mesure, il faut activer à nouveau le mode "Forced".
mySensor.settings.runMode = 3 mode "Normal". Le capteur passe en mode "sleep" entre 2 mesures. Permet d’économiser la consommation

mySensor.settings.tStandby = ...
mySensor.settings.tStandby = 0  durée du standby entre 2 mesures 0.5ms
mySensor.settings.tStandby = 1  durée du standby entre 2 mesures 62.5ms
mySensor.settings.tStandby = 2  durée du standby entre 2 mesures 125ms
mySensor.settings.tStandby = 3  durée du standby entre 2 mesures 250ms
mySensor.settings.tStandby = 4  durée du standby entre 2 mesures 500ms
mySensor.settings.tStandby = 5  durée du standby entre 2 mesures 1000ms
mySensor.settings.tStandby = 6  durée du standby entre 2 mesures 10ms
mySensor.settings.tStandby = 7  durée du standby entre 2 mesures 20ms

mySensor.settings.filter = ... (permet de filtrer et supprimer les variations brusques (courant d’air ...))
mySensor.settings.filter = 0  pas de filtrage
mySensor.settings.filter = 1  coefficient de filtrage 2
mySensor.settings.filter = 2  coefficient de filtrage 4
mySensor.settings.filter = 3  coefficient de filtrage 8
mySensor.settings.filter = 4  coefficient de filtrage 16

mySensor.settings.tempOverSample = ...
mySensor.settings.tempOverSample=0  pas de mesure de la température.
mySensor.settings.tempOverSample=1  sur-échantillonnage x1. Résolution : 16 bit / 0.0050°C
mySensor.settings.tempOverSample=2  sur-échantillonnage x2. Résolution : 17 bit / 0.0025°C
mySensor.settings.tempOverSample=3  sur-échantillonnage x4. Résolution : 18 bit / 0.0012°C
mySensor.settings.tempOverSample=4  sur-échantillonnage x8. Résolution : 19 bit / 0.0006°C
mySensor.settings.tempOverSample=5  sur-échantillonnage x16. Résolution : 17 bit / 0.0003°C

mySensor.settings.pressOverSample =...
mySensor.settings.pressOverSample =0  pas de mesure de pression
mySensor.settings.pressOverSample =1  sur-échantillonnage x 1. Résolution 16 bit / 2.62 Pa
mySensor.settings.pressOverSample =2  sur-échantillonnage x 2. Résolution 17 bit / 1.31 Pa
mySensor.settings.pressOverSample =3  sur-échantillonnage x 4. Résolution 18 bit / 0.66 Pa
mySensor.settings.pressOverSample =4  sur-échantillonnage x 8. Résolution 19 bit / 0.33 Pa
mySensor.settings.pressOverSample =5  sur-échantillonnage x 16. Résolution 20 bit / 0.16 Pa

mySensor.settings.humidOverSample = ...
mySensor.settings.humidOverSample =0  pas de mesure d’humidité
mySensor.settings.humidOverSample =1  sur-échantillonnage x 1.
mySensor.settings.humidOverSample =2  sur-échantillonnage x 2.
mySensor.settings.humidOverSample =3  sur-échantillonnage x 4.
mySensor.settings.humidOverSample =4  sur-échantillonnage x 8.
mySensor.settings.humidOverSample =1  sur-échantillonnage x 16.
*/

include "SparkFunBME280.h"
//Library allows either I2C or SPI, so include both.
#include "Wire.h"
#include "SPI.h"

BME280 bme280;

void setup() {

   Serial.begin(9600);
  while (!Serial) {
    // Attente de l'ouverture du port série pour Arduino LEONARDO
  }
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
  Serial.print("Température: ");
  Serial.print(bme280.readTempC(), 1);
  Serial.print(" °C");
  Serial.print("\t Pression: ");
  Serial.print(bme280.readFloatPressure() / 100.0F, 1);
  Serial.print(" HPa");
  Serial.print("\t humidité relative : ");
  Serial.print(bme280.readFloatHumidity(), 1);
  Serial.println(" %");
  delay(1000);
}
