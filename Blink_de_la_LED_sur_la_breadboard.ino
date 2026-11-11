// Clignotement d'une LED externe branchée sur la broche 8

const int brocheLED = 8;           // Numéro de la broche où est branchée la LED

void setup() {
  pinMode(brocheLED, OUTPUT);      // La broche 8 est une sortie
}

void loop() {
  digitalWrite(brocheLED, HIGH);   // Allume la LED (5 V sur la broche 8)
  delay(1000);                     // Attend 1 seconde
  digitalWrite(brocheLED, LOW);    // Éteint la LED (0 V)
  delay(1000);                     // Attend 1 seconde
}