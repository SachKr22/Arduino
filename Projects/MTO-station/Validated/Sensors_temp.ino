// Consolidtation des codes
//Collecte des données via DHT22 & MQ135 -> MQTT -> Rasp -> InfluxDB -> Grafana

// Librairies
#include "DHT.h"              // DHT22 - Temp/Humidite
#include "MQ135.h"            // MQ135 - Air Quality
#include <Wire.h>             // ???
#include <SPI.h>              // ???
#include <Ethernet.h>         // Shield Ethernet
#include <PubSubClient.h>     // MQTT
#define IR_output 2

// Definition des pins & Variables
#define DHTPIN 8                // broche ou l'on a branche le capteur
const int mq135Pin = 0;         // Pin sur lequel est branché de MQ135
#define DHTTYPE DHT22 // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);   //déclaration du capteur
MQ135 gasSensor = MQ135(mq135Pin);  // Initialise l'objet MQ135 sur le Pin spécifié

// Mettre à jour les 3 lignes suivantes selon la configuration réseau:
byte mac[] = { 0xFB, 0xA2, 0xDA, 0x0E, 0xD1, 0xFB }; // mac adress shield Ethernet
IPAddress ip(192, 168, 1, 70);     // (192.168.1.70) adresse IP carte UNO
IPAddress server(192, 168, 1, 11);   //(192.168.1.11);  // Broker

//#define mqtt_user "moi"         // Si configuré sur le Broker !
//#define mqtt_password "motdepasse" // Si configuré sur le Broker !

long lastMsg = 0;
int value = 0;
char message_buff[100];

// Fonction appelée lors de la réception d'un topic + traitement du payload:
void callback(char* topic, byte* payload, unsigned int length)
{
  int i = 0;
  Serial.print("Message arrived [");
  Serial.print(topic);
  String topic2 = String(topic);
  Serial.println("] ");
  // Traitement du payload, on le transforme en String.
  for (i = 0; i<length; i++)
  {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  String msgString = String(message_buff);
  Serial.println("Payload: " + msgString);
}// Fin fonction appelée lors de la réception d'un topic + traitement du payload

// Fonction connexion
EthernetClient ethClient;
PubSubClient client(ethClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // On attend la connection, mqtt_user et mqtt_password si configuré sur le Broker !
    // (client.connect("arduinoClient", mqtt_user, mqtt_password))
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // une fois connectée, on souscrit au différent topic:
      client.subscribe("stationUno/switchBleu");
      client.subscribe("stationUno/switchVert");
      client.subscribe("stationUno/switchRouge");
      client.subscribe("stationUno/switchJaune");

    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // On attend 5 secondes pour essayer de se reconnecter au Broker
      delay(5000);
    }
  }
} // Fin fonction connexion

void setup() {
  Serial.begin(115200);
  pinMode(IR_output,INPUT);   // Capteur PIR
  float rzero = gasSensor.getRZero();
  Serial.print("R0: ");
  Serial.println(rzero);  // Valeur à reporter ligne 27 du fichier mq135.h après 48h de préchauffage
  dht.begin();

// Configuration client / server
  client.setServer(server, 1883);
  client.setCallback(callback);
  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
} // Fin Setup

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();

  // Boucle capteur PIR
  if(digitalRead(IR_output)==HIGH){
   //Serial.println("mouvement detecte");
   client.publish("alarme/intrusion", "ALARME INTRUSION DETECTEE !");
   Serial.println("ALARME INTRUSION DETECTEE !");
   }
  if(digitalRead(IR_output)==LOW){
   Serial.println("pas de mouvement detecte");

   Serial.println("Publish message Capteur PIR");
  }
  delay(2000);
  // Fin boucle capteur PIR

  if (now - lastMsg > 5000) // toute les 5 secondes on envoi les mesures au Broker
  {
    char t[10];
    lastMsg = now;
    ++value;
    // On envoi les mesures au Broker:
    float temp_U = dht.readTemperature();  // Lecture Temperature en celsius (par defaut)
    dtostrf(temp_U, 3, 1, t);  // Conversion float en char.
    client.publish("stationUno/temperature", t);

    float hum_U = dht.readHumidity();     // Lecture hygrometrie
    dtostrf(hum_U, 3, 1, t);
    client.publish("stationUno/humidite", t);

    float ppm_U = gasSensor.getPPM();     // Lecture CO2 en ppm
    dtostrf(ppm_U, 3, 1, t);
    client.publish("stationUno/ppm", t);

    Serial.println("Publish message du DHT22 & MQ135");

  }
}
