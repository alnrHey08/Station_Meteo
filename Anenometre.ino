// =====================================================================
//  ANÉMOMÈTRE : mesure de la vitesse du vent
//  Capteur : contact à lame souple (1 impulsion par tour)
//  Branchement : un fil sur D2, l'autre sur GND
// =====================================================================

// ---------------------------- RÉGLAGES -------------------------------
const byte PIN_ANEMO = 2;                    // Broche D2 (interruption externe INT0)

// Coefficient du capteur : combien de km/h pour 1 impulsion par seconde.
// 2.4 est la valeur des anémomètres courants (type Misol / SparkFun).
// À VÉRIFIER dans la fiche technique de TON anémomètre.
const float KMH_PAR_HZ = 2.4;

// Durée pendant laquelle on compte les impulsions (en millisecondes).
// Plus la durée est longue, plus la mesure est stable (mais moins réactive).
const unsigned long FENETRE_MS = 3000;

// Temps minimal entre deux impulsions valides (en microsecondes).
// Sert à ignorer les "rebonds" du contact mécanique.
// 10 000 µs = 10 ms, ce qui permet de mesurer jusqu'à environ 240 km/h.
const unsigned long ANTI_REBOND_US = 10000;
// ---------------------------------------------------------------------

// "volatile" est OBLIGATOIRE : ces variables sont modifiées dans une
// interruption, le compilateur ne doit pas les optimiser.
volatile unsigned int impulsions = 0;        // Nombre d'impulsions comptées
volatile unsigned long dernierInstantUs = 0; // Instant de la dernière impulsion valide

unsigned long debutFenetre = 0;              // Instant de début de la fenêtre de mesure

// Cette fonction est appelée AUTOMATIQUEMENT à chaque impulsion
// (front descendant sur D2 = fermeture du contact).
void compterImpulsion() {
  unsigned long maintenant = micros();
  // On ne compte l'impulsion que si assez de temps s'est écoulé depuis la précédente
  if (maintenant - dernierInstantUs >= ANTI_REBOND_US) {
    impulsions++;
    dernierInstantUs = maintenant;
  }
}

void setup() {
  Serial.begin(9600);                        // Liaison série vers le PC

  // INPUT_PULLUP : active la résistance interne qui maintient la broche à 5 V
  // quand le contact est ouvert. Quand le contact se ferme, la broche passe à 0 V.
  pinMode(PIN_ANEMO, INPUT_PULLUP);

  // On demande à l'Arduino d'exécuter compterImpulsion() à chaque front descendant
  attachInterrupt(digitalPinToInterrupt(PIN_ANEMO), compterImpulsion, FALLING);

  debutFenetre = millis();
  Serial.println("Anemometre pret. Mesure toutes les 3 secondes.");
}

void loop() {
  unsigned long maintenant = millis();

  // La fenêtre de mesure est-elle terminée ?
  if (maintenant - debutFenetre >= FENETRE_MS) {

    // On copie le compteur en désactivant brièvement les interruptions,
    // pour éviter qu'une impulsion arrive pendant la copie.
    noInterrupts();
    unsigned int n = impulsions;
    impulsions = 0;                          // Remise à zéro pour la fenêtre suivante
    interrupts();

    // Durée réelle de la fenêtre, en secondes
    float duree_s = (maintenant - debutFenetre) / 1000.0;
    debutFenetre = maintenant;

    // Fréquence = impulsions par seconde (en Hz)
    float frequence = n / duree_s;

    // Conversion en vitesse du vent
    float vitesse_kmh = frequence * KMH_PAR_HZ;
    float vitesse_ms  = vitesse_kmh / 3.6;   // 1 m/s = 3,6 km/h

    Serial.print("Impulsions: ");
    Serial.print(n);
    Serial.print("  Vitesse: ");
    Serial.print(vitesse_kmh, 1);
    Serial.print(" km/h  (");
    Serial.print(vitesse_ms, 1);
    Serial.println(" m/s)");
  }
}
