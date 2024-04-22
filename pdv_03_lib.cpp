#include <Arduino.h>
#include "pdv_03_lib.h"

Motor::Motor(int number_of_steps, int pin_1, int pin_2,
                                      int pin_3, int pin_4){
  // first initialize variables for motor communication
  this->step_number = 0;    
  this->direction = 0;      
  this->last_step_time = 0; 
  this->number_of_steps = number_of_steps; 

  // Pins on Arduino (setted by user) which control the motor
  this->upin_1 = pin_1;
  this->upin_2 = pin_2;
  this->upin_3 = pin_3;
  this->upin_4 = pin_4;


  // initialize buttons for user com. and calibration
  this->forward = 10;
  this->backwards = 11;
  this->confirm = 12;
  this->calibrated = false;
  this->calibration_button_last = 1;
  this->any_button_pressed = false;
  for (int i = 10; i < 14; i++){pinMode(i, INPUT);}

  // initialize variables for normal prosedure (deadman switch)
  this->deadman_on = false;
  this->up = 10;
  this->down = 11;
  this->deadman = 12;

  // setup pins for motor control
  pinMode(this->upin_1, OUTPUT);
  pinMode(this->upin_2, OUTPUT);
  pinMode(this->upin_3, OUTPUT);
  pinMode(this->upin_4, OUTPUT);

  this->pin_count = 4;
}

void Motor::speed(long whatSpeed){
  this->step_delay = 60L * 1000L * 1000L / this->number_of_steps / whatSpeed; //! ?????????????????????????????????????????????
}

// step handling
void Motor::step(int steps_to_move){
  // steps to go until the motor stops
  int steps_left = abs(steps_to_move);  

  // DIR = 1 -> forward, DIR = 0 -> backwards
  if (steps_to_move > 0) { this->direction = 1; }
  if (steps_to_move < 0) { this->direction = 0; }


  // decrement the remaining steps
  while (steps_left > 0)
  {
    unsigned long now = micros();
    // only move if interval has passed
    if (now - this->last_step_time >= this->step_delay)
    {
      // timestamp for next calc.
      this->last_step_time = now;
      
      // remaining step handling (stops when given step number reached)
      if (this->direction == 1)
      {
        this->step_number++;
        if (this->step_number == this->number_of_steps) {
          this->step_number = 0;
        }
      }
      else
      {
        if (this->step_number == 0) {
          this->step_number = this->number_of_steps;
        }
        this->step_number--;
      }
      // c.f. l. 53
      steps_left--;
      // modulo 4 to choose the current phase
      execute_steps(this->step_number % 4);
    }
  }
}

// initial communication with user before motor is used for the first time
void Motor::user_launch_communication(){

  Serial.println();
  Serial.println("You have to calibrate the motor before using it!");
  Serial.println();
  Serial.println("The motor is correctly calibrated, when the cabinet is positioned horizontally.");
  Serial.println();
  Serial.println("Please press Button: ");
  Serial.println("  - 10, to move it forwards;");
  Serial.println("  - 11, to move it backwards;");
  Serial.println("  - 12, to confirm the calibration.");
  Serial.println();

}

// calibrating the motor, user and motor interaction
void Motor::calibrate(){
  // print user communication interface to console
  user_launch_communication();

  // calibration loop
  while(this->calibrated != true){
    if ((digitalRead(this->forward)) == HIGH){
      this->step(1);
    }else if ((digitalRead(this->backwards)) == HIGH){
      this->step(-1);
    }else if (digitalRead((this->confirm)) == HIGH){
      // prevent bouncing, only one confirmation per press
      Serial.println("Sure, the motor is calibrated?");
      Serial.println("Please press confirmation Button again to confirm.");
      Serial.println("Press any other button to continue calibration.");
      Serial.println();
      this->calibration_button_last = 1;

      
      while(this->any_button_pressed == false){
        // check if any button is pressed, cancel calibration
        if((digitalRead(this->forward)) == HIGH || digitalRead((this->backwards)) == HIGH || digitalRead(13) == HIGH){
          Serial.println("Confirmation has been canceled. Proceeding calibration.");
          Serial.println();
          this->any_button_pressed = true;
          
        // check if confirm button is pressed, confirm calibration
        }else if((digitalRead(this->confirm) == HIGH) && this->calibration_button_last == 0){
          this->calibrated = true;
          Serial.println("Motor is calibrated.");
          break;
        }
        // this line guarantees that the confirm button needs to be pressed a second time in order to confirm the calibration
        this->calibration_button_last = (digitalRead(this->confirm) == LOW) ? 0 : this->calibration_button_last;
      }
      // set bool to false to make loop available for next calibration
      this->any_button_pressed = false;
    }
  }

}

// communication with motor, actual step executor
void Motor::execute_steps(int thisStep){
  // depending in which phase we are currently the function passes the sequence
  switch (thisStep){
    case 0:  // 1010
      digitalWrite(upin_1, HIGH);
      digitalWrite(upin_2, LOW);
      digitalWrite(upin_3, HIGH);
      digitalWrite(upin_4, LOW);
    break;
    case 1:  // 0110
      digitalWrite(upin_1, LOW);
      digitalWrite(upin_2, HIGH);
      digitalWrite(upin_3, HIGH);
      digitalWrite(upin_4, LOW);
    break;
    case 2:  //0101
      digitalWrite(upin_1, LOW);
      digitalWrite(upin_2, HIGH);
      digitalWrite(upin_3, LOW);
      digitalWrite(upin_4, HIGH);
    break;
    case 3:  //1001
      digitalWrite(upin_1, HIGH);
      digitalWrite(upin_2, LOW);
      digitalWrite(upin_3, LOW);
      digitalWrite(upin_4, HIGH);
    break;
  
  }
}


// user interaction to steer the motor
//  bug fixes

void Motor::user_interaction_deadman(){
  if(digitalRead(this->deadman) == HIGH){
    this->deadman_on = true;
}}

