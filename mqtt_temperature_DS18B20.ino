
/*
 ESP8266 MQTT - Relevé de température via DS10B20
 Création Dominique PAUL.
 Dépot Github : https://github.com/DomoticDIY/MQTT-ModuleTemp_DS18B20
 Chaine YouTube du Tuto Vidéo : https://www.youtube.com/c/DomoticDIY
  
 Bibliothéques nécessaires :
  - pubsubclient : https://github.com/knolleary/pubsubclient
  - ArduinoJson v5.13.3 : https://github.com/bblanchon/ArduinoJson
 Télécharger les bibliothèques, puis dans IDE : Faire Croquis / inclure une bibliothéque / ajouter la bibliothèque ZIP.
 Puis dans IDE : Faire Croquis / inclure une bibliothéque / Gérer les bibliothèques, et ajouter :
  - OnWire
  - DallasTemperature
  - ESP8266Wifi.
 Installer le gestionnaire de carte ESP8266 version 2.5.0 
 Si besoin : URL à ajouter pour le Bord manager : http://arduino.esp8266.com/stable/package_esp8266com_index.json
 Adaptation pour reconnaissance dans Domoticz :
 Dans le fichier PubSubClient.h : La valeur du paramètre doit être augmentée à 512 octets. Cette définition se trouve à la ligne 26 du fichier.
 Sinon cela ne fonctionne pas avec Domoticz
 Pour prise en compte du matériel :
 Installer si besoin le Driver USB CH340G : https://wiki.wemos.cc/downloads
 dans Outils -> Type de carte : generic ESP8266 module
  Flash mode 'QIO' (régle générale, suivant votre ESP, si cela ne fonctionne pas, tester un autre mode.
  Flash size : 1M (no SPIFFS)
  Port : Le port COM de votre ESP vu par windows dans le gestionnaire de périphériques.
*/

// Inclure les librairies.
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <OneWire.h>                                  //Librairie du bus OneWire
#include <DallasTemperature.h>                        //Librairie du capteur


// Déclaration des constantes, données à adapter à votre réseau.
// ------------------------------------------------------------
const char* ssid = "_MON_SSID_";                	// SSID du réseau Wifi
const char* password = "_MOT_DE_PASSE_WIFI_";   	// Mot de passe du réseau Wifi.
const char* mqtt_server = "_IP_DU_BROKER_";     	// Adresse IP ou DNS du Broker.
const int mqtt_port = 1883;                     	// Port du Brocker MQTT
const char* mqtt_login = "_LOGIN_";             	// Login de connexion à MQTT.
const char* mqtt_password = "_PASSWORD_";       	// Mot de passe de connexion à MQTT.
// ------------------------------------------------------------
// Variables de configuration :
#define ONE_WIRE_BUS 2                                // Pin de connexion de la DS18B20
char* topicIn = "domoticz/out";                       // Nom du topic envoyé par Domoticz
char* topicOut = "domoticz/in";                       // Nom du topic écouté par Domoticz
float valTemp = 0.0;                                  // Variables contenant la valeur de température.
float valTemp_T;                                      // Valeurs de relevé temporaires.
#define tempsPause 30                                 // Nbre de secondes de pause (3600 = 1H00)
// ------------------------------------------------------------
// Variables et constantes utilisateur :
String nomModule = "Température DS18B20";             // Nom usuel de ce module. Sera visible uniquement dans les Log Domoticz.
int idxDevice = 31;                                   // Index du Device à actionner.
// ------------------------------------------------------------


WiFiClient espClient;
PubSubClient client(espClient);
OneWire oneWire(ONE_WIRE_BUS);              // Initialisation du Bus One Wire
DallasTemperature sensors(&oneWire);        // Utilistion du bus Onewire pour les capteurs



// SETUP
// *****
void setup() {
  Serial.begin(115200);                       // On initialise la vitesse de transmission de la console.
  sensors.begin();                            // On initialise la bibliothèque Dallas
  setup_wifi();                               // Connexion au Wifi
  client.setServer(mqtt_server, mqtt_port);   // On défini la connexion MQTT
}

// BOUCLE DE TRAVAIL
// *****************
void loop() {
  if (!client.connected()) {
    Serial.println("MQTT déconnecté, on reconnecte !");
    reconnect();
  } else {
    // On interroge la sonde de Température.
    getTemperatureC();
    // Envoi de la données via JSON et MQTT
    SendData();
    // On met le système en pause pour un temps défini
    delay(tempsPause * 1000);
  }
}


// CONNEXION WIFI
// **************
void setup_wifi() {
  // Connexion au réseau Wifi
  delay(10);
  Serial.println();
  Serial.print("Connection au réseau : ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    // Tant que l'on est pas connecté, on boucle.
    delay(500);
    Serial.print(".");
  }
  // Initialise la séquence Random
  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connecté");
  Serial.print("Addresse IP : ");
  Serial.println(WiFi.localIP());
}

// CONNEXION MQTT
// **************
void reconnect() {
  // Boucle jusqu'à la connexion MQTT
  while (!client.connected()) {
    Serial.print("Tentative de connexion MQTT...");
    // Création d'un ID client aléatoire
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Tentative de connexion
    if (client.connect(clientId.c_str(), mqtt_login, mqtt_password)) {
      Serial.println("connecté");
      
      // Connexion effectuée, publication d'un message...
      String message = "Connexion MQTT de "+ nomModule + " réussi sous référence technique : " + clientId + ".";
      // String message = "Connexion MQTT de "+ nomModule + " réussi.";
      StaticJsonBuffer<256> jsonBuffer;
      // Parse l'objet root
      JsonObject &root = jsonBuffer.createObject();
      // On renseigne les variables.
      root["command"] = "addlogmessage";
      root["message"] = message;
      
      // On sérialise la variable JSON
      String messageOut;
      if (root.printTo(messageOut) == 0) {
        Serial.println("Erreur lors de la création du message de connexion pour Domoticz");
      } else  {
        // Convertion du message en Char pour envoi dans les Log Domoticz.
        char messageChar[messageOut.length()+1];
        messageOut.toCharArray(messageChar,messageOut.length()+1);
        client.publish(topicOut, messageChar);
      }
        
      // On souscrit (écoute)
      client.subscribe("#");
    } else {
      Serial.print("Erreur, rc=");
      Serial.print(client.state());
      Serial.println(" prochaine tentative dans 5s");
      // Pause de 5 secondes
      delay(5000);
    }
  }
}

// RELEVE DE TEMPERATURE.
// *********************
void getTemperatureC() {
  // On relévé la température.
  // ------------------------
  sensors.requestTemperatures();            // On demande au capteur de lire la température
  valTemp = sensors.getTempCByIndex(0);      // On stocke la température relevé dans une variable temporaire
  Serial.print("Valeur de température relevée : ");
  Serial.println(valTemp);
}

// ENVOI DES DATAS.
// ***************
void SendData () {
  // Création et Envoi de la donnée JSON.
  StaticJsonBuffer<256> jsonBuffer;
  // Parse l'objet root
  JsonObject &root = jsonBuffer.createObject();
  // On renseigne les variables.
  root["type"]    = "command";
  root["param"]   = "udevice";
  root["idx"]     = idxDevice;
  root["nvalue"]  = 0;
  root["svalue"]  = String(valTemp);
      
  // On sérialise la variable JSON
  String messageOut;
  if (root.printTo(messageOut) == 0) {
    Serial.println("Erreur lors de la création du message de connexion pour Domoticz");
  } else  {
    // Convertion du message en Char pour envoi dans les Log Domoticz.
    char messageChar[messageOut.length()+1];
    messageOut.toCharArray(messageChar,messageOut.length()+1);
    client.publish(topicOut, messageChar);
    // Pause de 5 secondes
    delay(5000);
    Serial.println("Message envoyé à Domoticz");
  }
}
