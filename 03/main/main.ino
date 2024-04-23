#include "pdv_03_lib.h"



// RED: 3, GREEN: 4,, YELLOW: 5, BLUE: 6

// 1: 0 1 0 1
// 2: 0 1 1 0
// 3: 1 0 1 0
// 4: 1 0 0 1

const int RED = 4, GREEN = 5, YELLOW = 6, BLUE = 7;
int steps_to_go = 50;

Motor motor = Motor(200, BLUE, YELLOW, GREEN, RED);

void setup(){
  Serial.begin(9600);
  motor.speed(30);
}

void loop(){


  motor.now = millis();
  if(motor.calibrated == false || ((motor.now - motor.last_time >= 3000) && motor.calibrate_repeat())){
    
    if(motor.calibrated == false){
      motor.calibrate();
    }

  }

  motor.check_buttons();

  if(motor.deadman_pin_check == true){
    if(motor.down_pin_check == false && motor.standard_position != 50
      && (((motor.up_pin_check == true) && motor.standard_position == 0) || motor.running_dir == 1)){          
      motor.step(1);
      motor.standard_position += 1;
      motor.running_dir = 1;
    }else if(motor.standard_position != 0 && (motor.down_pin_check == true || motor.running_dir == 0)){
      motor.step(-1);
      motor.standard_position -= 1;
      motor.running_dir = 0;
    }
  }


 




}