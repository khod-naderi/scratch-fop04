#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include "generaldef.h"

// handle state of menu buttons
struct MenuState
{
    bool isfileMenuOpen;
    bool iseditMenuOpen;
    int hoveredFileItem;
    int hoveredEditItem;
};

/*
    This function render text
    input:
    - target renderer
    - font
    - text
    - color

    output:
    - SDL_Texture form that textbox.
*/
SDL_Texture *renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    if (!surface)
        return nullptr;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

/*
    This function will check that a point is inside a rectangle or not
*/
bool isPointInRect(int x, int y, SDL_Rect rect)
{
    return (x >= rect.x) && (x <= (rect.x + rect.w)) && (y >= rect.y) && (y <= (rect.y + rect.h));
}

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

    // Menu state
    MenuState menuState = {false, false, -1, -1};
    // Define menu reactangles
    SDL_Rect fileMenuRect = {LOGO_MARGIN_LEFT + LOGO_WIDTH + 2 * MENU_MARGIN_LEFT, 5, 60, MENUBAR_HEIGHT - 10};
    SDL_Rect editMenuRect = {fileMenuRect.x + fileMenuRect.w + MENU_MARGIN_LEFT, 5, 60, MENUBAR_HEIGHT - 10};

    // File menu Items
    const std::string fileMenuItems[] = {"New Project", "Load Project", "Save Project"};
    // Edit menu items
    const std::string editMenuItems[] = {"Help"};

    SDL_Texture *fileMenuText = renderText(renderer, font, "File", {255, 255, 255, 255});
    SDL_Texture *editMenuText = renderText(renderer, font, "Edit", {255, 255, 255, 255});

    while (running)
    {
        // Get mouse pointer state
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

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

        // Draw Logo
        SDL_Rect logoRect = {
            LOGO_MARGIN_LEFT, // margin left
            (MENUBAR_HEIGHT - LOGO_HEIGHT) / 2,
            LOGO_WIDTH,
            LOGO_HEIGHT};

        SDL_RenderCopy(renderer, logoTexture, NULL, &logoRect);

        // Draw File menu button
        bool fileMenuHoverd = isPointInRect(mouseX, mouseY, fileMenuRect);
        if (fileMenuHoverd || menuState.isfileMenuOpen)
        {
            SDL_SetRenderDrawColor(renderer, 50, 50, 200, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        }
        SDL_RenderFillRect(renderer, &fileMenuRect);

        // Draw File menu text
        if (fileMenuText)
        {
            int tw, th;
            SDL_QueryTexture(fileMenuText, NULL, NULL, &tw, &th);
            SDL_Rect textRect = {
                fileMenuRect.x + (fileMenuRect.w - tw) / 2,
                fileMenuRect.y + (fileMenuRect.h - th) / 2,
                tw,
                th,
            };
            SDL_RenderCopy(renderer, fileMenuText, NULL, &textRect);
        }

        // Draw Edit menu button
        bool editMenuHoverd = isPointInRect(mouseX, mouseY, editMenuRect);
        if (editMenuHoverd || menuState.iseditMenuOpen)
        {
            SDL_SetRenderDrawColor(renderer, 50, 50, 200, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        }
        SDL_RenderFillRect(renderer, &editMenuRect);

        // Draw Edit menu text
        if (editMenuText)
        {
            int tw, th;
            SDL_QueryTexture(editMenuText, NULL, NULL, &tw, &th);
            SDL_Rect textRect = {
                editMenuRect.x + (editMenuRect.w - tw) / 2,
                editMenuRect.y + (editMenuRect.h - th) / 2,
                tw,
                th,
            };
            SDL_RenderCopy(renderer, editMenuText, NULL, &textRect);
        }

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