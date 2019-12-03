//
// Fichiers d'interface 
//

#include <Wire.h>            // Fichier d'interface pour linterface série I2C 
#include <Arduino.h>         // Fichier de définitions de déclarations Arduino 
#include <Adafruit_Sensor.h> // Fichier de définition d'interfaces capteurs 

//
// Déclaration de variables 
//

int          lumiere;                    // variable pour la lumiere 
unsigned int pollingInterval = 10;       // Variable de temporisation (millisecondes)

//
// Code de démarrage  
//


void setup() {
  // Initialisation de la liaison série 
  Serial.begin(115200);

  Wire.begin();
  
  // Attente de la fin d'initialisation 
  delay(pollingInterval);  
}

//
// Boucle principale 
//


void loop() {
  
  // Lecture de la lumiere 
  lumiere = analogRead(A1);
  Serial.print("Lumière: "); 
  Serial.println((int)lumiere);
  
  // Attente 
  delay(pollingInterval);   
}
