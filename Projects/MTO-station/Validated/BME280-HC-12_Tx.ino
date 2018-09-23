/*    Arduino Long Range Wireless Communication using HC-12
      Tx = Arduino Micro - Rx = Arduino Uno
*/

#include <SoftwareSerial.h>
#include "DHT.h"
#define DHTPIN 7   // DHT22 pin
#define DHTTYPE DHT22 // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE); //déclaration du capteur
SoftwareSerial HC12(10, 11); // HC-12 TX Pin 10, HC-12 RX Pin 11

void setup() {
  Serial.begin(115200);             // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12
  dht.begin();

}

void loop() {
  while (HC12.available()) {        // If HC-12 has data
    Serial.write(HC12.read());      // Send the data to Serial monitor
  }
  while (Serial.available()) {      // If Serial monitor has data
    HC12.write(Serial.read());      // Send that data to HC-12
  }
  delay(2000);
  // La lecture du capteur prend 250ms
  // Les valeurs lues peuvet etre vieilles de jusqu'a 2 secondes (le capteur est lent)
  float h = dht.readHumidity();//on lit l'hygrometrie
  float t = dht.readTemperature();//on lit la temperature en celsius (par defaut)

  //On verifie si la lecture a echoue, si oui on quitte la boucle pour recommencer.
  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float hic = dht.computeHeatIndex(t, h, false);

  //Affichages :
  HC12.print("Humidite: ");
  HC12.print(h);
  HC12.print(" %\t");
  HC12.print("Temperature: ");
  HC12.print(t);
  HC12.println(" *C ");

}
