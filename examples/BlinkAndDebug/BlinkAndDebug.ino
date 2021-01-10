// Déclaration un compteur qui démarre à 0
int i = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  // Initialisation de la ligne série pour l'affichage sur le PC
  Serial.begin(115200);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  // Envoie du texte "ON" sur la console
  Serial.print("ON ");
  // Envoie de la valeur du compteur i sur la console
  Serial.print(i);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  // Envoie le texte "OFF" sur la console et passage à la ligne (commande println)
  Serial.println("  OFF ");
  delay(1000);                       // wait for a second
  // On ajoute 1 au compteur
  i++;
}
