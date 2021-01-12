//
// Fichiers d'interface
//
#include <Wire.h>            // Fichier d'interface pour linterface série I2C 
#include <HTS221.h>          // Fichier d'interface pour le composant HTS221 (capteur température et humidité) de la carte des capteurs

//
// Déclaration de variables
//
double temperature;                  // variable pour la température
double humidity;                     // variable pour l'hygrométrie
unsigned int pollingInterval = 1000; // Variable de temporisation (millisecondes)

//
// Code de démarrage
//
void setup() {
  // Initialisation de la liaison série
  Serial.begin(115200);
  // Configuration de la broche connectée à la LED en sortie
  pinMode(LED_BUILTIN, OUTPUT);
  // Initialisation de l'interface I2C
  Wire.begin();

  // Initialisation des capteurs
  if (!smeHumidity.begin()) { // Led clignotante si il y a une erreur à l'initialisation du capteur
    while (1) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
    }
  }
  // Attente de la fin d'initialisation
  delay(pollingInterval);
}

//
// Boucle principale
//
void loop() {
  // Lecture de la température
  temperature = smeHumidity.readTemperature();
  Serial.print("Température: ");
  Serial.print(temperature);
  Serial.print(" celsius");

  // Lecture de l'hygrométrie
  humidity = smeHumidity.readHumidity();
  Serial.print("\tHygrométrie: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Attente
  delay(pollingInterval);
}