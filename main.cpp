#include <SDL2/SDL.h>

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "Scratch",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}