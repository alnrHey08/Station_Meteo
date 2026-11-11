// =====================================================================
//  PLUVIOMÈTRE À AUGETS : quantité et intensité des précipitations
//  Capteur : contact à lame souple (1 impulsion par bascule de l'auget)
//  Branchement : un fil sur D3, l'autre sur GND
// =====================================================================

// ---------------------------- RÉGLAGES -------------------------------
const byte PIN_PLUIE = 3;                    // Broche D3 (interruption externe INT1)

// Hauteur de pluie correspondant à UNE bascule de l'auget (en millimètres).
// 0.2794 mm est la valeur des pluviomètres courants (type SparkFun).
// À VÉRIFIER dans la fiche technique de TON pluviomètre.
const float MM_PAR_BASCULE = 0.2794;

// Durée de la fenêtre pour calculer l'intensité (en ms). 60 000 ms = 1 minute.
const unsigned long FENETRE_MS = 60000;

// Anti-rebond : temps minimal entre deux bascules valides (en microsecondes).
// 50 000 µs = 50 ms. Un auget met bien plus de temps que ça à rebasculer.
const unsigned long ANTI_REBOND_US = 50000;
// ---------------------------------------------------------------------

volatile unsigned int bascules = 0;          // Bascules dans la fenêtre en cours
volatile unsigned long dernierInstantUs = 0; // Instant de la dernière bascule valide

unsigned long debutFenetre = 0;
unsigned long totalBascules = 0;             // Bascules depuis le démarrage (cumul)

// Appelée AUTOMATIQUEMENT à chaque bascule de l'auget
void compterBascule() {
  unsigned long maintenant = micros();
  if (maintenant - dernierInstantUs >= ANTI_REBOND_US) {
    bascules++;
    dernierInstantUs = maintenant;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_PLUIE, INPUT_PULLUP);          // Résistance interne vers 5 V
  attachInterrupt(digitalPinToInterrupt(PIN_PLUIE), compterBascule, FALLING);
  debutFenetre = millis();
  Serial.println("Pluviometre pret. Point toutes les 60 secondes.");
}

void loop() {
  unsigned long maintenant = millis();

  if (maintenant - debutFenetre >= FENETRE_MS) {

    // Copie sécurisée du compteur (interruptions coupées un instant)
    noInterrupts();
    unsigned int n = bascules;
    bascules = 0;
    interrupts();

    float duree_h = (maintenant - debutFenetre) / 3600000.0; // durée en heures
    debutFenetre = maintenant;

    totalBascules += n;

    // Pluie tombée pendant cette fenêtre (mm)
    float pluieFenetre_mm = n * MM_PAR_BASCULE;

    // Intensité = pluie tombée / durée en heures (mm/h)
    float intensite_mmh = pluieFenetre_mm / duree_h;

    // Cumul total depuis le démarrage (mm)
    float cumul_mm = totalBascules * MM_PAR_BASCULE;

    Serial.print("Bascules: ");
    Serial.print(n);
    Serial.print("  Intensite: ");
    Serial.print(intensite_mmh, 1);
    Serial.print(" mm/h  Cumul: ");
    Serial.print(cumul_mm, 2);
    Serial.println(" mm");
  }
}