#pragma once

#include <linux/input.h>
#include <linux/joystick.h>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <filesystem>
#include <chrono>
#include <vector>

#include <linux/input-event-codes.h>  // For BTN_* and ABS_ codes
#include <map>
#include <string>
#include <iostream>

// axes struct
struct Axes {
    double LS_VERT = 0.0;   // Left Stick X-direction
    double LS_HORIZ = 0.0;   // Left Stick Y-direction
    double RS_VERT = 0.0;   // Right Stick X-direction
    double RS_HORIZ = 0.0;   // Right Stick Y-direction
    double LT = 0.0;     // Left Trigger
    double RT = 0.0;     // Right Trigger
};

// buttons struct
struct Buttons {
    bool A = false;
    bool B = false;
    bool X = false;
    bool Y = false;
    bool LB = false;
    bool RB = false;
    bool SELECT = false;
    bool START = false;
    bool XBOX = false;
};

class Joystick
{
    public: 

        // Constructor and Destructor
        Joystick();
        ~Joystick();
        
        void Update(); // Reads input and updates values
        double CmdX() const { return this->v_x_; }
        double CmdY() const { return this->v_y_; }
        double CmdZ() const { return this->w_z_; }
        bool ShutDownCmd() const { return this->shut_down_; }

        void UpdateInputs();

        // structs containing axes and buttons
        Axes axes;
        Buttons buttons;

    private:

        // File descriptor for the input device
        int fd; 

        // Known joystick names to search for
        std::vector<std::string> knownNames = {
            "Microsoft X-Box One pad",
            "Microsoft Xbox Series S|X Controller",
            "Xbox Wireless Controller",
            "Xbox One Wireless Controller",
            "Xbox Controller"  // fallback, can be broad
        };

        // Method to find the joystick device
        std::string FindJoystick();
        std::string FindJoystick(const std::string& target_name);

        // axes
        double LS_X = 0.0;   // Left Stick X-direction
        double LS_Y = 0.0;   // Left Stick Y-direction
        double RS_X = 0.0;   // Right Stick X-direction
        double RS_Y = 0.0;   // Right Stick Y-direction
        double LT = 0.0;     // Left Trigger
        double RT = 0.0;     // Right Trigger

        // buttons
        bool A = false;
        bool B = false;
        bool X = false;
        bool Y = false;
        bool LB = false;
        bool RB = false;
        bool SELECT = false;
        bool START = false;
        bool XBOX = false;

        double v_x_ = 0.0;
        double v_y_ = 0.0;
        double w_z_ = 0.0;
        bool shut_down_ = false;

        double val_max_ = 32767.0;
        double w_z_max_ = 1023.0;

        double val_threshold_ = 0.05;

};

