//
// Fichiers d'interface 
//

#include <Wire.h>            // Fichier d'interface pour linterface série I2C 
#include <HTS221.h>          // Fichier d'interface pour le composant HTS221 (capteur température et humidité) de la carte des capteurs
#include <Arduino.h>         // Fichier de définitions de déclarations Arduino 
#include <Adafruit_Sensor.h> // Fichier de définition d'interfaces capteurs 

//
// Déclaration de variables 
//

double temperature;                  // variable pour la température 
double humidity;                     // variable pour l'hygrométrie
unsigned int pollingInterval = 1000; // Variable de temporisation (millisecondes)
int compteur = 0;                    // Variable de comptage pour capture de la température 

//
// Code de démarrage  
//


void setup() {
  // Initialisation de la liaison série 
  Serial.begin(115200);

  Wire.begin();

  // Initialisation des capteurs  
  if (!smeHumidity.begin()) 
  {     // Led On if there is an error initializing the sensor
        digitalWrite(LED_BUILTIN, HIGH);
  }
  else 
  {
    digitalWrite(LED_BUILTIN, LOW);
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
  Serial.print(" | Temperature: ");
  Serial.print(temperature);
  delay(2000);


  // Lecture de l'hygrométrie 
  humidity = smeHumidity.readHumidity();
  Serial.print(" | Hygométrie (%): ");
  Serial.println(humidity);


  // Attente 
  delay(pollingInterval);   
}
