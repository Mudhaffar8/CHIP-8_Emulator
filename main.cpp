#define SDL_MAIN_USE_CALLBACKS 1

#include "chip.hpp"

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_keyboard.h>

const int PIXEL_SIZE = 10;

SDL_AppResult handleKeyPress(SDL_Scancode key);
void draw();

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

static Chip8* chip = NULL;

int I = 0;

SDL_Scancode keyBindings[16] = {
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
    SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
    SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V
};

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    if (argc != 2) 
    {
        SDL_Log("Invalid # of arguments");
        return SDL_APP_FAILURE;
    }

    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) 
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Create Window
    if (!SDL_CreateWindowAndRenderer("CHIP-8 Emulator", WIDTH * PIXEL_SIZE, HEIGHT * PIXEL_SIZE, 0, &window, &renderer)) 
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    chip = new Chip8();

    if (!chip->loadGame(argv[1]))
        return SDL_APP_FAILURE;

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    /* end the program, reporting success to the OS. */
    switch (event->type)
    {
        case SDL_EVENT_QUIT: 
            return SDL_APP_SUCCESS;  

        case SDL_EVENT_KEY_DOWN:
            return handleKeyPress(event->key.scancode);

        default: 
            break;
    } 
    
    return SDL_APP_CONTINUE;  
}



/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
    chip->emulateCycle();

    if (chip->drawFlag) 
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

        /* clear the window to the draw color. */
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        
        draw();
        
        /* put the newly-cleared rendering on the screen. */
        SDL_RenderPresent(renderer);
    }

    SDL_Delay(4);

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
    delete chip;
}


SDL_AppResult handleKeyPress(SDL_Scancode key)
{
    if (key == SDL_SCANCODE_ESCAPE) 
        return SDL_APP_SUCCESS;

    // const bool* keyboardState = SDL_GetKeyboardState(NULL);

    for (int i = 0; i < 16; i++)
    {
        if (key == keyBindings[i])
        {
            chip->keys[i] = true; 
        } 
        else 
        {
            chip->keys[i] = false;
        }
    }

    return SDL_APP_CONTINUE;
}

void draw()
{
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            if (chip->display[j + (i * WIDTH)]) 
            {
                SDL_FRect rect = {
                    (float)(j * PIXEL_SIZE),
                    (float)(i * PIXEL_SIZE),
                    (float)PIXEL_SIZE,
                    (float)PIXEL_SIZE
                };

                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}