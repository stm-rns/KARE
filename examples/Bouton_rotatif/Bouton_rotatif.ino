// Pour l'afficheur
#include <MD_Parola.h>
#include <MD_MAX72xx.h>

// Définition de l'afficheur: nombre d'afficheur et broches nécessaires
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
uint16_t scrollPause = 2000; // en milliseconds
#define  BUF_SIZE  75
char Message[BUF_SIZE] = { "" };

// Broches pour le bouton
#define pinS1 6
#define pinS2 7
#define pinSelect 8

volatile int compteur = 0;
int ancien_compteur = compteur;

volatile boolean halfleft = false;      // pour compter les demi pas
volatile boolean halfright = false;


void setup() {
  Serial.begin(115200);

  P.begin();
  P.setZoneEffect(0, true, PA_FLIP_UD);
  P.setZoneEffect(0, true, PA_FLIP_LR);
  P.setTextEffect(scrollEffect, scrollEffect);
  P.setIntensity(brightness);

  pinMode (pinS1, INPUT);
  digitalWrite(pinS1, HIGH);
  pinMode (pinS2, INPUT);
  digitalWrite(pinS2, HIGH);
  pinMode (pinSelect, INPUT);

  attachInterrupt(pinS1, isr_S1, FALLING);   // Appeler la routine isr_S1 quand la pinS1 descend
  attachInterrupt(pinS2, isr_S2, FALLING);   // Appeler la routine isr_S2 quand la pinS2 descend

  Serial.begin (115200);

}

void loop() {
  if (digitalRead(pinSelect) == 0) { // si appui le bouton
    delay(1);
    Serial.println("click ");
    compteur = 0;
  }

  if (compteur != ancien_compteur) {    // si le compteur a changé
    Serial.println(compteur);           // affichage
    ancien_compteur = compteur;
  }

  P.print(String(compteur));
}

void isr_S1() {                                            // PinS1 est passé à 0
  delay(1);                                                // delai anti-rebond
  if (digitalRead(pinS1) == LOW) {                         // PinS1 toujours à 0 ?
    if (digitalRead(pinS2) == HIGH && halfright == false) { // -->
      halfright = true;                                    // on avance d'un demi tour à droite
    }
    if (digitalRead(pinS2) == LOW && halfleft == true) {   // <--
      halfleft = false;                                    // on avance d'un demi tour en plus
      compteur--;                                          // donc un tour dans le sens contraire des aiguilles
    }
  }
}
void isr_S2() {                                           // PinS2 est passé à zéro
  delay(1);                                               // delai anti-rebond
  if (digitalRead(pinS2) == LOW) {                        // pinS2 toujors à zéro ?
    if (digitalRead(pinS1) == HIGH && halfleft == false) { // <--
      halfleft = true;                                    // on avance d'un demi tour à gauche
    }
    if (digitalRead(pinS1) == LOW && halfright == true) { // -->
      halfright = false;                                  // et un tour complet dans le sens des aiguilles
      compteur++;
    }
  }
}