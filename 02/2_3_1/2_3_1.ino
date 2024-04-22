//Anfangswerte initialisieren
void setup() {
  // serielle Kommunikation starten, 9600 Bit/s
  Serial.begin(9600);
}


//Endlosschleife, Haupteinstiegspunkt des Programms
void loop() {
  //Text alle 500ms auf seriellen Monitor drucken
  Serial.println("Hello STTS Arduino World");
  delay(500);
}
