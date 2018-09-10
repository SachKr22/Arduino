// Consolidtation des codes
//HC-12 messenger MTO send/receive

// Librairies
#include "DHT.h"              // DHT22 - Temp/Humidite
#include "MQ135.h"            // MQ135 - Air Quality
#include <Wire.h>             // ???
#include <SPI.h>              // ???
#include <Ethernet.h>         // Shield Ethernet
#include <PubSubClient.h>     // MQTT
//#include <SoftwareSerial.h>   // HC-12 - 433mhz si nécessaire

// Definition des pins
int led_Red = 5;

// Mettre à jour les 3 lignes suivantes selon votre configuration réseau:
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

  if (topic2 == "stationUno/switchRouge") // LED rouge
  {
    if (msgString == "ON") {
      digitalWrite(led_Red, HIGH);
    }
    else {
      digitalWrite(led_Red, LOW);
    }
  }
}
// Fin fonction appelée lors de la réception d'un topic + traitement du payload

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
}
// Fin fonction connexion

void setup() {
  Serial.begin(115200);
  //float rzero = gasSensor.getRZero();
  //Serial.print("R0: ");
  //Serial.println(rzero);  // Valeur à reporter ligne 27 du fichier mq135.h après 48h de préchauffage
  //dht.begin();

  // Configuration des pins Arduino en sortie:
  pinMode(led_Red, OUTPUT);



  client.setServer(server, 1883);
  client.setCallback(callback);
  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
  /*
  long now = millis();
  if (now - lastMsg > 5000) // toute les 5 secondes on envoi les mesures au Broker
  {
    char t[10];
    lastMsg = now;
    ++value;
    // On envoi les mesures au Broker:
    float temp_U = bme.readTemperature();
    dtostrf(temp_U, 3, 2, t);  // Conversion float en char.
    client.publish("stationUno/temperature", t);

    float hum_U = bme.readHumidity();
    dtostrf(hum_U, 3, 2, t);
    client.publish("stationUno/humidite", t);

    float press_U = bme.readPressure();
    dtostrf(press_U, 0, 0, t);
    client.publish("stationUno/pression", t);

    float altit_U = bme.readAltitude(1013.25);
    dtostrf(altit_U, 3, 2, t);
    client.publish("stationUno/altitude", t);

    Serial.println("Publish message du BME280");

  }
  */

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
  /* Export en JSON
  mySerial.print("{\"hum_raw\":");
  mySerial.print(h);
  mySerial.print(",\"temp_raw\":");
  mySerial.print(t);
  mySerial.print(",\"co2_raw\":");
  mySerial.print(ppm);
  mySerial.print("}");
  */
