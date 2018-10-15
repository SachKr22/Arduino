// Code consolide Rx BME280 depuis HC-12 -> mqtt_user

// Librairies
#include <Wire.h>             // ???
#include <SPI.h>              // ???
#include <Ethernet.h>         // Shield Ethernet
#include <PubSubClient.h>     // MQTT
#include <SoftwareSerial.h>   // Moniteur serie

// Definition des pins du HC-12 (Vin-GND-Rx-Tx)
SoftwareSerial HC12(10, 11); // HC-12 TX Pin 10, HC-12 RX Pin 11

// Definition des pins & Variables
#define IR_output 2           // Capteur PIR

// CONFIGURATION MQTT - Mettre à jour les 3 lignes suivantes selon la configuration réseau:
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
      Serial.println("connected :)");
      // une fois connectée, on souscrit au différent topic:
      //client.subscribe("stationUno/switchBleu");

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
  Serial.begin(115200);           // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12
  pinMode(IR_output,INPUT);       // Capteur PIR
  // Configuration client / server
  client.setServer(server, 1883);
  client.setCallback(callback);
  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
} // Fin Setup

void loop() {
  while (HC12.available()) {        // Si je recois sur via HC-12
    Serial.write(HC12.read());      // Envoi vers Serial monitor
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();

  // DEBUT boucle capteur PIR
  if(digitalRead(IR_output)==HIGH){
   //Serial.println("mouvement detecte");
   client.publish("alarme/intrusion", "PYRO-ALARM");
   Serial.println("PYRO-ALARM");
   Serial.println("Publish message Capteur PIR");
   delay(2000);
   }
  // FIN boucle capteur PIR

  // DEBUT boucle communication MTO MQTT
  if (now - lastMsg > 5000) // toute les 5 secondes on envoie les mesures au Broker
  {
    char t[10];
    lastMsg = now;
    ++value;
    // On envoi les mesures au Broker:

//    float temp_U = HC12.read());  // Lecture Temperature en celsius
    dtostrf(HC12.read(), 3, 1, t);  // Conversion float en char.
    client.publish("stationUno/temperature", t);
/*
    float hum_U = mySensor.readFloatHumidity();     // Lecture hygrometrie
    dtostrf(hum_U, 3, 1, t);
    client.publish("stationUno/humidite", t);

    float press_U = (mySensor.readFloatPressure() / 100);     // Lecture pression en HPa
    dtostrf(press_U, 3, 1, t);
    client.publish("stationUno/pressure", t);

    float ppm_U = gasSensor.getPPM();     // Lecture CO2 en ppm
    dtostrf(ppm_U, 3, 1, t);
    client.publish("stationUno/ppm", t);

    Serial.println("Publish message du HC-12");
*/
  }
  // FIN boucle communication MTO MQTT
}
