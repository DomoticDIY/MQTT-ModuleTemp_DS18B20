# MQTT - ModuleTemp_DS18B20
Capteur de température autonome. Transfert des données à Domoticz, via MQTT

But : Remonter les informations de température depuis un capteur autonome, et transfèrer l'information à Domoticz via MQTT.

Voici comment créer un capteur de tempèrature, à l'aide d'un module DS18B20, support d'un ESP8266, et envoyant les informations au format JSON par protocole MQTT. L'intéret est de récupérer ces informations de température dans un outils domotique. Dans l'exemple de la vidéo, nous nous appuieront sur Domoticz.

Partie logiciel necessaire : 
- Driver USB CH340G : https://wiki.wemos.cc/downloads 
- Logiciel Arduino IDE : https://www.arduino.cc/en/Main/Software 
- URL à ajouter pour le Bord manager : http://arduino.esp8266.com/stable/package_esp8266com_index.json 

Installer la prise en charge des cartes ESP8266

Bibliothéques :
- pubsubclient : https://github.com/knolleary/pubsubclient
- ArduinoJson : https://github.com/bblanchon/ArduinoJson 

Dans IDE : Faire Croquis / inclure une bibliothéque / ajouter la bibliothèque ZIP.

Puis dans IDE : Faire Croquis / inclure une bibliothéque / Gérer les bibliothèques, et ajouter :
  - ESP8266Wifi
  - OnWire
  - DallasTemperature

Adaptation pour reconnaissance dans Domoticz : Dans le fichier PubSubClient.h : La valeur du paramètre doit être augmentée à 512 octets. Cette définition se trouve à la ligne 26 du fichier, sinon cela ne fonctionne pas avec Domoticz

# Explications et Tuto vidéo
Dans la suite des créations d'objets connectés, nous allons voir dans cette vidéo, comment créer un détecteur de température autonome. L’objet connecté fonctionne via MQTT, et transmet ses informations à Domoticz. Pour rappel, nous utilisons ici Domoticz, mais le code est fourni, et peut être facilement adapté pour d’autre logiciel de domotique.

## Soft et bibliothèques à installer :
- Driver USB CH340G : https://wiki.wemos.cc/downloads
- Logiciel Arduino IDE : https://www.arduino.cc/en/Main/Software
- URL à ajouter pour le Bord manager : http://arduino.esp8266.com/stable/package_esp8266com_index.json

Installer la prise en charge des cartes ESP8266

Bibliothéques :
 - pubsubclient : https://github.com/knolleary/pubsubclient
 - ArduinoJson : https://github.com/bblanchon/ArduinoJson
Dans IDE : Faire Croquis / inclure une bibliothéque / ajouter la bibliothèque ZIP.


Adaptation pour reconnaissance dans Domoticz :
Dans le fichier PubSubClient.h : La valeur du paramètre doit être augmentée à 512 octets. Cette définition se trouve à la ligne 26 du fichier, sinon cela ne fonctionne pas avec Domoticz

## Tuto vidéo
Vidéo explicative sur YouTube : https://www.youtube.com/c/DomoticDIY
