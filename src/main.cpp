#include <iostream>
#include <chrono>
#include <string>

#include <chip.hpp>
#include <display.hpp>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Invalid # of Arguments (Expected 2 or 3)" << std::endl;
        exit(1);
    }

    Chip8 chip;

    if (!chip.load_game(argv[1]))
        exit(1);

    Display display(chip);
    // if (argc > 2)
    //     display.set_scale_factor(std::stoi(argv[2]));

    static const double frame_duration = 5.0;
    while (display.is_program_running()) 
    {
        auto start = std::chrono::high_resolution_clock::now();

        display.handle_input();

        chip.execute_instruction();

        if (chip.check_draw_flag())
            display.update_screen();
 
        auto end = std::chrono::high_resolution_clock::now();
        double diff = std::chrono::duration<double, std::milli>(end - start).count();

        if (diff < frame_duration)
            SDL_Delay(frame_duration - diff); 
    }

    return 0;
}