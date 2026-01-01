#include <SDL2/SDL.h>
#include <iostream>
#include "generaldef.h"

int main(int argc, char *argv[])
{
    // Initialize SDL2 everything
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return SDL_INITIALIZE_ERROR;
    }

    // Creating a static windows
    SDL_Window *window = SDL_CreateWindow(
        "Scratch",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        MAIN_WINDOW_WIDTH,
        MAIN_WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_bool running = SDL_TRUE; // this is root app running indenticator

    SDL_Event eventSDL;

    while (running)
    {
        while (SDL_PollEvent(&eventSDL))
        {
            // handle Quit button on top-right corner of page
            if (eventSDL.type == SDL_QUIT)
            {
                running = SDL_FALSE;
            }
        }

        // Clear background color to white.
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Draw blue rectangle at top as menu bar
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_Rect menubar = {
            0,
            0,
            MAIN_WINDOW_WIDTH,
            30};
        SDL_RenderFillRect(renderer, &menubar);

        SDL_RenderPresent(renderer);
    }

    // Exiting from program.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}