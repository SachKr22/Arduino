/*
  Programme de test du MQ135 permettant de mesurer la présence de polluants dans l'atmosphère (CO, CO2 Alcool, fumées...)
  Pour plus d'infos ici https://www.projetsdiy.fr/mq135-mesure-qualite-air-polluant-arduino
  Utilise la librairie mq135.cpp mise à disposition par Georg Krocker https://github.com/GeorgK/MQ135
  Projets DIY - Mars 2016 - www.projetsdiy.fr
*/

#include "MQ135.h"
const int mq135Pin = 0;     // Pin sur lequel est branché de MQ135

MQ135 gasSensor = MQ135(mq135Pin);  // Initialise l'objet MQ135 sur le Pin spécifié

void setup()
{
    Serial.begin(9600);     // Initialise le port série à 9600 bps
    float rzero = gasSensor.getRZero();
    Serial.print("R0: ");
    Serial.println(rzero);  // Valeur à reporter ligne 27 du fichier mq135.h après 48h de préchauffage

}

void loop()
{
    float ppm = gasSensor.getPPM();
    Serial.print("A0: ");
    Serial.print(analogRead(mq135Pin));
    Serial.print(" ppm CO2: ");
    Serial.println(ppm);
    delay(5000);        // Delai d'actualisation de la mesure
}
