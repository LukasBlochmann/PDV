#ifndef pdv_03_lib_h
#define pdv_03_lib_h


class Motor {
  public:
    Motor(int number_of_steps, int pin_1, int pin_2, int pin_3, int pin_4);
    

    // speed setting
    void speed(long user_speed);

    // calculate delay and number of steps
    int step(int number_of_steps);

    // align the motor to the initial position
    void calibrate();

    // user calibration for normal prosedure (deadman switch)
    void user_interaction_deadman();

    void print_user_interruption_deadman();	

    void execute_user_interruption_deadman();

    bool prevent_bouncing(int button_last, int active_last);


    // saves wether motor has been calibrated or not
    bool calibrated;

    int deadman_pin;
    int up_pin;
    int down_pin;

  private:
    // execute calibrated steps
    void execute_steps(int this_step);

    // user interface for calibration
    void user_launch_communication();

    // upin = user pin, given by master
    int upin_1;
    int upin_2;
    int upin_3;
    int upin_4;

    // needed in calibration func as an cancel indicator
    bool any_button_pressed;

    // used in calibration func, declares the buttons used for following actions
    int forward;
    int backwards;
    int confirm;

    // used in calibration func, to prevent bouncing
    int calibration_button_last;

    // following variables are used for direct interaction with the motor
    int direction;          
    unsigned long step_delay; 
    int number_of_steps;      
    int pin_count;            
    int step_number;
    unsigned long last_step_time; 

    // variables needed for normal prosedure (deadman switch)
    int deadman_on;
    int std_pos;
    int exit_pos;
    bool set_back;
    bool not_calibrating;
    
};

#endif

