// Consolidtation des codes
//HC-12 messenger MTO send/receive

// Librairies
#include "DHT.h"              // DHT22 - Temp/Humidite
#include "MQ135.h"            // MQ135 - Air Quality
#include <SoftwareSerial.h>   // HC-12 - 433mhz

// Variables


// Definition des pins
#define DHTPIN 8                // broche ou l'on a branche le capteur
const int mq135Pin = 0;         // Pin sur lequel est branché de MQ135
SoftwareSerial mySerial(2, 3);  // RX, TX

// Section DHT22
// de-commenter le capteur utilise
  //#define DHTTYPE DHT11 // DHT 11
  //#define DHTTYPE DHT21 // DHT 21 (AM2301)
#define DHTTYPE DHT22 // DHT 22 (AM2302)

//#define DHTTYPE DHT21 // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);   //déclaration du capteur

// Section MQ135
MQ135 gasSensor = MQ135(mq135Pin);  // Initialise l'objet MQ135 sur le Pin spécifié

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  float rzero = gasSensor.getRZero();
  Serial.print("R0: ");
  Serial.println(rzero);  // Valeur à reporter ligne 27 du fichier mq135.h après 48h de préchauffage
  dht.begin();
}

void loop() {

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
  /*
  Serial.print("Humidite: ");
  Serial.println(h);
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println();

  // Affichage MQ135
  //Serial.print("A0: ");
  //Serial.println(analogRead(mq135Pin));
  Serial.print("ppm CO2: ");
  Serial.println(ppm);
  Serial.println();
  */
  // Export en JSON
  mySerial.print("{\"hum_raw\":");
  mySerial.print(h);
  mySerial.print(",\"temp_raw\":");
  mySerial.print(t);
  mySerial.print(",\"co2_raw\":");
  mySerial.print(ppm);
  mySerial.println("}");

  // Loop HC-12
  if(Serial.available() > 0){//Read from serial monitor and send over HC-12
    String input = Serial.readString();
    mySerial.println(input);
  }

  if(mySerial.available() > 1){//Read from HC-12 and send to serial monitor
    String input = mySerial.readString();
    Serial.println(input);
  }
  delay(20);
  // Fin Loop HC-12


}
