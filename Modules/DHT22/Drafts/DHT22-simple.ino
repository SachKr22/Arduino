//
#include "DHT.h"
#define DHTPIN 7
#define DHTTYPE DHT22 // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE); //déclaration du capteur

void setup()
{
 Serial.begin(9600);
 Serial.println("DHTxx test!");
 dht.begin();
}
void loop()
{
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
 Serial.print("Humidite: ");
 Serial.print(h);
 Serial.print(" %\t");
 Serial.print("Temperature: ");
 Serial.print(t);
 Serial.println(" *C ");

}
