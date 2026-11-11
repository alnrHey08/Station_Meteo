// =====================================================================
//  PYRANOMÈTRE : rayonnement solaire (W/m²)
//  Capteur : sortie analogique en TENSION, proportionnelle au rayonnement
//  Branchement : VCC -> 5 V, GND -> GND, signal -> A1
// =====================================================================

// ---------------------------- RÉGLAGES -------------------------------
const byte PIN_PYRANO = A1;                  // Entrée analogique du pyranomètre

// Tension de référence du convertisseur analogique-numérique (en volts).
// 5.0 sur l'Arduino Uno alimenté en 5 V, 3.3 sur la station alimentée en 3,3 V.
const float V_REF = 5.0;

// Sensibilité du capteur : volts de sortie par W/m² reçu.
// ATTENTION : valeur d'EXEMPLE (sortie 0-5 V pour 0-1800 W/m² => 5/1800).
// À REMPLACER par la valeur de la fiche technique de TON pyranomètre.
const float SENSIBILITE_V_PAR_WM2 = 5.0 / 1800.0;

// Tension de sortie dans le noir complet (décalage / offset), en volts.
// Souvent 0 V. Pour le mesurer : couvre le capteur et lis la tension affichée.
const float V_OFFSET = 0.0;
// ---------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  Serial.println("Pyranometre pret.");
}

void loop() {
  // Moyenne de 16 lectures pour réduire le bruit
  long somme = 0;
  for (int i = 0; i < 16; i++) {
    somme += analogRead(PIN_PYRANO);
    delay(2);
  }
  float lectureMoyenne = somme / 16.0;

  // Conversion de la valeur du CAN (0-1023) en tension (volts)
  float tension = lectureMoyenne * V_REF / 1023.0;

  // Conversion de la tension en rayonnement (W/m²)
  float rayonnement = (tension - V_OFFSET) / SENSIBILITE_V_PAR_WM2;

  // Le rayonnement ne peut pas être négatif : on corrige les petites dérives
  if (rayonnement < 0) rayonnement = 0;

  Serial.print("CAN: ");
  Serial.print(lectureMoyenne, 1);
  Serial.print("  Tension: ");
  Serial.print(tension, 3);
  Serial.print(" V  Rayonnement: ");
  Serial.print(rayonnement, 0);
  Serial.println(" W/m2");

  delay(1000);
}