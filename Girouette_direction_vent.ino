// =====================================================================
//  GIROUETTE : direction du vent (16 positions)
//  Capteur : réseau de résistances sélectionnées par un aimant
//  Branchement : 5 V -> résistance 10 kOhm -> A0 -> girouette -> GND
// =====================================================================

// ---------------------------- RÉGLAGES -------------------------------
const byte PIN_GIROUETTE = A0;               // Entrée analogique de la girouette

// Valeur de la résistance fixe de 10 kOhm (en ohms)
const float R_FIXE = 10000.0;

// Tolérance : écart maximal (en unités du CAN) entre la mesure et la valeur
// attendue pour accepter la direction. Au-delà, on signale une erreur.
const int TOLERANCE = 30;
// ---------------------------------------------------------------------

// Résistance de la girouette (en ohms) pour chacune des 16 directions,
// dans l'ordre : N, NNE, NE, ENE, E, ESE, SE, SSE, S, SSO, SO, OSO, O, ONO, NO, NNO.
// Ces valeurs correspondent à la girouette des kits courants (type SparkFun).
// À VÉRIFIER dans la fiche technique de TA girouette.
const float RESISTANCES[16] = {
  33000.0,  6570.0,  8200.0,   891.0,
   1000.0,   688.0,  2200.0,  1410.0,
   3900.0,  3140.0, 16000.0, 14120.0,
 120000.0, 42120.0, 64900.0, 21880.0
};

// Noms des directions, dans le même ordre que le tableau ci-dessus
const char* NOMS[16] = {
  "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
  "S", "SSO", "SO", "OSO", "O", "ONO", "NO", "NNO"
};

// Tableau qui contiendra la valeur du CAN attendue pour chaque direction
int attendu[16];

void setup() {
  Serial.begin(9600);

  // Calcul de la valeur du CAN attendue pour chaque direction.
  // Formule du diviseur de tension : V/Valim = R_girouette / (R_girouette + R_fixe)
  // Le CAN renvoie 0 à 1023, donc : valeur = 1023 * R / (R + R_fixe)
  for (int i = 0; i < 16; i++) {
    attendu[i] = (int)(1023.0 * RESISTANCES[i] / (RESISTANCES[i] + R_FIXE));
  }

  Serial.println("Girouette prete. Tourne-la pour tester.");
}

void loop() {
  // On fait la moyenne de 10 lectures pour limiter le bruit de mesure
  long somme = 0;
  for (int i = 0; i < 10; i++) {
    somme += analogRead(PIN_GIROUETTE);
    delay(2);
  }
  int mesure = somme / 10;

  // Recherche de la direction dont la valeur attendue est la plus proche
  int meilleur = 0;
  int ecartMin = 9999;
  for (int i = 0; i < 16; i++) {
    int ecart = abs(mesure - attendu[i]);    // abs() = valeur absolue
    if (ecart < ecartMin) {
      ecartMin = ecart;
      meilleur = i;
    }
  }

  Serial.print("Valeur brute: ");
  Serial.print(mesure);

  if (ecartMin <= TOLERANCE) {
    Serial.print("  Direction: ");
    Serial.print(NOMS[meilleur]);
    Serial.print("  (");
    Serial.print(meilleur * 22.5, 1);          // Angle en degrés (0 = Nord)
    Serial.println(" degres)");
  } else {
    // Mesure trop éloignée de toutes les valeurs connues
    Serial.println("  Direction: valeur hors tableau (verifier branchement ou resistances)");
  }

  delay(500);
}