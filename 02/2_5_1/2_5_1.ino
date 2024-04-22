// Der Button ist an Pin Nr. 10 angeschlossen, der DIP an Nr.3
const int Button = 10;
const int DIP = 3;

// Zustand des Tasters. 1 steht für 'gedrückt', 0 steht für 'nicht gedrückt'
int Zustand = 0;

// Anfangswerte initialisieren
void setup() {
  // Der Button & der DIP senden signale an den Arduino (in Form von Spannungsdurchlass) --> INPUT
  pinMode(Button, INPUT);
  pinMode(DIP, INPUT);

  // serielle Kommunikation mit 9600 Baud (9600 Bit/s) starten
  Serial.begin(9600);
}

// Endlosschleife
void loop() {
  // Sofern der Button HIGH ist, setzt Zustand auf 1, ansonten auf 0
  Zustand = digitalRead(Button) == HIGH ? 1 : 0;
  // Zuerst ohne Zeilenumbruch printen, damit in der gleichen Zeile weiter geprintet werden kann
  Serial.print("Button Zustand: ");
  Serial.println(Zustand);

  Zustand = digitalRead(DIP) == HIGH ? 1 : 0;
  Serial.print("DIP Zustand: ");
  Serial.println(Zustand);

  // Langsamer auslesen, um den Output auf der Konsoel übersichtlicher  zu machen
  delay(500);
}