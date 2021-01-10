// Pour l'afficheur
#include <MD_Parola.h>
#include <MD_MAX72xx.h>

// Décommenter une des 2 lignes suivante pour tester quelques effets
// #define AFFICHAGE2
// #define AFFICHAGE3

// Définition de l'afficheur: nombre d'afficheur et broches nécessaires
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// Connection SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

void setup() {
  P.begin();
  // Décommenter les 2 lignes suivante pour inverser l'affichage
  // P.setZoneEffect(0, true, PA_FLIP_UD);
  // P.setZoneEffect(0, true, PA_FLIP_LR);

#if !defined(AFFICHAGE2) && !defined(AFFICHAGE3)
  P.displayText("Test!", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
#endif
#if defined(AFFICHAGE2)
  P.displayText("Test!", PA_CENTER, 25, 2000, PA_SCROLL_LEFT, PA_SCROLL_RIGHT);
#endif
#if defined(AFFICHAGE3)
  P.displayText("Test!", PA_CENTER, 25, 2000, PA_SCROLL_LEFT, PA_SCROLL_UP);
#endif
}

void loop() {
  P.displayAnimate();
}