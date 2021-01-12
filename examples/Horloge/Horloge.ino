#include <STM32RTC.h>

/* Get the rtc object */
STM32RTC& rtc = STM32RTC::getInstance();

/* Change these values to set the current initial time */
const byte seconds = 0;
const byte minutes = 0;
const byte hours = 16;

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
uint16_t scrollPause = 2000; // in milliseconds
#define  BUF_SIZE  75
char Message[BUF_SIZE] = { "" };

uint8_t semiC[] = { 2, 54, 54 }; // pour redefinir le ':'

// Definition des numeros de pins du bouton rotatif
#define pinS1 6
#define pinS2 7
#define pinSelect 8

int bouton, ancien_bouton = 0;
volatile bool choix_minute = true;

volatile int compteur = 0;
int ancien_compteur = 0;
unsigned long chrono, tempo = 0;
volatile bool point_heure = false;   // pour afficher le point entre les heures

volatile bool halfleft = false;      // pour compter les demi pas
volatile bool halfright = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  P.begin();
  P.setTextEffect(scrollEffect, scrollEffect);
  P.setZoneEffect(0, true, PA_FLIP_UD);
  P.setZoneEffect(0, true, PA_FLIP_LR);
  P.setIntensity(brightness);
  P.addChar(':', semiC);

  pinMode (pinS1, INPUT);
  digitalWrite(pinS1, HIGH);                // Pour allumer la resistance de pull up interne
  pinMode (pinS2, INPUT);
  digitalWrite(pinS2, HIGH);                // Pour allumer la resistance de pull up interne
  pinMode (pinSelect, INPUT);

  attachInterrupt(pinS1, isr_S1, FALLING);   // Appeler la routine isr_S1 quand la pinS1 descend
  attachInterrupt(pinS2, isr_S2, FALLING);   // Appeler la routine isr_S2 quand la pinS2 descend

  rtc.begin(); // initialize RTC 24H format

  // Set the time
  rtc.setClockSource(STM32RTC::LSE_CLOCK);
  rtc.setHours(hours);
  rtc.setMinutes(minutes);
  rtc.setSeconds(seconds);
}

void loop() {
  int heures, minutes;
  bouton = digitalRead(pinSelect);
  heures = rtc.getHours();
  minutes = rtc.getMinutes();

  if (bouton == 0) { // si on appuie sur le bouton
    Serial.println("bouton");
    if (ancien_bouton == 0) {
      ancien_bouton = bouton;
      Serial.println("click");
      delay(500);
      choix_minute = !choix_minute;
      if (choix_minute == true) {
        compteur = heures;
      } else {
        compteur = minutes;
      }
      Serial.print("choix minute =");
      Serial.println(choix_minute);
      compteur = 0;
    }
    ancien_bouton = bouton;
  }
  else  if (compteur != ancien_compteur) {     // si le compteur a changé
    ancien_compteur = compteur;
    if (choix_minute == false) {
      compteur = heures + compteur;
    } else {
      compteur = minutes + compteur;
    }
    Serial.println(compteur);             // on l'affiche
    if (choix_minute == true) { // gestion des depassement de minutes
      if ((compteur > 60) ||  (compteur < 0)) {
        rtc.setMinutes(0);
      } else {
        rtc.setMinutes(compteur);
      }
    } else { // gestion des dépassements des heures
      if ((compteur > 24) || (compteur < 0)) {
        rtc.setHours(0);
      } else {
        rtc.setHours(compteur);
      }
    }
    compteur = 0;
  }

  chrono = millis(); // on recupere le temps depuis lequel le F3 tourne
  if (chrono - tempo > 500) { // si une demi seconde est passée
    tempo = chrono;       // on realigne la tempo sur le temps du F3
    point_heure = !point_heure;
  }

  //  if (heures < 10) {
  //    sprintf (Message, "0%d", heures); // pour afficher le 0 devant les heures <10
  //  } else {
  //    sprintf (Message, "%d", heures);
  //  }
  //
  //  if (point_heure == true) { // pour faire clignoter le ':' entre les heures et les minutes
  //    strcat(Message, ":");
  //  } else {
  //    strcat(Message, " ");
  //  }
  //
  //  if (minutes < 10) {
  //    snprintf (Message, BUF_SIZE, "%s0%d", Message, minutes); // pour afficher un 0 devant les minutes si elles sont <10
  //  } else {
  //    snprintf (Message, BUF_SIZE, "%s%d", Message, minutes);
  //  }
  // La ligne suivante remplace toutes les lignes commentées ci-dessus
  sprintf(Message, "%s%d%s%s%d", (heures < 10) ? "0" : "", heures, (point_heure) ? ":" : " ", (minutes < 10) ? "0" : "", minutes);

  P.displayText(Message, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  P.displayAnimate();
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