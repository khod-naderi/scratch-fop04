#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include "generaldef.h"

int main(int argc, char *argv[])
{
    // Initialize SDL2 everything
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return SDL_INITIALIZE_ERROR;
    }

    // Initialize SDL_ttf
    if (TTF_Init() != 0)
    {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return SDL_TTF_INIT_ERROR;
    }

    // Load font
    TTF_Font *font = TTF_OpenFont("assets/fonts/Sans_Serif.ttf", 16);
    if (!font)
    {
        std::cerr << "Failed to Load font: no such file assets/fonts/Sans_Serif.ttf" << std::endl;
        return SDL_FONT_NOT_FOUND;
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

    // Import logo image
    SDL_Surface *logoSurface = IMG_Load("assets/images/logo.png");
    if (!logoSurface)
    {
        std::cerr << "Faild to load logo image: " << IMG_GetError() << std::endl;
        return SDL_IMAGE_LOAD_ERROR;
    }
    SDL_Texture *logoTexture = SDL_CreateTextureFromSurface(renderer, logoSurface);
    SDL_FreeSurface(logoSurface);

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
            MENUBAR_HEIGHT};
        SDL_RenderFillRect(renderer, &menubar);

        SDL_Rect logoRect = {
            LOGO_MARGIN_LEFT, // margin left
            (MENUBAR_HEIGHT - LOGO_HEIGHT) / 2,
            LOGO_WIDTH,
            LOGO_HEIGHT};

        SDL_RenderCopy(renderer, logoTexture, NULL, &logoRect);

        SDL_RenderPresent(renderer);
    }

    // Exiting from program.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}