#include "pdv_03_lib.h"

// Names derived from cable colors
const int RED = 4, GREEN = 5, YELLOW = 6, BLUE = 7;
int steps_to_go = 50;

//Create motor based on the pdv_03_lib.cpp file
Motor motor = Motor(200, BLUE, YELLOW, GREEN, RED);

void setup(){
  Serial.begin(9600);
  //Set speed to 30 rpm
  motor.speed(30);
}

void loop(){
  // Time initialization
  motor.now = millis();
  // Calibrate the motor if it is not yet calibrated or needs re-calibration and the re-calibration buttons are pressed for 3 seconds
  if(motor.calibrated == 0 || ((motor.now - motor.last_time >= 3000) && motor.calibrate_repeat())){
    // Necessary for initial startup because of not-yet set variables
    if(motor.calibrated == 0){
      motor.calibrate();
    }
  }

  motor.check_buttons();

  // Only perform up/down motor movement if the deadman-switch is pressed
  if(motor.deadman_pin_check == true){
    // If the motor is either already going up or
    // The down button is not pressed, it is not in the up position, the up button is pressed and it was in down position
    if(motor.down_pin_check == false && motor.standard_position != 50
      && (((motor.up_pin_check == true) && motor.standard_position == 0) || motor.running_dir == 1)){        
      // Perform one stop upwards, adjust the current position and set the direction to upwards  
      motor.step(1);
      motor.standard_position += 1;
      motor.running_dir = 1;
    }
    // If the motor is not in down position, and down is pressed OR it is already moving downwards
    // The last condition lets the down button overwrite all other movements
    else if(motor.standard_position != 0 && (motor.down_pin_check == true || motor.running_dir == 0)){
      // Perform one step downwards, adjust the current position and set the direction to downwards
      motor.step(-1);
      motor.standard_position -= 1;
      motor.running_dir = 0;
    }
  }
  // If the deadman-switch is not pressed and the motor is neither in op nor in down position, move it downwards
  else if(motor.standard_position != 50 && motor.standard_position != 0){
    motor.step(-1);
    motor.standard_position -= 1;
  }
}