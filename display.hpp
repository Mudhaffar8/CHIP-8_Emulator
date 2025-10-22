#pragma once

#include <cstdint>

#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>

#include "chip.hpp"

namespace ColourSpecs
{
static const uint32_t bg_colour = 0x00000FF;
static const uint32_t draw_colour = 0xFFFFFFFF;
}

class Display
{
public:
    Display();
    ~Display();

    void handle_input(bool* buffer);
    void update_screen(const uint8_t* buffer);

    inline bool is_program_running() { return is_running; }

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    SDL_Event event;

    bool is_running = true;
};
