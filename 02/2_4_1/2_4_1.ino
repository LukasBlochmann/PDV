// Die LED ist an Pin Nr.13 des Arduino UNO angeschlossen
const int LED = 13;
// Der aktuelle Status der LED wird als true (on) initialisiert. Dieser muss global sein,
// damit er in der Loop nicht erneut initialisiert wird. 
bool status = true;

// Anfangswerte initialisieren
void setup() {
  // Der Arduino sendet Signale an die LED, also ist die LED ein OUTPUT
  pinMode(LED, OUTPUT);
}

// Endlosschleife
void loop() {
  // LED-Pin unter Spannung setzen --> LED leuchtet
  digitalWrite(LED, status);
  // status invertieren
  status = !status;
  // warten um blinkeffekt mit 1 Hz zu erzeugen
  delay(500);
}
