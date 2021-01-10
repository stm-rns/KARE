//
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
  Serial.println(lumiere);

  // Attente
  delay(pollingInterval);
}