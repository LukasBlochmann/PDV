#include <Arduino.h> 
#include "pdv_03_lib.h"

//!!! Datentypen anpassen!!

Motor::Motor(int number_of_steps, int pin_1, int pin_2, int pin_3, int pin_4){
  // Initialize variables for internal motor control
  this->step_number = 0;    
  this->direction = 0;      
  this->last_step_time = 0; 
  this->number_of_steps = number_of_steps; 
  this->calibrated = false; //If the motor is currently calibrated
  this->calibration_button_last = 1; 
  this->is_any_button_pressed = false;
  this->standard_position; //Position the motor is in after a successful movement
  this->exit_pos; //Absolute value of the remaining steps
  this->calibrating = false;
  this->is_returning = false;

  // Pins that are connected to the motor
  this->upin_1 = pin_1;
  this->upin_2 = pin_2;
  this->upin_3 = pin_3;
  this->upin_4 = pin_4;

  // initialize buttons for user communication and calibration
  this->up_pin = 10;
  this->down_pin = 11;
  this->confirmation_pin = 12;
  this->deadman_pin = 13;
  
  // Configure the user communication pins as inputs
  for (int i = 10; i <= 13; i++){pinMode(i, INPUT);}

  // Configure the motor pins as outputs
  pinMode(this->upin_1, OUTPUT);
  pinMode(this->upin_2, OUTPUT);
  pinMode(this->upin_3, OUTPUT);
  pinMode(this->upin_4, OUTPUT);
}

// Set the speed in revs per minute
void Motor::speed(long whatSpeed){
  this->step_delay = 60L * 1000L * 1000L / this->number_of_steps / whatSpeed; //! ?????????????????????????????????????????????
}

// Perform a given number of steps
int Motor::step(int steps_to_move){
  //steps_to_move: Number of steps to move, positive for up and negative for down

  int steps_left = abs(steps_to_move);  

  // determine direction based on whether steps_to_move is + or -:
  if (steps_to_move > 0) { this->direction = 1; }
  else if (steps_to_move < 0) { this->direction = 0; }

  // decrement the remaining steps
  while (steps_left > 0){
    // In case the motor is not being calibrated, releasing the deadman switch has to stop all actions
    if(digitalRead(this->deadman_pin) == LOW && this->calibrating == false && this->is_returning == false){
      // Save the remaining steps and exit with an error message
      this->exit_pos = steps_left;
      this->step_number = 0;
      this->number_of_steps = 50;
      // Return an error
      return 1;
    }

    // only move if interval has passed
    unsigned long now = micros();
    if (now - this->last_step_time >= this->step_delay){
      // timestamp for next iteration
      this->last_step_time = now;
      
      // increment or decrement step_number based on direction
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
      // modulo 4 to choose the current phase, which also determines the direction
      execute_steps(this->step_number % 4);
    }
  }
  // If all steps were performed, return 0
  this->is_returning = false;
  return 0;
}

// Physical communication with the motor
void Motor::execute_steps(int thisStep){
  // depending in which phase we are currently in, the function passes the sequence
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

bool Motor::prevent_bouncing(int target_button, int last_state){
  while(1){
    unsigned long now = micros();
    if(now - this->last_step_time >= 200){
      this->last_step_time = now;
      if((digitalRead(target_button) == HIGH) && last_state == 0){
        return true;
      }
    }   
    last_state = (digitalRead(target_button) == LOW) ? 0 : last_state;
  }     
}

// Initial communication with user before motor is used for the first time
void Motor::print_initial_help(){
  Serial.println("\nYou have to calibrate the motor before using it!\n");
  Serial.println("The motor is correctly calibrated, when the cabinet is positioned horizontally, to the left of the motor, when looking at it from the cabinet side.\n");
  Serial.println("Please press the Buttons accordingly (left to right): ");
  Serial.println("  - 1: Rotate clockwise");
  Serial.println("  - 2: Rotate counterclockwise");
  Serial.println("  - 3: Confirm calibration");
  Serial.println("  - 4: Deadman-Switch\n\n");
}

// Calibrate the motor
void Motor::calibrate(){
  print_initial_help();
  //Indicate the motor is being calibrated
  this->calibrating = true;
  // calibration loop
  while(this->calibrated != true){
    // Check which button is pressed, handle accordingly
    if ((digitalRead(this->up_pin)) == HIGH){
      this->step(1);
    }else if ((digitalRead(this->down_pin)) == HIGH){
      this->step(-1);
    }else if (digitalRead((this->confirmation_pin)) == HIGH){
      Serial.println("Please check again if the motor is calibrated correctly and confirm again.");
      Serial.println("Press any other button to continue the calibration.\n");

      //Prevent bouncing by checking if the button was released before being pressed again
      this->calibration_button_last = 1;
      
      while(this->is_any_button_pressed == false){
        // check if any other button is pressed, cancel calibration
        if((digitalRead(this->up_pin)) == HIGH || digitalRead((this->down_pin)) == HIGH || digitalRead(13) == HIGH){
          Serial.println("Confirmation has been canceled. Proceeding calibration.\n");
          this->is_any_button_pressed = true;
        }
        // check if confirm button is pressed, confirm calibration
        else{
          this->calibrated = prevent_bouncing(this->confirmation_pin, this->calibration_button_last);
          if(this->calibrated == true){
            Serial.println("----------------------\nCalibration successful!\n----------------------\n\n");
            //The calibrated position is defined as 0
            this->standard_position = 0;
            this->calibrating = false;
            break;
          }
        }
      }
      this->is_any_button_pressed = false;
    }
  }
}

// Print user information in case the deadman switch was activated
void Motor::print_user_interruption_deadman(){
  Serial.println("You stopped pressing the Deadman-Switch.\n");
  Serial.println("Press the Deadman-Switch again to return to the calibrated position");
}

// Waits in cas teh deadman switch was released while an action was performed
void Motor::execute_user_interruption_deadman(){
  //while(digitalRead(this->deadman_pin) == LOW) //Wait until the deadman switch is pressed again. Stop all other actions, until this happens. 
    //continue; //Empty loop
  this->is_returning = true;
  this->direction == 1 ? this->step(-( 50 - this->exit_pos)) : this->step(- this->exit_pos); //Move the motor to the calibrate position based on the intended direction and abs(remaining steps)
  //Restore normal working conditions
  this->exit_pos = 0;
  this->standard_position = 0;
}

// Performs all the action with the user and takes account of the deadman switch
// Is being used in the main loop
void Motor::user_interaction_deadman(){
  // Only perform action if the deadman switch is pressed
  if(digitalRead(this->deadman_pin) == HIGH){
    // Check if the motor is in normal working conditions and up is pressed
    if(digitalRead(this->up_pin) == HIGH && this->standard_position == 0){
      // Move to up position, if it fails due to the deadman switch being released, call the according function
      if(digitalRead(this->deadman_pin) == high){
        
      }
      }
      // If the motor was rotated successfuly, set the standard position to 50 (up) 
      else{
        this->standard_position = 50;
      }
    }
    // Check if the motor is in normal working conditions and down is pressed
    // All the other logic applies here too, for details, see above
    else if(digitalRead(this->down_pin) == HIGH && this->standard_position == 50){
      if(this->step(-50) == 1){
        print_user_interruption_deadman();
        execute_user_interruption_deadman();
      }else{
        this->standard_position = 0;
      }
    }
  }
}
