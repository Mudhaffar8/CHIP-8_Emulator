#pragma once

#include <cstdint>

#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>

#include <chip.hpp>

namespace ColourSpecs
{
    static const uint32_t BG_COLOUR = 0x00000FF;
    static const uint32_t DRAW_COLOUR = 0xFFFFFFFF;
}

class Display
{
public:
    Display(Chip8& chip);
    ~Display();

    void handle_input();
    void update_screen();

    inline bool is_program_running() { return is_running; }
    inline void set_scale_factor(int scale_factor) { this->scale_factor = scale_factor; }

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;

    SDL_Event event;

    Chip8& chip;

    bool is_running = true;
    int scale_factor = 1;
};
