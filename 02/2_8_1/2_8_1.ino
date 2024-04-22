// Definieren der Pin-Nummern für die DIP-Schalter und LED
const int ledPin = 10; // PWM-fähiger Pin für die LED
const int dipSwitchPins[] =  {A0, A1, A2, A3, A4, A5, 8, 9}; // Pins für die 8 DIP-Schalter  
const int numSwitches = sizeof(dipSwitchPins) / sizeof(dipSwitchPins[0]);
int i, exponent;

void setup() {
  // LED Pin als Ausgang konfigurieren
  pinMode(ledPin, OUTPUT);
  // Pins der Dip switsches als Eingang konfigurieren
  for (int i = 0; i < numSwitches; i++) {
    pinMode(dipSwitchPins[i], INPUT);
  }
}

void loop() {
  int value = 0; // Value als 0 initialisieren
  // Liest den Zustand der DIP-Schalter und berechnet den gesamten Wert. 
  for (i = 0; i < numSwitches; i++) {  // For Schleife für jeden DIP-Switch
    value = value | (digitalRead(dipSwitchPins[i]) << i); // Lese den binären Wert aus (0, 1) und shifte ihn um seine Wertigkeit nach links. Verknüpfe dies über oder mit dem value
  }
  // da 8 Bit Werte von 0 bis 255 abbilden können, und der PWM-Output einen Wert in dieser Range braucht, ist eine Map-function nicht nötig
  analogWrite(ledPin, value); // Stellt die LED-Helligkeit ein
}
