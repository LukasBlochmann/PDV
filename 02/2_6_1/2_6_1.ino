// Der Button wird an Pin Nr. 10 angeschlossen, die LED an Nr.13
const int buttonPin = 10;
const int LED = 13;

//die LED soll für 500ms an sein [1Hz = 1 Schwingung/s --> 500ms_LOW + 500ms_HIGH = 1 sec. Schwingungsdauer]
const long blinkTime = 500;
// da die aktuelle Uhrzeit als Messwert genommen wird, muss sie immerwieder neu afgenommen werden
unsigned long lastBlinkTime = 0;
// letzten LED Zustand abspeichern um alternieren (blinken) zu generieren
bool ledState = false;

// Anfangswerte initialisieren
void setup() {
  // Der Button sendet signale an den Arduino (in Form von Spannungsdurchlass) --> INPUT
  pinMode(buttonPin, INPUT);
  // Die LED bekommt Signale (Spannung) vom Arduino --> OUTPUT
  pinMode(LED, OUTPUT);
}

// Endlosschleife
void loop() {

  // Zwischenspeichern der aktuellen Zeit, um damit das 500 sec. Intervall zu berechnen
  unsigned long now = millis();

  // WENN der Button gedrückt wird
  if(digitalRead(buttonPin) == HIGH){
    // Formel, um zu berechnen ob das Intervall bereits durchgelaufen ist
    if(now - lastBlinkTime >= blinkTime){
      // Startzeitpunkt für neues Intervall resetten
      lastBlinkTime = now;
      // LED Zustand alternieren (blinken)
      ledState = !ledState;
      // je nach aktueller Phase (an oder aus) wird die LED mit Spannung versorgt, oder eben nicht
      digitalWrite(LED, ledState);
    }
  }else{
    // Wenn der Knopf nicht gedrückt wird, ist die LED aus
    digitalWrite(LED, LOW);
  }
}
