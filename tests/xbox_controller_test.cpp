#include "joy.h"
#include <iostream>
#include <unistd.h> // For sleep()
#include <iomanip> // For std::setprecision()

int main() 
{
    // Set precision for printing to the console
    std::cout << std::fixed;
    std::cout << std::setprecision(3);

    Joystick xbox;

    while (true) 
    {
        xbox.Update();
        
        std::cout << "X: " << xbox.CmdX() << " Y: " << xbox.CmdY() << " Z: " << xbox.CmdZ() << std::endl;

        if(xbox.ShutDownCmd() == true) 
        {
            std::cout << "Xbox button pressed!" << std::endl;
            return 0;
        }

        usleep(10000); // Sleep for 50ms to avoid excessive CPU usage
    }

    return 0;
}
