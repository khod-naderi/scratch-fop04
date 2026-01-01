/*
This cpp file is for handling UI component
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "ui.h"
#include "menubar.h"
#include <iostream>
#include "color.h"

/*
    This function render text
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

int menu_init(SDL_Renderer *renderer, TTF_Font *font)
{
    logoSurface = IMG_Load("assets/images/logo.png");
    if (!logoSurface)
    {
        std::cerr << "Faild to load logo image: " << IMG_GetError() << std::endl;
        return SDL_IMAGE_LOAD_ERROR;
    }
    logoTexture = SDL_CreateTextureFromSurface(renderer, logoSurface);
    SDL_FreeSurface(logoSurface);

    fileMenuText = renderText(renderer, font, "File", color_white);
    editMenuText = renderText(renderer, font, "Edit", color_white);

    return 0;
}