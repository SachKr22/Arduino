#include <SPI.h> //bibliothèqe pour SPI
#include <Ethernet.h> //bibliothèque pour Ethernet
byte mac[] = {0xFB, 0xA2, 0xDA, 0x0F, 0xDF, 0xFB}; //adresse mac de votre carte
byte ip[] = {192, 168, 1, 70}; //adresse IP
EthernetServer serveur(80); // déclare l'objet serveur au port d'écoute 80

void setup() {
  Serial.begin (9600); //initialisation de communication série
  Ethernet.begin (mac, ip); //initialisation de la communication Ethernet
  Serial.print("\nLe serveur est sur l'adresse : ");
  Serial.println(Ethernet.localIP()); //on affiche l'adresse IP de la connexion
  serveur.begin(); // démarre l'écoute
}

void loop() {
  EthernetClient client = serveur.available(); //on écoute le port
  if (client) { //si client connecté
    Serial.println("Client en ligne\n"); //on le dit...
    if (client.connected()) { // si le client est en connecté
      while (client.available()) { // tant qu'il a des infos à transmettre
        char c=client.read(); // on lit le caractère
        Serial.write(c);// on l'écrit sur le moniteur série
        delay(1); //delai de lecture
      }
      //réponse au client
      client.println("<!DOCTYPE HTML>"); // informe le navigateur du type de document à afficher
      client.println("<html>"); //début du code html
      client.println("<head>"); //entête
      client.println("<title>Relevés analogiques</title>"); //titre de la fenêtre
      client.println("</head>");//fin d'entête
      client.println("<body>"); //corps
      client.println("<h1>Etat des pins analogiques</h1>"); //titre en grosse lettres
      client.println("<hr>"); //ligne horizontale
      for (int p=0;p<6;p++){ // boucle pour parcourir les pins
        client.print("pin A"); // affichage
        client.print(p); //numéro de pin
        client.print(" : "); // affichage
        client.print(analogRead(p)); //valeur donnée par le CAN
        client.print("<br>"); //saut de ligne
      }
      client.println("<hr>"); //ligne horizontale
      client.println("</body>"); //fin du corps
      client.println("</html>"); //fin du code html
      client.stop(); //on déconnecte le client
      Serial.println("Fin de communication avec le client");
    }
  }
}
