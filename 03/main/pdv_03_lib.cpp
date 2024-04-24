#include <Arduino.h> 
#include "pdv_03_lib.h"

//!!! Datentypen anpassen!!

Motor::Motor(int number_of_steps, int pin_1, int pin_2, int pin_3, int pin_4){
  // Initialize variables for internal motor control
  this->step_number = 0;    
  this->direction = 0; //Direciton, 0 for down, 1 for up  
  this->last_step_time = 0; //Time of the last performed step
  this->number_of_steps = number_of_steps; //Number of steps for one revolution
  this->calibrated = false; //If the motor is currently calibrated
  this->calibration_button_last = 1; 
  this->is_any_button_pressed = false;
  this->standard_position; //Position the motor is in after a successful movement
  this->calibrating = false; //If the motor is currently being calibrated
  this->now = 0; //Internal time tracking variable
  this->last_time = 0; //

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

  // Button states
  this->up_pin_check = false;  //True in case up is pressed
  this->down_pin_check = false; //True in case down is pressed
  this->deadman_pin_check = false; //True in case deadman-switch is pressed
  
  // Configure the user communication pins as inputs
  for (int i = 10; i <= 13; i++){pinMode(i, INPUT);}

  // Configure the motor pins as outputs
  pinMode(this->upin_1, OUTPUT);
  pinMode(this->upin_2, OUTPUT);
  pinMode(this->upin_3, OUTPUT);
  pinMode(this->upin_4, OUTPUT);
}

// Set the speed in revs per minute
void Motor::speed(long what_speed){
  this->step_delay = 60L * 1000L * 1000L / this->number_of_steps / what_speed; //! ?????????????????????????????????????????????
  // Use as longs because of the data type of the micros() function
  // 60 Minutes * 1000 ms * 1000 µs / number of steps / speed in rpm = delay between steps in µs
}

// Perform a given number of steps in a given direction, normally 1 or -1
void Motor::step(int steps_to_move){ 
  int steps_left = abs(steps_to_move);  

  // determine direction based on whether steps_to_move is + or -:
  if (steps_to_move > 0) { this->direction = 1; }
  else if (steps_to_move < 0) { this->direction = 0; }

  while (steps_left > 0){
    // only move if interval has passed
    unsigned long now = micros();
    if (now - this->last_step_time >= this->step_delay){
      // Set timestamp for next step
      this->last_step_time = now;
      
      // increment or decrement step_number based on direction
      if (this->direction == 1){
        this->step_number++;
        // If the current step number is equal to the number of steps in one revolution, reset it to 0 to start over again
        if (this->step_number == this->number_of_steps) {this->step_number = 0;}
      }else{
        // If the current step number is equal to 0, set it to the number of steps in one revolution to start over again
        if (this->step_number == 0) {this->step_number = this->number_of_steps;} //number of steps = 200
        this->step_number--;
      }
      
      // Decrement steps
      steps_left--;
      // modulo 4 to choose the current phase, which also determines the direction used internally
      execute_steps(this->step_number % 4);
    }
  }
}

// Physical communication with the motor
void Motor::execute_steps(int this_step){
  // depending in which phase we are currently in, the function passes the sequence
  switch (this_step){
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
  unsigned long now = micros();
  // Intentionally block the processor because in this situation, we have to forcefully stop all other actions
  while(1){
    now = micros();
    // 
    if(now - this->last_step_time >= 200){
      this->last_step_time = now;
      // This is the only relevant change in state, so we only have to check this. It detects a change from LOW to HIGH
      if((digitalRead(target_button) == HIGH) && last_state == 0){
        // Exit the function and return success
        return true;
      }
    }   
    // Get the last state
    last_state = (digitalRead(target_button) == LOW) ? 0 : last_state;
  }     
}

// Initial communication with user before motor is used for the first time
void Motor::print_initial_help(){
  // Print calibration menu
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
  // Print how to calibrate the motor
  print_initial_help();
  
  //Indicate the motor is being calibrated
  this->calibrating = true;
  // Intentionally block the processor because in this situation, we have to forcefully stop all other actions before calibration is done
  while(this->calibrated != true){
    // Check which button is pressed, handle accordingly
    if ((digitalRead(this->up_pin)) == HIGH){
      this->step(1);
    }else if ((digitalRead(this->down_pin)) == HIGH){
      this->step(-1);
    }else if (digitalRead((this->confirmation_pin)) == HIGH){
      Serial.println("Please check again if the motor is calibrated correctly and confirm again.");
      Serial.println("Press any other button to continue the calibration.\n");

      // Setting the last button state as up for later bouncing prevention
      this->calibration_button_last = 1;
      
      while(this->is_any_button_pressed == false){
        // check if any other button is pressed, cancel confirmation
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
            this->last_time = millis();
            break;
          }
        }
      }
      this->is_any_button_pressed = false;
    }
  }
}
  
void Motor::check_buttons(){
  // Check and save the states of all buttons (low = false, high = true)
  this->up_pin_check = (digitalRead(this->up_pin) == HIGH) ? true : false;
  this->down_pin_check = (digitalRead(this->down_pin) == HIGH) ? true : false;
  this->deadman_pin_check = (digitalRead(this->deadman_pin) == HIGH) ? true : false;
}

int Motor::calibrate_repeat() {
  // Repeat calibration if all 4 buttons are pressed at the same time and hold for 3 seconds
  // We encountered one single test where the motor lost track of its position, however, we could not reproduce this error
  // In case it ever happens, this provides a solution to re-calibrate the motor
    if (digitalRead(this->confirmation_pin) == HIGH &&
        digitalRead(this->up_pin) == HIGH &&
        digitalRead(this->down_pin) == HIGH &&
        digitalRead(this->deadman_pin) == HIGH) {

          this->last_time = this->calibrating == true ? this->last_time : millis();

          // Indicate uncalibrated state if it is calibrating
          if(this->calibrating == true){
            this->calibrated = false;
          }else
          // Re-Calibration detected
          {
            Serial.println("Calibrating attempt detected.");
            this->calibrating = true;
          }
          // User-canceled re-calibration
          return 1; 
        }
    // Successfuly re-calibrated
    return 0;
}