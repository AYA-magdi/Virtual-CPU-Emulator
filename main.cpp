
// main.cpp
#include "vOLE_machine.h"

int main()
{
    std::string filename;
    std::cout << "Enter the name of the program file: ";
    std::cin >> filename;

    VOLEMachine machine(256, 16); // Initialize with 256 bytes of memory and 16 registers
    if (machine.loadProgramFromFile(filename))
    {
        machine.run();
    }
    else
    {
        std::cerr << "Failed to load program." << std::endl;
    }

    return 0;
}

