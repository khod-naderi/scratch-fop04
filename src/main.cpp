#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include "menubar.h"
#include "color.h"
#include "ui.h"
#include "canvas.h"
#include "spritebox.h"
#include "workspace.h"
#include "saveload.h"
#include "logger.h"

int main(int argc, char *argv[])
{
    // -----------------------------
    // Initialization
    // -----------------------------
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return SDL_INITIALIZE_ERROR;
    }

    if (TTF_Init() != 0)
    {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return SDL_TTF_INIT_ERROR;
    }

    TTF_Font *font = TTF_OpenFont("assets/fonts/Sans_Serif.ttf", 16);
    if (!font)
    {
        std::cerr << "Failed to load font" << std::endl;
        return SDL_FONT_NOT_FOUND;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Scratch",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        MAIN_WINDOW_WIDTH,
        MAIN_WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS);

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED);

    // -----------------------------
    // Logger
    // -----------------------------
    Logger appLogger;
    logger_log(appLogger, "Application started");
    logger_log(appLogger, "SDL and TTF initialized");

    // -----------------------------
    // Initialize UI systems
    // -----------------------------
    int errcode = menu_init(renderer, font);
    if (errcode != 0)
        return errcode;
    logger_log(appLogger, "Menu initialized");

    errcode = categoryColumnInit(renderer, font);
    if (errcode != 0)
        return errcode;
    logger_log(appLogger, "Category column initialized");

    errcode = blockColumnInit(renderer, font);
    if (errcode != 0)
        return errcode;
    logger_log(appLogger, "Block column initialized");

    errcode = sprintBoxInit(renderer, font);
    if (errcode != 0)
        return errcode;
    logger_log(appLogger, "Sprite box initialized");

    errcode = workspaceScreenInit(renderer, font);
    if (errcode != 0)
        return errcode;
    logger_log(appLogger, "Workspace initialized");

    // -----------------------------
    // Window icon
    // -----------------------------
    SDL_Surface *appIconSurface = IMG_Load("assets/icons/icon.png");
    if (appIconSurface)
    {
        SDL_SetWindowIcon(window, appIconSurface);
        SDL_FreeSurface(appIconSurface);
        logger_log(appLogger, "Window icon loaded");
    }
    else
    {
        logger_log(appLogger, "Failed to load window icon", LogLevel::WARN);
    }

    SDL_bool running = SDL_TRUE;
    SDL_Event eventSDL;

    // -----------------------------
    // Main loop
    // -----------------------------
    while (running)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        while (SDL_PollEvent(&eventSDL))
        {
            if (eventSDL.type == SDL_QUIT)
            {
                logger_log(appLogger, "Quit event received");
                running = SDL_FALSE;
            }
            else if (eventSDL.type == SDL_MOUSEMOTION)
            {
                controlWorkspaceMouseMotion(mouseX, mouseY);
            }
            else if (eventSDL.type == SDL_MOUSEBUTTONDOWN)
            {
                if (eventSDL.button.button == SDL_BUTTON_LEFT)
                {
                    controlSaveClickDown(mouseX, mouseY);
                    controlMenubarClickDown(mouseX, mouseY);
                    controlCategoryColumnClickDown(mouseX, mouseY);
                    controlBlockColumnClickDown(mouseX, mouseY);
                    controlWorkspaceClickDown(mouseX, mouseY);
                }
            }
            else if (eventSDL.type == SDL_MOUSEBUTTONUP)
            {
                if (eventSDL.button.button == SDL_BUTTON_LEFT)
                {
                    controlBlockColumnClickUp(mouseX, mouseY);
                    controlWorkspaceClickUp(mouseX, mouseY);
                }
            }
            else if (eventSDL.type == SDL_MOUSEWHEEL)
            {
                int direction = -1;
                if (eventSDL.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
                    direction = 1;

                // Logger scroll if hovered
                if (isPointInRect(mouseX, mouseY, LOGGER_Box))
                {
                    appLogger.scroll_offset -= eventSDL.wheel.y * direction;
                    if (appLogger.scroll_offset < 0)
                        appLogger.scroll_offset = 0;
                }
                else
                {
                    controlBlockColumnMouseScroll(
                        mouseX, mouseY,
                        eventSDL.wheel.y * direction);
                }
            }
        }

        updateMenuHoverState(mouseX, mouseY);

        // -----------------------------
        // Rendering
        // -----------------------------
        SDL_SetRenderDrawColor(renderer, color_white);
        SDL_RenderClear(renderer);

        drawWorkspaceScreen(renderer, font, mouseX, mouseY);
        drawCatagoryColumn(renderer, font, mouseX, mouseY);
        drawSpriteBoxScreen(renderer, font, mouseX, mouseY);
        drawCanvasScreen(renderer, font, mouseX, mouseY);
        drawBlockColumn(renderer, font, mouseX, mouseY);
        drawMenubar(renderer, font, mouseX, mouseY);

        if (isOnLoadScreen)
        {
            drawLoadScreen(renderer, font, mouseX, mouseY);
        }
        else if (isOnSaveScreen)
        {
            drawSaveScreen(renderer, font, mouseX, mouseY);
        }

        // ✅ Logger rendered last (on top)
        render_logger(renderer, font, appLogger);

        SDL_RenderPresent(renderer);
    }

    // -----------------------------
    // Cleanup
    // -----------------------------
    logger_log(appLogger, "Application shutting down");

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
