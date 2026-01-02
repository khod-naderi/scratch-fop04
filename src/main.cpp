#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include "generaldef.h"
#include "menubar.h"
#include "color.h"
#include "ui.h"

int main(int argc, char *argv[])
{
    /*
    ---------------------------------------------
    Initialization part
    in this part we intialize SDL2 library and resources like
    fonts, SDL_Renderer, SDL_event handler, Windows
    ---------------------------------------------
    */
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

    // Initialize menu textures
    int errcode = menu_init(renderer, font);
    if (errcode != 0)
        return errcode;
    errcode = categoryColumnInit(renderer, font);
    if (errcode != 0)
        return errcode;

    SDL_bool running = SDL_TRUE; // this is root app running indenticator

    SDL_Event eventSDL;

    /*
    ---------------------------------------------
    Main while of program
    ---------------------------------------------
    */
    while (running)
    {
        // Get mouse pointer state
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        /*
        ---------------------------------------------
        Event Handler
        in this part we handle SDL evnets before rendering next frame
        ---------------------------------------------
        */
        while (SDL_PollEvent(&eventSDL))
        {
            // handle Quit button on top-right corner of page
            if (eventSDL.type == SDL_QUIT)
            {
                running = SDL_FALSE;
            }
            else if (eventSDL.type == SDL_MOUSEBUTTONDOWN)
            {
                if (eventSDL.button.button == SDL_BUTTON_LEFT)
                {
                    controlMenubarClickDown(mouseX, mouseY);
                    controlCategoryColumnClickDown(mouseX, mouseY);
                }
            }
        }

        /*
        ---------------------------------------------
        In This part we will update hover status of every hoverable thing
        ---------------------------------------------
        */
        updateMenuHoverState(mouseX, mouseY);

        /*
        ---------------------------------------------
        Now it's to render next frame
        ---------------------------------------------
        */
        // Clear background color to white.
        SDL_SetRenderDrawColor(renderer, color_white);
        SDL_RenderClear(renderer);

        // Draw Menu Bar
        drawCatagoryColumn(renderer, font, mouseX, mouseY);
        drawBlockColumn(renderer, font, mouseX, mouseY);
        drawMenubar(renderer, font, mouseX, mouseY);

        // show next frame
        SDL_RenderPresent(renderer);
    }

    // Cleanup and exiting from program.
    SDL_DestroyTexture(fileMenuText);
    SDL_DestroyTexture(editMenuText);
    SDL_DestroyTexture(logoTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}