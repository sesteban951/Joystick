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

    // axes and buttons
    Buttons buttons;
    Axes axes;

    // if joystick is connected, start reading inputs
    if (xbox.isConnected())
    {
        while (true) 
        {
            xbox.update();

            buttons = xbox.buttons;
            axes = xbox.axes;

            std::cout << "LS: (" << axes.LS_X << ", " << axes.LS_Y << "), "
                    << "RS: (" << axes.RS_X << ", " << axes.RS_Y << "), "
                    << "LT: " << axes.LT << ", "
                    << "RT: " << axes.RT << ", "
                    << "DPAD: (" << axes.DPAD_X << ", " << axes.DPAD_Y << "), "
                    << "A: " << buttons.A << ", "
                    << "B: " << buttons.B << ", "
                    << "X: " << buttons.X << ", "
                    << "Y: " << buttons.Y << ", "
                    << "LB: " << buttons.LB << ", "
                    << "RB: " << buttons.RB << ", "
                    << "SELECT: " << buttons.SELECT << ", "
                    << "START: " << buttons.START << ", "
                    << "XBOX: " << buttons.XBOX
                    << std::endl;

            if (!xbox.isConnected())
            {
                std::cout << "Joystick disconnected." << std::endl;
                break;
            }

            usleep(10000); // Sleep for 1ms to avoid excessive CPU usage
        }
    }

    return 0;
}
