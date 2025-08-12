#include "joy.h"

Joystick::Joystick() 
{
    // Find the joystick controller device path
    std::string devicePath = FindJoystick();

    if (devicePath.empty()) 
    {
        std::cerr << "No joystick found." << std::endl;
        return;
    }

    // Open the joystick device file
    fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK); // Open in non-blocking mode
    if (fd < 0) 
    {
        std::cerr << "Error opening device file: " << devicePath << " - " << strerror(errno) << std::endl;
    }
    else 
    {
        std::cout << "Joystick connected: [" << devicePath << "]" << std::endl;
        this->joy_connected = true; // Set the connection status to true
    }
}

Joystick::~Joystick() 
{
    // close the file descriptor if it was opened
    if (fd >= 0) 
    {
        close(fd);
    }
}

void Joystick::update() 
{   
    // Check if the joystick is connected
    if (fd < 0) {
        //std::cerr << "Device file not open" << std::endl;
        return;
    }

    struct input_event ev;
    while (read(fd, &ev, sizeof(ev)) > 0) 
    {
        // axes values
        if (ev.type == EV_ABS) 
        {
            switch (ev.code) 
            {
                case ABS_X:  // Left Stick X-axis (LS_X)
                {
                    this->LS_X = (double) ev.value / this->stick_value_max;
                    this->LS_X = (fabs(this->LS_X) < this->val_deadzone) ? 0.0 : this->LS_X;
                    break;
                }
                case ABS_Y:  // Left Stick Y-axis (LS_Y)
                {
                    this->LS_Y = -(double) ev.value / this->stick_value_max; 
                    this->LS_Y = (fabs(this->LS_Y) < this->val_deadzone) ? 0.0 : this->LS_Y;
                    break;
                }
                case ABS_RX: // Right Stick X-axis (RS_X)
                {
                    this->RS_X = (double) ev.value / this->stick_value_max;
                    this->RS_X = (fabs(this->RS_X) < this->val_deadzone) ? 0.0 : this->RS_X;
                    break;
                }
                case ABS_RY: // Right Stick Y-axis (RS_Y)
                {
                    this->RS_Y = -(double) ev.value / this->stick_value_max;
                    this->RS_Y = (fabs(this->RS_Y) < this->val_deadzone) ? 0.0 : this->RS_Y;
                    break;
                }
                case ABS_Z:  // Left Trigger (LT)
                {
                    this->LT = (double) ev.value / this->trigg_value_max;
                    break;
                }
                case ABS_RZ: // Right Trigger (RT)
                {
                    this->RT = (double) ev.value / this->trigg_value_max;
                    break;
                }
                case ABS_HAT0X: // D-Pad Left/Right
                {
                    this->DPAD_X = (double) ev.value;
                    break;
                }
                case ABS_HAT0Y: // D-Pad Up/Down
                {
                    this->DPAD_Y = -(double) ev.value;
                    break;
                }
            }
        }

        // button press
        if (ev.type == EV_KEY) 
        {
            switch (ev.code) {
                case BTN_MODE:    // Sometimes called KEY_MODE or KEY_XBOX
                    this->XBOX = (ev.value == 1); 
                    break;

                case BTN_SOUTH:   // A button
                    this->A = (ev.value == 1); 
                    break;

                case BTN_EAST:    // B button
                    this->B = (ev.value == 1); 
                    break;

                case BTN_NORTH:   // X button
                    this->X = (ev.value == 1); 
                    break;

                case BTN_WEST:    // Y button
                    this->Y = (ev.value == 1); 
                    break;

                case BTN_TL:      // Left Bumper
                    this->LB = (ev.value == 1); 
                    break;

                case BTN_TR:      // Right Bumper
                    this->RB = (ev.value == 1); 
                    break;

                case BTN_SELECT:  // select button
                    this->SELECT = (ev.value == 1); 
                    break;

                case BTN_START:   // start button
                    this->START = (ev.value == 1); 
                    break;
            }
        }
    }

    // build the axes struct
    this->axes.LS_X = this->LS_X;
    this->axes.LS_Y = this->LS_Y;
    this->axes.RS_X = this->RS_X;
    this->axes.RS_Y = this->RS_Y;
    this->axes.LT = this->LT;
    this->axes.RT = this->RT;
    this->axes.DPAD_X = this->DPAD_X;
    this->axes.DPAD_Y = this->DPAD_Y;

    // build the buttons struct
    this->buttons.A = this->A;
    this->buttons.B = this->B;
    this->buttons.X = this->X;
    this->buttons.Y = this->Y;
    this->buttons.LB = this->LB;
    this->buttons.RB = this->RB;
    this->buttons.SELECT = this->SELECT;
    this->buttons.START = this->START;
    this->buttons.XBOX = this->XBOX;

    // Check if the joystick is still connected
    ssize_t n = read(fd, &ev, sizeof(ev));
    if (n == -1) {
        if (errno == ENODEV) 
        {
            std::cerr << "Controller disconnected (ENODEV)" << std::endl;
            this->joy_connected = false; // Set the connection status to false
        }
    }
}

std::string Joystick::FindJoystick() 
{
    for (const auto& name : knownNames) 
    {
        std::string path = FindJoystick(name);
        if (!path.empty())
            return path;
    }
    return "";
}

std::string Joystick::FindJoystick(const std::string& target_name) 
{
    std::string basePath = "/dev/input/";

    for (const auto& entry : std::filesystem::directory_iterator(basePath)) 
    {
        std::string devPath = entry.path().string();

        // Only look at js* and event* devices
        if (devPath.find("event") == std::string::npos &&
            devPath.find("js") == std::string::npos)
            continue;

        int fd = open(devPath.c_str(), O_RDONLY);
        if (fd < 0) continue;

        char name[256] = "Unknown";
        if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0) 
        {
            close(fd);
            continue;
        }

        close(fd);

        if (std::string(name).find(target_name) != std::string::npos) 
        {
            return devPath;
        }
    }

    return "";
}