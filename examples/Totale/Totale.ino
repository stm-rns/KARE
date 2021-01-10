#include <SPI.h>      // pour le SPI
#include <Wire.h>     // pour l'I2C
#include <HTS221.h>   // pour le capteur humidité et temperature
#include <LPS25H.h>   // pour le capteur pression
#include <STM32RTC.h> // pour la gestion de l'heure

// Pour l'affichage
#include <MD_Parola.h>
#include <MD_MAX72xx.h>

// pour les capteurs I2C
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// Definition des pins pour le bouton/molette
#define pinS1 6
#define pinS2 7
#define pinSelect 8

// Seuil de sensibilité pour l'acceleromètre
#define CLICKTHRESHHOLD 80
uint8_t click;

// Definition des paramètres de l'afficheur 4x8x8 leds.
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW //PAROLA_HW //FC16_HW  //PAROLA_HW, GENERIC_HW, ICSTATION_HW
#define MAX_DEVICES 4
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// paramètres d'affichage
uint8_t scrollSpeed = 50;    // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
int brightness = 1; // 0 to 9
uint16_t scrollPause = 2000; // en milliseconds
#define  BUF_SIZE  75
char Message[BUF_SIZE] = { "...." };

// redefinition de la police pour certains caractères
uint8_t deux_point[] = { 2, 54, 54 };         // changement du caractere ':' en "::". 2 colonnes
uint8_t degC[] = { 6, 3, 3, 56, 68, 68, 68 }; // Degre Celcius
uint8_t zero[] = { 5, 62, 65, 65, 65, 62 };   // zero
uint8_t mini_b[] = { 3, 124, 84, 40 };  // b pour milibar

double humidity, temperature;

/* creation de l'objet pour l'heure  */
STM32RTC& rtc = STM32RTC::getInstance();

const byte seconds = 0;
const byte hours = 15;
int heures;
int minutes = 43;

int humidite;
int lumiere;
int pression;

unsigned int pollingInterval = 1000; // in milliseconds pour sensibilité accelerometre

int bouton, ancien_bouton = 0;
bool choix_minute = true;

volatile int molette = 0;
int ancien_molette = molette;
int compteur = 0;

volatile bool halfleft = false;      // pour compter les demi pas
volatile bool halfright = false;
volatile bool clignotte = false;

// définition des constantes pour les menus
#define menu_heure            1
#define menu_reglage_minute   11
#define menu_reglage_heure    12
#define menu_temperature      2
#define menu_barometre        3
#define menu_humidite         4
#define menu_lumiere          5
#define menu_accelerometre    6
#define menu_click            7

int menu = menu_heure;

// Fonction qui sera executée une fois seulement au démarrage
// Utilisé pour initialiser les périphériques
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  P.begin(0);
  P.setZone(0, 0, MAX_DEVICES);
  P.setZoneEffect(0, true, PA_FLIP_UD);
  P.setZoneEffect(0, true, PA_FLIP_LR);

  P.setTextEffect(scrollEffect, scrollEffect);
  P.setIntensity(brightness);
  P.addChar(':', deux_point);  // changement de police pour ':'
  P.addChar('$', degC);        // changement de police pour '$'
  P.addChar('0', zero);        // changement de police pour '0'
  P.addChar('&', mini_b);      // changement de police pour '0'

  Wire.begin();
  if (!smeHumidity.begin()) {
    while (1) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
    }
  }
  if (!smePressure.begin()) {
    while (1) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
    }
  }

  if (!lis.begin(0x18)) {
    while (1) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(2000);
      digitalWrite(LED_BUILTIN, LOW);
      delay(2000);
    }
  }
  lis.setRange(LIS3DH_RANGE_2_G);   // 2, 4, 8 or 16 G!

  Serial.print("Range = "); Serial.print(2 << lis.getRange());
  Serial.println("G");

  // 0 = turn off click detection & interrupt
  // 1 = single click only interrupt output
  // 2 = double click only interrupt output, detect single click
  // Adjust threshhold, higher numbers are less sensitive
  lis.setClick(2, CLICKTHRESHHOLD);
  delay(pollingInterval);

  // parametres pour la molette
  pinMode (pinS1, INPUT);
  digitalWrite(pinS1, HIGH);                // Pour allumer la resistance de pull up interne
  pinMode (pinS2, INPUT);
  digitalWrite(pinS2, HIGH);                // Pour allumer la resistance de pull up interne
  pinMode (pinSelect, INPUT);

  attachInterrupt(pinS1, isr_S1, FALLING);   // Appeler la routine isr_S1 quand la pinS1 descend
  attachInterrupt(pinS2, isr_S2, FALLING);   // Appeler la routine isr_S2 quand la pinS2 descend
  //  parametres de l'interruption : attachInterrupt(digitalPinToInterrupt(pin), ISR, mode); // option : LOW, CHANGE, RISING, FALLING, HIGH

  rtc.begin(); // initialize RTC 24H format

  // Set the time
  rtc.setClockSource(STM32RTC::LSE_CLOCK);
  rtc.setHours(hours);
  rtc.setMinutes(minutes);
  rtc.setSeconds(seconds);

  pression = smePressure.readPressure();
  Serial.print("pression :");
  Serial.println(pression);
}

void loop() {
  // boucle principale qui sera executée indéfiniement
  bouton = digitalRead(pinSelect);
  if ((millis() % 1000) > 500) {
    clignotte = false;
  } else {
    clignotte = true;
  }

  P.displayAnimate();
  if (bouton == LOW) { // si bouton appuyé
    Serial.println("bouton");
    if (ancien_bouton == LOW) {
      ancien_bouton = bouton;
      Serial.printf("%i click\n", menu);
      delay(500);
      switch (menu) {
        case menu_heure:
          choix_minute = true;
          menu = menu_reglage_minute;
          break;
        case menu_reglage_minute :
          choix_minute = false;
          menu = menu_reglage_heure;
          break;
        case menu_reglage_heure :
          menu = menu_heure;
          break;
        case menu_temperature:
        case menu_barometre:
        case menu_humidite:
        case menu_accelerometre:
        case menu_click:
        case menu_lumiere:
          break;
        default:
          menu = menu_heure;
          molette = 0;
          Serial.printf("menu=%i\nmenu default\n", menu);
          break;
      } // fin du switch
      ancien_bouton = bouton;
    } // fin de l'appui bouton
  } // fin bouton appuyé
  else if (molette != ancien_molette) { // si la molette a tourné
    ancien_molette = molette;
    switch (menu) {
      case menu_reglage_minute :
        reglage_heure();
        break;
      case menu_reglage_heure :
        reglage_heure();
        break;
      case menu_heure:
        menu = menu_temperature;
        break;
      case menu_temperature:
        menu = menu_humidite;
        break;
      case menu_humidite:
        menu = menu_barometre;
        break;
      case menu_barometre:
        menu = menu_lumiere;
        break;
      case menu_lumiere:
        menu = menu_accelerometre;
        break;
      case menu_accelerometre:
        menu = menu_click;
        break;
      case menu_click:
        menu = menu_heure;
        break;
      default:
        molette = 0;
        Serial.printf("menu=%i\ndefault\n", menu);
        menu = menu_heure;
        break;
    } // fin du switch
  }   // end if molette tournee

  // Affichage -----------------------------------------
  switch (menu) {
    case menu_heure:
      heures = rtc.getHours();
      minutes = rtc.getMinutes();
      sprintf(Message, "%s%d%s%s%d", (heures < 10) ? "0" : "", heures, (clignotte) ? ":" : " ", (minutes < 10) ? "0" : "", minutes);
      break;
    case menu_reglage_minute :
    case menu_reglage_heure :
      heures = rtc.getHours();
      minutes = rtc.getMinutes();
      if (clignotte == true) {
        sprintf(Message, "%s%d:%s%d", (heures < 10) ? "0" : "", heures, (minutes < 10) ? "0" : "", minutes);
      }
      else {
        if (choix_minute == true) {
          sprintf(Message, "%s%d:    ", (heures < 10) ? "0" : "", heures);
        }
        else {
          sprintf(Message, "    :%s%d", (minutes < 10) ? "0" : "", minutes);
        }
      }
      break;
    case menu_temperature:
      temperature = smeHumidity.readTemperature();
      sprintf (Message, "%d C", (int)temperature);
      break;
    case menu_humidite:
      humidite = smeHumidity.readHumidity();
      sprintf (Message, "%d %%", (int)humidite);
      break;
    case menu_barometre:
      if ((millis() % 1000) > 1900) {
        pression = smePressure.readPressure(); // une lecture que toutes les demi secondes
      }
      sprintf (Message, "%dmb", (int)pression);
      break;
    case menu_lumiere:
      if ((millis() % 2000) > 1900) {
        lumiere = analogRead(A1); // une lecture que toutes les demi secondes
      }
      sprintf (Message, "%d lux", (int)lumiere);
      break;
    case menu_accelerometre:
      if ((millis() % 500) > 490) {
        lis.read();      // get X Y and Z data at once
        // Then print out the raw data
        Serial.printf("X: %i\tY: %i\tZ: %i", lis.x, lis.y, lis.z);

        /* Or....get a new sensor event, normalized */
        sensors_event_t event;
        lis.getEvent(&event);

        /* Display the results (acceleration is measured in m/s^2) */
        Serial.printf("\t\tX: %i\tY: %i\tZ: %i m/s^2\n\n", event.acceleration.x, event.acceleration.y, event.acceleration.z);
        sprintf (Message, "x%d", (int)lis.x);
      }
      break;
    case menu_click:
      click = lis.getClick();
      compteur++;
      if (compteur < 10) {
        sprintf (Message, "toc ?");
      }
      if (compteur > 5000) {
        sprintf (Message, "toc ?");
        compteur = 0;
      } else {
        if (click & 0x30) {
          Serial.printf("Click detected (0x%x): ", click);
          if (click & 0x10) {
            Serial.println("single click");
            sprintf (Message, "click");
            compteur = 0;
          }
          if (click & 0x20) {
            Serial.println("double click");
            sprintf (Message, "2click");
            compteur = 0;
          }
        }
      }
      break;
    default:
      //molette=0;
      sprintf (Message, "erreur");
      Serial.printf("defaut. menu =%i", menu);
      break;
  } // fin du switch
  P.displayText(Message, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);  //{ PA_SCROLL_UP, "SC_U", 5, 1 },{ PA_SCROLL_DOWN, "SC_D", 5, 1 }
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
      molette--;                                          // donc un tour dans le sens contraire des aiguilles
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
      molette++;
    }
  }
}

void reglage_heure()
{
  heures = rtc.getHours();
  minutes = rtc.getMinutes();

  if (choix_minute == false) {
    molette = heures + molette;
  } else {
    molette = minutes + molette;
  }
  if (choix_minute == true) {
    if ((molette > 60) || (molette < 0)) {
      rtc.setMinutes(0);
    } else {
      rtc.setMinutes(molette);
    }
  } else {
    if ((molette > 24) || (molette < 0)) {
      rtc.setHours(0);
    } else {
      rtc.setHours(molette);
    }
  }
  molette = 0;
}