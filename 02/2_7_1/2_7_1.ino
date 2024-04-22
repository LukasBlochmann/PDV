// setzen der Pins für den User Input
const int dip_led_count = 6, dip_invert = 7, button_direction = 10;

// "Speichervariablen" für User-Input-Zustände
int invert; // Invertiert bedeutet, dass alle LEDs an sind und eine "nicht leuchtende" wandert, bei nicht invertiert wandert eine ageschaltete und alle andern sind aus
int ledCount;
int dir;
int lastButtonState = LOW;
int buttonState;
int invertState;
int lastInvertState;
int ledCountState;
int ledLastCountState;

//setzen der Pins für die Output LEDs
int ledPins[] ={14, 15, 16, 17, 18, 19, 8, 9};


// Variablen um Delay-Funktion zu umgehen
int now, lasttimeact;
int intervall = 500;

// Array um dynamisch auf LED-Zustände eingehen zu können
int states[] = {0, 1};

// Variablen um die LED-Position zwischenzuspeichern
int pos = 0;
int sec_led;
int i = 0;

// Variablen um LED-Positions- und Anzahlszustände zu prüfen
bool boarder_case = true;
bool two_on = false;

// Setzt die aktuelle LED (je nach invert Zustand) auf HIGH / LOW
void move_led(){
  // Wenn die LED-Anzeige invertiert ist wird die Positions-LED ausgeschaltet, sonst an
  digitalWrite(ledPins[pos], (invert == 1 ? states[0] : states[1]));
  // Wenn 2 LEDs leuchten sollen, wird die IF ausgeführt
  if(ledCount != 1){
    // Um zwei LEDs leuchten zu lassen, wird die LED unmittelbar vor der aktiven LED ebenfalls angeschaltet
    digitalWrite(ledPins[pos + dir] , (invert == 1 ? states[0] : states[1]));
  }
}

void check_input_states(){
  // Prüft, ob 1 oder 2 LEDs leuchten sollen
  ledCount = (digitalRead(dip_led_count) == HIGH) ? 0 : 1;
  // Prüft, ob leuchtende oder nicht-leuchtende LEDS wandern sollen
  invert = (digitalRead(dip_invert) == HIGH) ? 1 : 0;
  // Gibt an, in welche Richtung die LEDs wandern sollen
  dir = (digitalRead(button_direction) == LOW) ? 1 : -1;


  // Speichert die aktuellen Inputwerte in Variablen (siehe function "instant_input_reaction()")
  buttonState = (dir == -1) ? HIGH : LOW;
  invertState = (invert == 1) ? HIGH : LOW;
  ledCountState = (ledCount == 0) ? HIGH : LOW;
}

// je nachdem, ob leuchtende oder nicht leuchtende LEDs wandern werden vor dem setzen einer neuen Position alle LEDs an / aus geschaltet
void clear_field(){
    for(int l = 0; l <= 7; l++){
      digitalWrite(ledPins[l], (invert == 1 ? states[1] : states[0]));
    }
}

void calc_pos(){

  /* Die Variable i ist ein "Puffer" an dem durch Modulorechnung die aktuelle ArrayPosition berechnet wird
     damit i nicht zu groß wird und so zu Fehlern (z.b.: Overflow) führt, wird sie bei erreichen des Wertes
    0 oder 160 zurückgesetzt, da beide Werte %8 0 ergeben.
    zu welchem Wert sie angepasst wird, ist abhängig von der aktuellen Direction--> siehe Zeile mit ***-Markierung
  */

  i += dir;
  i = (i == 160 && dir == 1) ? 0 : ((i == 0 && dir == -1) ? 160 : i); //***
  // Es muss nochmal überprüft werden, ob der Knopfgedrückt wurde (wenn ja wird die Position sofort um 1 verringert/vergrößert)
  // da die LED sonst ein Intervall zulange stehen bleibt
  pos = (buttonState != lastButtonState) ? (pos += dir) : i % 8;

/* um einen dynamischen Übergang zu generieren wenn zwei LEDs laufen sollen, wird in Randfällen 
   die Position um einen Index (-1 ; 8) zu weit versetzt, dann wird der Boardercase auf false gesetzt 
   um eine Endlosschleife zu vermieden (sonst würde pos immer wieder von z.B. -1 auf 0 gesetzt und
   wieder in die IF Abfrage "rutschen")
*/
  if(ledCount != 1 && ((pos == 0 && dir == 1) || (pos == 7 && dir == -1)) && boarder_case == true){
    pos -= dir;
    i -= dir;
    boarder_case = false;
  }

  // boarder_case erst wieder auf true setzen, wenn pos es mit dem Wert 0 oder 7 mindestens einal durch die vorherige 
  // IF-Abfrage "geschafft" hat
  if(pos != -1 && pos != 8){
    boarder_case = true;
  }
}

// Da die alten und aktuellen Inputwerte zwischengespeichert wurden, kann man jetzt prüfen, ob der Knopf gerade gedrückt wurde
void instant_input_reaction(){
  if(invertState != lastInvertState){
    // Wenn der Invertknopf gedrückt wurde, werden alle LEDs (je nach neuem invert zustand) an / ausgeschaltet
    clear_field();
    // Der neue Zustand wird dem alten übergeben
    lastInvertState = invertState;
  }

  if(ledCountState != ledLastCountState){
    // Boolean anpassen, ob 2 odder 1 LED leuchtet
    two_on = !two_on;

    // Die 2te leuchtende LED wird bei dir == 1 den nächstgrößeren, sonst den nächstkleineren Wert haben
    sec_led = (dir == 1) ? (pos+1) : (pos-1);
    // Wenn mit dem Knopfdruck 2 angeschalteet wurden...
    if(two_on == true){
      //... dann wird die nächste LED angeschaltet
      digitalWrite(ledPins[sec_led], (invert == 1 ? states[0] : states[1]));
    }else{
      // sonst ausgeschaltet
      digitalWrite(ledPins[sec_led], (invert == 1 ? states[1] : states[0]));
    }
    ledLastCountState = ledCountState;
  }
}


void setup(){
  // LED Pins als output konfigurieren
  for(int i = 0; i < 8; i++){
    pinMode(ledPins[i], OUTPUT);
  }
  
  // User input pins als input konfigurieren
  pinMode(dip_led_count, INPUT);
  pinMode(dip_invert, INPUT);
  pinMode(button_direction, INPUT);

  // Dip-Schalter prüfen, ob gerade 2 oder 1 LED laufen sollen (siehe "instant_input_reaction: if(ledCountState....)")
  two_on = (digitalRead(dip_led_count) == HIGH) ? true : false;
}

void loop(){

  // Zuerst prüfen, ob der User einen Knopf gedrückt hat
  check_input_states();

  // aktuelle Zeit um LEDs im richtigen Intervall laufen zu lassen
  now = millis();

  // Falls ein Knopf gedrückt wurde, wird die jeweilige Aktion sofort, ohne Anpassung an den "normalen" LED-Lauf-Takt, ausgeführt
  instant_input_reaction();

  // IF ausführen wenn: ((die aktuelle Zeit) - (der letzte Zeitpunkt, an dem eine LED verrückt wurde) >= (intervall)), oder wenn der "direction(dir)-change"-button gedrückt wurde 
  if(now - lasttimeact >= intervall || buttonState != lastButtonState){  

    // Alte Zustände "löschen"
    clear_field();

    // Neue Postition für aktive LED berechnen
    calc_pos();

    // neue Zeit und neuen Buttonzustand setzen
    lasttimeact = millis();
    lastButtonState = buttonState;
  }

  // Die LED mit allen neuen Werten bewegen
  move_led();
}