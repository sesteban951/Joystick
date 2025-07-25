#ifndef xbox_controller_h
#define xbox_controller_h

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

class XboxController
{
    public: XboxController();
    public: ~XboxController();

    private: std::string FindXboxController();
    private: std::string FindXboxController(const std::string& target_name);

    public: void Update(); // Reads input and updates values
    public: double CmdX() const { return this->v_x_; }
    public: double CmdY() const { return this->v_y_; }
    public: double CmdZ() const { return this->w_z_; }
    public: bool ShutDownCmd() const { return this->shut_down_; }

    public: void UpdateInputs();


    private: int fd; // File descriptor for the input device

    private: double v_x_ = 0.0;
    private: double v_y_ = 0.0;
    private: double w_z_ = 0.0;
    private: bool shut_down_ = false;

    private: double val_max_ = 32767.0;
    private: double w_z_max_ = 1023.0;

    private: double val_threshold_ = 0.1;

    private: std::vector<std::string> knownNames = {
        "Microsoft X-Box One pad",
        "Microsoft Xbox Series S|X Controller",
        "Xbox Wireless Controller",
        "Xbox One Wireless Controller",
        "Xbox Controller"  // fallback, can be broad
    };


    // buttons
    private:
        bool A = false;
        bool B = false;
        bool X = false;
        bool Y = false;
        bool LB = false;
        bool RB = false;
        bool SELECT = false;
        bool START = false;
        bool XBOX = false;
    public:
        Buttons buttons;
};

#endif
