// Basé sur le code de LadyAda traduit et modifie par moi, domaine public
#include "DHT.h"
#define DHTPIN 8 // broche ou l'on a branche le capteur
// de-commenter le capteur utilise
//#define DHTTYPE DHT11 // DHT 11
#define DHTTYPE DHT22 // DHT 22 (AM2302)
//#define DHTTYPE DHT21 // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);//déclaration du capteur
 
void setup()
{
 Serial.begin(9600);
 Serial.println("DHT22 test!");
 dht.begin();
}
void loop()
{
 delay(5000);
 
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
 
 // Calcul de l'indice de temperature en Celsius
 float hic = dht.computeHeatIndex(t, h, false);
 
 //Affichages :
 Serial.print("Humidite: ");
 Serial.print(h);
 Serial.print(" \t");
 Serial.print("Temperature: ");
 Serial.print(t);
 //Serial.print(" *C ");
 Serial.println();
}
