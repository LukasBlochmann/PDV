#ifndef pdv_03_lib_h
#define pdv_03_lib_h

class Motor {
  public:
    Motor(int number_of_steps, int pin_1, int pin_2, int pin_3, int pin_4);
    
    // speed setting
    void speed(long what_speed);

    // calculate delay and number of steps
    void step(int steps_to_move);

    // align the motor to the initial position
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

    int running_dir;

    bool up_pin_check = false;
    bool down_pin_check = false;
    bool deadman_pin_check = false;
    unsigned long last_step_time; 
    unsigned long step_delay; 


  private:
    // execute calibrated steps
    void execute_steps(int this_step);

    // user interface for calibration
    void print_initial_help();

    // upin = user pin, given by master
    int upin_1;
    int upin_2;
    int upin_3;
    int upin_4;

    // needed in calibration func as an cancel indicator
    bool is_any_button_pressed;

    // used in calibration func, declares the buttons used for following actions
    int forward;
    int backwards;
    int confirmation_pin;

    // used in calibration func, to prevent bouncing
    int calibration_button_last;

    // following variables are used for direct interaction with the motor
    int direction;          
    int number_of_steps;      
    int pin_count;            
    int step_number;
 
    // variables needed for normal prosedure (deadman switch)
    int deadman_on;
    int exit_pos;
    bool set_back;
    bool calibrating;
    bool is_returning;
};

#endif

