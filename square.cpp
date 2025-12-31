#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include "handleErrors.h"

void drawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius)
{
    for (int x = -radius; x <= radius; x++)
    {
        for (int y = -radius; y <= radius; y++)
        {
            if (x * x + y * y <= radius * radius)
            {
                SDL_RenderDrawPoint(renderer, cx + x, cy + y);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return SDL_INITIALIZE_ERROR;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Scratch Scene",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool running = true;
    SDL_Event eventSDL;

    while (running)
    {
        while (SDL_PollEvent(&eventSDL))
        {
            if (eventSDL.type == SDL_QUIT)
                running = false;
        }
        
        SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 50, 180, 50, 255);
        SDL_Rect ground{0, 450, 800, 150};
        SDL_RenderFillRect(renderer, &ground);

        SDL_SetRenderDrawColor(renderer, 101, 67, 33, 255);
        SDL_Rect trunk{130, 300, 40, 150};
        SDL_RenderFillRect(renderer, &trunk);

        SDL_SetRenderDrawColor(renderer, 30, 140, 30, 255);
        drawFilledCircle(renderer, 150, 280, 60);
        drawFilledCircle(renderer, 110, 300, 50);
        drawFilledCircle(renderer, 190, 300, 50);

        SDL_SetRenderDrawColor(renderer, 255, 224, 189, 255);
        drawFilledCircle(renderer, 550, 310, 20);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawLine(renderer, 550, 330, 550, 390);

        SDL_RenderDrawLine(renderer, 520, 360, 580, 360);

        SDL_RenderDrawLine(renderer, 550, 390, 530, 430);
        SDL_RenderDrawLine(renderer, 550, 390, 570, 430);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
