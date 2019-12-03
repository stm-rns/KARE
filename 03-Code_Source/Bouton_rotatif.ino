#include <Arduino.h>

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

uint8_t scrollSpeed = 50;    // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
int brightness = 1; // 0 to 9
uint16_t scrollPause = 2000; // in milliseconds
#define  BUF_SIZE  75
char Message[BUF_SIZE] = { "" };

// For Button :
#define pinS1 6
#define pinS2 7
#define pinSelect 8

volatile int compteur = 0; 
int ancien_compteur = compteur;

volatile boolean halfleft = false;      // pour compter les demi pas
volatile boolean halfright = false;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  P.begin();
  P.setTextEffect(scrollEffect, scrollEffect);
  P.setIntensity(brightness);

  pinMode (pinS1,INPUT);
  digitalWrite(pinS1, HIGH);                // Pour allumer la resistance de pull up interne
  pinMode (pinS2,INPUT);
  digitalWrite(pinS2, HIGH);                // Pour allumer la resistance de pull up interne
  pinMode (pinSelect,INPUT);

  attachInterrupt(pinS1, isr_S1, FALLING);   // Appeler la routine isr_S1 quand la pinS1 descend
  attachInterrupt(pinS2, isr_S2, FALLING);   // Appeler la routine isr_S2 quand la pinS2 descend
  
  Serial.begin (115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(pinSelect) == 0) { // si on appuie sur le bouton
    delay(1);
    Serial.println("click ");
    compteur =0;
    }

  if(compteur != ancien_compteur){      // si le compteur a changé :
    Serial.println(compteur);             // on l'affiche
    ancien_compteur = compteur;
  }

  P.print(String(compteur));
}

void isr_S1(){                                             // PinS1 est passé à 0
  delay(1);                                                // delai anti-rebond
  if(digitalRead(pinS1) == LOW){                           // PinS1 toujours à 0 ?
    if(digitalRead(pinS2) == HIGH && halfright == false){  // -->
      halfright = true;                                    // on avance d'un demi tour à droite
    }  
    if(digitalRead(pinS2) == LOW && halfleft == true){     // <--
      halfleft = false;                                    // on avance d'un demi tour en plus
      compteur--;                                          // donc un tour dans le sens contraire des aiguilles
    }
  }
}
void isr_S2(){                                            // PinS2 est passé à zéro
  delay(1);                                               // delai anti-rebond
  if(digitalRead(pinS2) == LOW){                          // pinS2 toujors à zéro ?
    if(digitalRead(pinS1) == HIGH && halfleft == false){  // <--
      halfleft = true;                                    // on avance d'un demi tour à gauche
    }                                                     
    if(digitalRead(pinS1) == LOW && halfright == true){   // -->
      halfright = false;                                  // et un tour complet dans le sens des aiguilles
      compteur++;
    }
  }
}
