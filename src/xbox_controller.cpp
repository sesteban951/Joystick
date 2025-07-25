#include "joystick/xbox_controller.h"

XboxController::XboxController() 
{
    // Find the Xbox controller device path
    std::string devicePath = FindXboxController();

    if (devicePath.empty()) 
    {
        std::cerr << "Xbox controller not found" << std::endl;

        // Set the shudown flag to true
        std::cerr << "Shutting down ..." << std::endl;
        shut_down_ = true;

        return;
    }


    fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK); // Open in non-blocking mode
    if (fd < 0) 
    {
        std::cerr << "Error opening device file: " << devicePath << " - " << strerror(errno) << std::endl;
    }
}

XboxController::~XboxController() 
{
    if (fd >= 0) 
    {
        close(fd);
    }
}

void XboxController::Update() 
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
                case BTN_MODE:  // Sometimes called KEY_MODE or KEY_XBOX
                {
                    if (ev.value == 1) // 1 = pressed, 0 = released, 2 = held
                    {
                        // xbox button pressed
                        this->XBOX = true;
                    }
                    else {
                        // xbox button released
                        this->XBOX = false;
                    }
                    break;
                }
                case BTN_SOUTH: // A button
                {
                    if (ev.value == 1) 
                    {
                        this->A = true;
                    }
                    else 
                    {
                        this->A = false;
                    }
                    break;
                }
                case BTN_EAST: // B button
                {
                    if (ev.value == 1) 
                    {
                        this->B = true;
                    }
                    else 
                    {
                        this->B = false;
                    }
                    break;
                }
                case BTN_NORTH: // X button
                {
                    if (ev.value == 1) 
                    {                        
                        this->X = true;
                    }
                    else 
                    {
                        this->X = false;
                    }
                    break;
                }
                case BTN_WEST: // Y button
                {
                    if (ev.value == 1) 
                    {
                        this->Y = true;
                    }
                    else 
                    {
                        this->Y = false;
                    }
                    break;
                }
                case BTN_TL: // Left Bumper
                {
                    if (ev.value == 1) 
                    {
                        this->LB = true;
                    }
                    else 
                    {
                        this->LB = false;
                    }
                    break;
                }
                case BTN_TR: // Right Bumper
                {
                    if (ev.value == 1) 
                    {
                        this->RB = true;
                    }
                    else 
                    {
                        this->RB = false;
                    }
                    break;
                }
                case BTN_SELECT: // select button
                {
                    if (ev.value == 1) 
                    {
                        this->SELECT = true;
                    }
                    else 
                    {
                        this->SELECT = false;
                    }
                    break;
                }
                case BTN_START: // start button
                {
                    if (ev.value == 1) 
                    {
                        this->START = true;
                    }
                    else 
                    {
                        this->START = false;
                    }
                    break;
                }
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

std::string XboxController::FindXboxController() 
{
    for (const auto& name : knownNames) 
    {
        std::string path = FindXboxController(name);
        if (!path.empty())
            return path;
    }
    return "";
}

std::string XboxController::FindXboxController(const std::string& target_name) 
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