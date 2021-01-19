#include <HTS221.h>

#include <Wire.h>
#include <HTS221.h>
#include <Arduino.h>

// FOR I2C sensors
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// Used for software SPI
#define LIS3DH_CLK 13
#define LIS3DH_MISO 12
#define LIS3DH_MOSI 11
// Used for hardware & software SPI
#define LIS3DH_CS 10
// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// Adjust this number for the sensitivity of the 'click' force
// this strongly depend on the range! for 16G, try 5-10
// for 8G, try 10-20. for 4G try 20-40. for 2G try 40-80
#define CLICKTHRESHHOLD 80

// for LCD
#include <MD_Parola.h>
#include <MD_MAX72xx.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary output pins
// MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);


double temperature;
unsigned int pollingInterval = 1000; // in milliseconds
int compteur = 0;

void setup() {
  // Initialisation de la liaison série 
  Serial.begin(115200);

  Serial.println("C'est parti!");

  // Initialisation de la matrice LCD 
  P.begin();
  P.displayText("Hello", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);  

  Wire.begin();

  // Initialisation des capteurs  
  if (!smeHumidity.begin()) { // Led On if there is an error initializing the sensor
        digitalWrite(LED_BUILTIN, HIGH);
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    //while (1);
  }
  delay(pollingInterval);  
}

void loop() {
  
  P.displayAnimate();

  // L'affichage de la température se faire tous les 10000 passages seulement 
  if (compteur >10000) 
  {
    // Lecture de la température 
    temperature = smeHumidity.readTemperature();
    Serial.print(" | Temperature: ");
    Serial.println(temperature);
    
    // Affichage de la température 
    P.print(String(temperature,1)+" C");

    // On repart à 0 dès qu'on atteint 10000, on repart à 0 
    compteur = 0;
  }
  else
  {
    compteur ++;
  }
}
