#include "pdv_03_lib.h"



// RED: 3, GREEN: 4,, YELLOW: 5, BLUE: 6

// 1: 0 1 0 1
// 2: 0 1 1 0
// 3: 1 0 1 0
// 4: 1 0 0 1

const int RED = A0, GREEN = A1, YELLOW = A2, BLUE = A3;
int steps_to_go = 50;

// korrekt ist 200 statt x
Motor motor = Motor(200, BLUE, YELLOW, GREEN, RED);

void setup(){
  Serial.begin(9600);
  motor.speed(30);
}

void loop(){

// fix, dass "horizontal" auf beiden seiten geht, abfrage machen, welche seite die schranke befüllen soll
// vieeeeele bugs wenn man bei confirmation abfrage mehrere buttons drückt
  if(motor.calibrated != true){
    motor.calibrate();
  }

  motor.user_interaction_deadman();
}