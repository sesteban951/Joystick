#include "joy.h"
#include <iostream>
#include <unistd.h> // For sleep()
#include <iomanip> // For std::setprecision()

int main() 
{
    // Set precision for printing to the console
    std::cout << std::fixed;
    std::cout << std::setprecision(3);

    // create xbox controller instance
    Joystick xbox;

    // variables to be mapped
    bool state_cmd_initialize_robot = false; // SELECT
    bool state_cmd_enter_standby = false;    // START
    bool state_cmd_go_down = false;          // LB
    bool state_cmd_go_up = false;            // RB
    bool open_servos = false;         // A
    bool close_servos = false;        // Y
    bool kill_robot = false;          // XBOX

    // buttons struct
    Buttons buttons;

    while (true) 
    {
        // update inputs
        xbox.update();
        buttons = xbox.buttons;
        
        // print the values
        // std::cout << "---------------------------------------" << std::endl;
        // std::cout << "X: " << buttons.X << ", "
        //           << "Y: " << buttons.Y << ", "
        //           << "A: " << buttons.A << ", "
        //           << "B: " << buttons.B << ", "
        //           << "LB: " << buttons.LB << ", "
        //           << "RB: " << buttons.RB << ", "
        //           << "SELECT: " << buttons.SELECT << ", "
        //           << "START: " << buttons.START << ", "
        //           << "XBOX: " << buttons.XBOX
        //           << std::endl;

        // assign the button states to the variables
        state_cmd_initialize_robot = buttons.SELECT;
        state_cmd_enter_standby = buttons.START;
        state_cmd_go_down = buttons.LB;
        state_cmd_go_up = buttons.RB;
        open_servos = buttons.A;
        close_servos = buttons.Y;
        kill_robot = buttons.XBOX;

        // print the commands
        if (state_cmd_initialize_robot) 
        {
            std::cout << "Initialize robot command received!" << std::endl;
        }
        if (state_cmd_enter_standby) 
        {
            std::cout << "Enter standby command received!" << std::endl;
        }
        if (state_cmd_go_down) 
        {
            std::cout << "Go down command received!" << std::endl;
        }
        if (state_cmd_go_up) 
        {            
            std::cout << "Go up command received!" << std::endl;
        }
        if (open_servos) 
        {
            std::cout << "Open servos command received!" << std::endl;
        }
        if (close_servos) 
        {
            std::cout << "Close servos command received!" << std::endl;
        }
        if (kill_robot) 
        {
            std::cout << "Kill robot command received!" << std::endl;
            return 0; // Exit the program
        }

        usleep(10000); // Sleep for 50ms to avoid excessive CPU usage
    }

    return 0;
}
