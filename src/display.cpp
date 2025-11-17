#include <stdexcept>

#include <display.hpp>

#include <SDL3/SDL_render.h>

/** @example
 * +-+-+-+-+
 * |1|2|3|4|
 * +-+-+-+-+
 * |Q|W|E|R|
 * +-+-+-+-+
 * |A|S|D|F|
 * +-+-+-+-+
 * |Z|X|C|V|
 * +-+-+-+-+
 */
const static SDL_Scancode KEY_BINDINGS[16] = {
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
    SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
    SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V
};

Display::Display(Chip8& _chip)
 : chip(_chip)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) 
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        throw std::runtime_error("Failed Initializing SDL");
    }

    if (!SDL_CreateWindowAndRenderer("CHIP-8 Emulator", 
        C8Resolution::WIDTH * scale_factor, 
        C8Resolution::HEIGHT * scale_factor, 
        SDL_WINDOW_RESIZABLE, 
        &window, &renderer
    )) 
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        throw std::runtime_error("Creating window/renderer failed");
    }

    texture = SDL_CreateTexture(
        renderer, 
        SDL_PIXELFORMAT_RGBA8888, 
        SDL_TEXTUREACCESS_STREAMING, 
        C8Resolution::WIDTH * scale_factor, 
        C8Resolution::HEIGHT * scale_factor
    );

    if (!texture)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("Could not initialize texture");
    }
}

Display::~Display()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

void Display::handle_input()
{
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            is_running = false;
            break;

        case SDL_EVENT_KEY_DOWN:
            for (int i = 0; i < 16; ++i)
            {
                if (event.key.scancode == KEY_BINDINGS[i])
                    chip.set_key(i);
            }
            break;
        
        case SDL_EVENT_KEY_UP:
            for (int i = 0; i < 16; ++i)
            {
                if (event.key.scancode == KEY_BINDINGS[i])
                    chip.unset_key(i);
            }
            break;
        
        default:
            break;
        }
    }
}

void Display::update_screen()
{
    int pitch = 0;
    uint32_t* pixels = nullptr;

    SDL_LockTexture(texture, NULL, (void**)(&pixels), &pitch);

    for (int y = 0; y < C8Resolution::HEIGHT; ++y) 
    {
        for (int x = 0; x < C8Resolution::WIDTH; ++x) 
        {   
            int index = y * C8Resolution::WIDTH + x;
            int pitch_index = y * (pitch / sizeof(uint32_t)) + x;

            pixels[pitch_index] = chip.get_display_pixel(index) == 0 ? ColourSpecs::BG_COLOUR : ColourSpecs::DRAW_COLOUR; 
        }
    }

    SDL_UnlockTexture(texture);

    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}