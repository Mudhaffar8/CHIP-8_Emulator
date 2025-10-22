#include <iostream>
#include <chrono>

#include "chip.hpp"
#include "display.hpp"

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Invalid # of Arguments (Expected 2)" << std::endl;
        exit(1);
    }

    Chip8 chip;

    if (!chip.loadGame(argv[1]))
        exit(1);

    Display display;

    const double frame_duration = 16.777; // In milliseconds
    while (display.is_program_running())
    {
        auto start = std::chrono::steady_clock::now();

        display.handle_input(chip.keys);

        chip.executeInstruction();

        if (chip.drawFlag)
            display.update_screen(chip.display);
 
        auto end = std::chrono::steady_clock::now();
        auto diff = end - start;

        SDL_Delay(3); // TODO: Have delay change based on loop execution time for consistent FPS 
    }

    return 0;
}