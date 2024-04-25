

class Motor {
  public:
    Motor(int number_of_steps, int pin_1, int pin_2, int pin_3, int pin_4);
    
    void speed(long what_speed);

    void step(int steps_to_move);

    void calibrate();

    void check_buttons();

    int prevent_bouncing(int target_button, int last_state);

    int calibrate_repeat();

    int standard_position;
    int current_position;

    int calibrated;

    unsigned int now;
    unsigned int last_time;

    int deadman_pin;
    int up_pin;
    int down_pin;

    bool up_pin_check = false;
    bool down_pin_check = false;
    bool deadman_pin_check = false;
    unsigned long last_step_time; 
    unsigned long step_delay; 
    int running_dir; // Indicates the current direction of rotation

  private:
    void execute_steps(int this_step);

    void print_initial_help();

    int upin_1;
    int upin_2;
    int upin_3;
    int upin_4;
    int confirmation_pin;

    bool is_any_button_pressed;

    int calibration_button_last;
    
    bool calibrating;

    int number_of_steps;         
    int step_number;
    int direction;  
};

Motor::Motor(int number_of_steps, int pin_1, int pin_2, int pin_3, int pin_4){
  // Initialize variables for internal motor control
  this->step_number = 0;    
  this->direction = 0; //Direciton, 0 for down, 1 for up  
  this->last_step_time = 0; //Time of the last performed step
  this->number_of_steps = number_of_steps; //Number of steps for one revolution
  this->calibrated = 0; //If the motor is currently calibrated
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
  this->step_delay = 60L * 1000L * 1000L / this->number_of_steps / what_speed; 
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
      
      // increment or decrement step_number based on directionn
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

int Motor::prevent_bouncing(int target_button, int last_state){
  unsigned long now = micros();

  now = micros();
  if(now - this->last_step_time >= 200){
    this->last_step_time = now;
    if((digitalRead(target_button) == HIGH) && last_state == 0){
      return 1; // Returns 1 in case of a successful change from low to high
    }else if((digitalRead(this->up_pin)) == HIGH || (digitalRead(this->down_pin)) == HIGH || (digitalRead(this->deadman_pin)) == HIGH){
      return 2; // Return 2 in case of any button press except for confirm
    }
    // Get the last state
  }     
  // Get the last state
  this->calibration_button_last = (digitalRead(target_button) == LOW) ? 0 : last_state;

  return 0;  // Return 0 for no change
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
  while(this->calibrated != 1){
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
        this->calibrated = prevent_bouncing(this->confirmation_pin, this->calibration_button_last);
        // check if any other button is pressed, cancel confirmation
        if(this->calibrated == 2){
          Serial.println("Confirmation has been canceled. Proceeding calibration.\n");
          this->is_any_button_pressed = true;
        }
        // check if confirm button is pressed, confirm calibration
        else if(this->calibrated == 1){
          Serial.println("----------------------\nCalibration successful!\n----------------------\n\n");
          //The calibrated position is defined as 0
          this->standard_position = 0;
          this->calibrating = false;
          this->last_time = millis();
          this->is_any_button_pressed = false;
          break;
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
            this->calibrated = 0;
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
    motor.running_dir = 0;
  }
}