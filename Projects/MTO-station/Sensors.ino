// Consolidtation des codes

// Librairies
#include "DHT.h"    // DHT22
#include "MQ135.h"  // MQ135

// Variables


// Definition des pins
#define DHTPIN 8          // broche ou l'on a branche le capteur
const int mq135Pin = 0;   // Pin sur lequel est branché de MQ135

// Section DHT22
// de-commenter le capteur utilise
  //#define DHTTYPE DHT11 // DHT 11
  //#define DHTTYPE DHT21 // DHT 21 (AM2301)
#define DHTTYPE DHT22 // DHT 22 (AM2302)

//#define DHTTYPE DHT21 // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);   //déclaration du capteur

// Section MQ135
MQ135 gasSensor = MQ135(mq135Pin);  // Initialise l'objet MQ135 sur le Pin spécifié

void setup()
{
  Serial.begin(9600);
  float rzero = gasSensor.getRZero();
  Serial.print("R0: ");
  Serial.println(rzero);  // Valeur à reporter ligne 27 du fichier mq135.h après 48h de préchauffage
  dht.begin();
}

void loop()
{
  // Loop DHT
  delay(5000);  // Delai d'actualisation de la mesure
                // La lecture du capteur MQ135 prend 250ms
                // Les valeurs MQ135 lues peuvent etre vieilles de jusqu'a 2 secondes (le capteur lent)
  float h = dht.readHumidity();     // Lecture hygrometrie
  float t = dht.readTemperature();  // Lecture Temperature en celsius (par defaut)

  //On verifie si la lecture a echoue, si oui on quitte la boucle pour recommencer.
  if (isnan(h) || isnan(t))
  {
    Serial.println("Lecture DHT error!");
    return;
  }

  // Calcul de l'indice de temperature en Celsius
  float hic = dht.computeHeatIndex(t, h, false);
  // Fin Loop DHT

  // Loop MQ135
  float ppm = gasSensor.getPPM();
  // Fin Loop MQ135

 // Affichages :
 // Affichage DHT
 Serial.print("Humidite: ");
 Serial.println(h);
 Serial.print("Temperature: ");
 Serial.print(t);
 Serial.println();

 // Affichage MQ135
 Serial.print("A0: ");
 Serial.println(analogRead(mq135Pin));
 Serial.print("ppm CO2: ");
 Serial.println(ppm);
 Serial.println();
}
