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
#include <vector>
#include <linux/input-event-codes.h>  // For BTN_* and ABS_ codes
#include <map>

// axes struct
struct Axes {
    double LS_Y = 0.0;    // Left Stick Y-direction (Up is +), [-1.0, 1.0]
    double LS_X = 0.0;    // Left Stick X-direction (Right is +), [-1.0, 1.0]
    double RS_Y = 0.0;    // Right Stick Y-direction (Up is +), [-1.0, 1.0]
    double RS_X = 0.0;    // Right Stick X-direction (Right is +), [-1.0, 1.0]
    double LT = 0.0;      // Left Trigger (Press is +), [0.0, 1.0]
    double RT = 0.0;      // Right Trigger (Press is +), [0.0, 1.0]
    double DPAD_X = 0.0;  // D-Pad X-direction (Right is +), (-1.0, 0.0, 1.0)
    double DPAD_Y = 0.0;  // D-Pad Y-direction (Up is +), (-1.0, 0.0, 1.0)
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
        
        // Reads input and updates values
        void update(); 

        // check if joystick is connected
        bool isConnected() const { return joy_connected; }

        // structs containing axes and buttons
        Axes axes;
        Buttons buttons;

    private:

        // File descriptor for the input device
        int fd; 

        // Known joystick names to search for (add to list as needed using "sudo evtest")
        std::vector<std::string> knownNames = {
            "Microsoft X-Box One pad",
            "Microsoft Xbox Series S|X Controller",
            "Xbox Wireless Controller",
            "Xbox One Wireless Controller",
            "Microsoft X-Box One Elite 2 pad",
            "Xbox Controller"  // fallback, can be broad
        };

        // Method to find the joystick device
        std::string FindJoystick();
        std::string FindJoystick(const std::string& target_name);

        // connection status
        bool joy_connected = false;

        // axes
        double LS_X = 0.0;   // Left Stick X-direction
        double LS_Y = 0.0;   // Left Stick Y-direction
        double RS_X = 0.0;   // Right Stick X-direction
        double RS_Y = 0.0;   // Right Stick Y-direction
        double LT = 0.0;     // Left Trigger
        double RT = 0.0;     // Right Trigger
        double DPAD_X = 0.0; // D-Pad X-direction
        double DPAD_Y = 0.0; // D-Pad Y-direction

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

        // mapping values
        double stick_value_max = 32767.0;  // Max value for joystick axes
        double trigg_value_max = 1023.0;   // Max value for triggers
        double val_deadzone = 0.05;        // Deadzone for joystick values
};

