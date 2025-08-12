#include "joy.h"

Joystick::Joystick() 
{
    // Find the joystick controller device path
    std::string devicePath = FindJoystick();

    if (devicePath.empty()) 
    {
        std::cerr << "Joystick not found" << std::endl;

        // Set the shudown flag to true
        std::cerr << "Shutting down ..." << std::endl;
        shut_down_ = true;

        return;
    }

    // Open the joystick device file
    fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK); // Open in non-blocking mode
    if (fd < 0) 
    {
        std::cerr << "Error opening device file: " << devicePath << " - " << strerror(errno) << std::endl;
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

void Joystick::Update() 
{
    //std::cout << "Updating" << std::endl;
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
                case ABS_Y:  
                {
                    double value = (double) ev.value;
                    double v_x_raw = -value / val_max_;
                    if(fabs(v_x_raw) < this->val_threshold_) 
                    {
                        v_x_raw = 0.0;
                    }
                    this->v_x_ = v_x_raw;
                    break;
                }
                case ABS_RX: 
                {
                    double value = (double) ev.value;
                    double v_y_raw = -value / val_max_;
                    if(fabs(v_y_raw) < this->val_threshold_) 
                    {
                        v_y_raw = 0.0;
                    }
                    this->v_y_ = v_y_raw;
                    break;
                }
                // RT and LT
                case ABS_RZ:
                {
                    double value = (double) ev.value;
                    double w_z_raw = -value / this->w_z_max_;
                    if(fabs(w_z_raw) < this->val_threshold_) 
                    {
                        w_z_raw = 0.0;
                    }
                    this->w_z_ = w_z_raw;
                    break;
                }
                // Left trigger
                case ABS_Z:
                {
                    double value = (double) ev.value;
                    double w_z_raw = value / this->w_z_max_;
                    if(fabs(w_z_raw) < this->val_threshold_) 
                    {
                        w_z_raw = 0.0;
                    }
                    this->w_z_ = w_z_raw;
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

    ssize_t n = read(fd, &ev, sizeof(ev));

    if (n == -1) {
        if (errno == ENODEV) 
        {
            std::cerr << "Controller disconnected (ENODEV)" << std::endl;
            this->shut_down_ = true;
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