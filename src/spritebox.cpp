/*
-------------------------------------------
This header file is for managing custome and backgorund setting area named sprite box
-------------------------------------------
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "spritebox.h"
#include "color.h"
#include "ui.h"
#include "generaldef.h"
#include <iostream>
#include "canvas.h"

// selected sprite id in sprite box
int SelectedSpriteID = -1;

/*
-------------------------------------------------
This function is for adding and initializeing a sprint to screen
this function will run only one time.
-------------------------------------------------

it will return id of new sprint
*/
int addSprintToScreen(SDL_Renderer *renderer, std::string name, const char *imgPath)
{
    SprintBody newSptint;

    newSptint.name = name;

    // spon on the center of screen
    newSptint.posX = CANVAS_SCREEN_WIDTH / 2;
    newSptint.posY = CANVAS_SCREEN_HEIGTH / 2;

    newSptint.angleRotation = 0; // render at it's original oriantation.

    int id = 0;
    for (const SprintBody &s : aliveSprints) // founding new latest ID
    {
        if (id <= s.id)
        {
            id = s.id + 1;
        }
    }
    newSptint.id = id;

    // load texture of it's custome.
    newSptint.nowCustome = renderImage(renderer, imgPath);
    if (!newSptint.nowCustome)
    {
        std::cerr << "Faild to load Custome image of \"" << name << "\": " << IMG_GetError() << std::endl;
        return SDL_IMAGE_LOAD_ERROR;
    }

    aliveSprints.push_back(newSptint);

    return id;
}

/*
-------------------------------------------------
This function is for drawing Sprite box.
this function will run on every frame.
-------------------------------------------------
*/

void drawSpriteBoxScreen(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY)
{
    // Draw backgound color
    SDL_SetRenderDrawColor(renderer, color_spriteBoxBackground);
    SDL_RenderFillRect(renderer, &SPRITE_BOX);
    // Draw border of Canvas
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &SPRITE_BOX);

    // Top bar of Sprite box 
    SDL_Rect tobBar = { SPRITE_BOX.x, SPRITE_BOX.y, SPRITE_BOX.w, 100 };
    SDL_SetRenderDrawColor(renderer, 211, 211, 211, 255);
    SDL_RenderFillRect(renderer, &tobBar);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &tobBar);

    // Sprite Area
    SDL_Rect spriteArea = { SPRITE_BOX.x, tobBar.y + tobBar.h, SPRITE_BOX.w, SPRITE_BOX.h - tobBar.h };
    SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
    SDL_RenderFillRect(renderer, &spriteArea);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &spriteArea);

    // Sprtie thumbnails
    const int thumbnailSize = 70;
    const int spacing = 10;
    int StartX = spriteArea.x + spacing; 
    int StartY = spriteArea.y + spacing;

    for ( int i = 0; i < aliveSprints.size(); i++ )
    {
        SprintBody& sprite = aliveSprints[i];

        // position of thumbnail
        int thumbnailX = StartX + i * (thumbnailSize + spacing);
        int thumbnailY = StartY;

        // Draw thumbnail
        SDL_Rect thumbRect = {thumbnailX , thumbnailY, thumbnailSize, thumbnailSize};

        // if hovering 
        bool ishovered = ( mouseX >= thumbRect.x && mouseX <= thumbRect.x + thumbRect.w &&
                           mouseY >= thumbRect.y && mouseY <= thumbRect.y + thumbRect.h);
        
        // background color 
        SDL_SetRenderDrawColor( renderer , 230, 230, 230, 255);
        SDL_RenderFillRect(renderer, &thumbRect);                     

        // image of thumbnail
        SDL_RenderCopy (renderer, sprite.nowCustome, nullptr, &thumbRect);

        // hovering effect
        if (ishovered) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 100, 150, 255, 80); // blue with some transparency
            SDL_RenderFillRect(renderer, &thumbRect);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
        // Draw border
        SDL_SetRenderDrawColor(renderer, color_black);
        SDL_RenderDrawRect(renderer, &thumbRect);

        // hovering border
        if (ishovered) {
            SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255); // blue
        } 
        else {
            SDL_SetRenderDrawColor(renderer, color_black);
        }
        SDL_RenderDrawRect(renderer, &thumbRect);

        // drawing sprite name 
        SDL_Color textColor = color_black;
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, sprite.name.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        // position of text
        int textX = thumbnailX + thumbnailSize / 2 - textSurface->w / 2;
        int textY = thumbnailY + thumbnailSize + 5; // 5 pixels below the thumbnail
        
        SDL_Rect textRect = {textX, textY, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
}

/*
----------------------------------------------------
This function is for initializing Sprint Screen
----------------------------------------------------
*/
int sprintBoxInit(SDL_Renderer *renderer, TTF_Font *font)
{
    // Add default Sprint
    int errcode = addSprintToScreen(renderer, "Mad Man", "assets/images/sprints/madman.png");
    if (errcode)
        return errcode;

    return 0;
}

/*
----------------------------------------------------
This function is for handling selection on sprite thumbnails 
----------------------------------------------------
*/

void SpriteBoxClick(const int mouseX , const int mouseY ) 
{
    int thumbnailSize = 70;
    int spacing = 10;

    SDL_Rect spriteArea = { SPRITE_BOX.x, SPRITE_BOX.y + 100, SPRITE_BOX.w, SPRITE_BOX.h - 100 };

    int startX = spriteArea.x + spacing;
    int startY = spriteArea.y + spacing;

    // checking each thumbnail 
    for ( int i = 0; i < aliveSprints.size(); i++ ){

        int thumbnailX = startX + i * (thumbnailSize + spacing);
        int thumbnailY = startY;

        SDL_Rect thumbRect = {thumbnailX , thumbnailY, thumbnailSize, thumbnailSize};

        // if clicked on this thumbnail 
        if ( mouseX >= thumbRect.x && mouseX <= thumbRect.x + thumbRect.w &&
             mouseY >= thumbRect.y && mouseY <= thumbRect.y + thumbRect.h) 
        {
            SelectedSpriteID = aliveSprints[i].id; // set selected sprite id
            break; 
        }
    } 
}
