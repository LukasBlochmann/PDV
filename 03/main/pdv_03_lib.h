#ifndef pdv_03_lib_h
#define pdv_03_lib_h

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

    int direction;          
    int number_of_steps;         
    int step_number;
};

#endif