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
This function is for adding and initializeing top bar of sprite box
this function will run only one time.
-------------------------------------------------
*/

void DrawTopBarOfSpriteBox(SDL_Renderer *renderer, TTF_Font *font, SDL_Rect topBar) {
    SDL_Color textColor = color_black;

    // 1. drawing "Sprite Label" text on top bar
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Sprite: ", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int labelX = topBar.x + 20; 
    int labelY = topBar.y + 20;

    SDL_Rect labelRect = {labelX, labelY, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &labelRect);

    // drawing a white rectangle for showing selected sprite name
    SDL_Rect nameBox = { labelRect.x + labelRect.w , labelRect.y - 5, 130, 30 };
    SDL_SetRenderDrawColor(renderer, color_white);
    SDL_RenderFillRect(renderer, &nameBox);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &nameBox);

    // 2. drawing X position 
    int xLabelX = labelX + textSurface->w + 10 + 150 + 20;
    int xLabelY = labelY;

    SDL_Surface* xLabelSurface = TTF_RenderText_Solid(font, "x ", textColor);
    SDL_Texture* xLabelTexture = SDL_CreateTextureFromSurface(renderer, xLabelSurface);
    SDL_Rect xLabelRect = {xLabelX, xLabelY, xLabelSurface->w, xLabelSurface->h};
    SDL_RenderCopy(renderer, xLabelTexture, nullptr, &xLabelRect);

    // X box
    SDL_Rect xBox = { xLabelRect.x + xLabelRect.w + 10 , xLabelRect.y - 6, 50, 30 };
    SDL_SetRenderDrawColor ( renderer, color_white);
    SDL_RenderFillRect(renderer, &xBox);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &xBox);

    // 3. drawing Y position
    int yLabelX = xBox.x + xBox.w + 20;
    int yLabelY = labelY;
    SDL_Surface* yLabelSurface = TTF_RenderText_Solid(font, "y ", textColor);
    SDL_Texture* yLabelTexture = SDL_CreateTextureFromSurface(renderer, yLabelSurface);
    SDL_Rect yLabelRect = {yLabelX, yLabelY, yLabelSurface->w, yLabelSurface->h};
    SDL_RenderCopy(renderer, yLabelTexture, nullptr, &yLabelRect);
    
    // Y box 
    SDL_Rect yBox = { yLabelRect.x + yLabelRect.w + 10 , yLabelRect.y - 6, 50, 30 };
    SDL_SetRenderDrawColor(renderer, color_white);
    SDL_RenderFillRect(renderer, &yBox);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &yBox);

    // 4. visibility box 
    int showLabelX = topBar.x + 20;
    int showLabelY = labelY + 50;

    SDL_Surface* showLabelSurface = TTF_RenderText_Solid(font, "Visibility", textColor);
    SDL_Texture* showLabelTexture = SDL_CreateTextureFromSurface(renderer, showLabelSurface);
    SDL_Rect showLabelRect = {showLabelX, showLabelY, showLabelSurface->w, showLabelSurface->h};
    SDL_RenderCopy(renderer, showLabelTexture, nullptr, &showLabelRect);

    // visibility box
    SDL_Rect showBox = { showLabelX + showLabelRect.w + 10 , showLabelRect.y - 6, 50, 30 };
    SDL_SetRenderDrawColor(renderer, color_white);
    SDL_RenderFillRect(renderer, &showBox);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &showBox);

    // 5. size box ( scale)

    int sizeLabelX = showLabelX + showBox.w + 30 + 50 + 30;
    int sizeLabelY = showLabelY;
    SDL_Surface* sizeLabelSurface = TTF_RenderText_Solid(font, "Size", textColor);
    SDL_Texture* sizeLabelTexture = SDL_CreateTextureFromSurface(renderer, sizeLabelSurface);
    SDL_Rect sizeLabelRect = {sizeLabelX, sizeLabelY, sizeLabelSurface->w, sizeLabelSurface->h};
    SDL_RenderCopy(renderer, sizeLabelTexture, nullptr, &sizeLabelRect);

    // size box
    SDL_Rect sizeBox = { sizeLabelRect.x + sizeLabelRect.w + 10 , sizeLabelRect.y - 6, 60, 30 };
    SDL_SetRenderDrawColor(renderer, color_white);
    SDL_RenderFillRect(renderer, &sizeBox);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &sizeBox);

    // 6. rotation box
    int rotationLabelX = sizeLabelX + sizeBox.w + 30 + 50 + 10;
    int rotationLabelY = sizeLabelY;

    SDL_Surface* rotationLabelSurface = TTF_RenderText_Solid(font, "Rotation", textColor);
    SDL_Texture* rotationLabelTexture = SDL_CreateTextureFromSurface(renderer, rotationLabelSurface);
    SDL_Rect rotationLabelRect = {rotationLabelX, rotationLabelY, rotationLabelSurface->w, rotationLabelSurface->h};
    SDL_RenderCopy(renderer, rotationLabelTexture, nullptr, &rotationLabelRect);

    // rotation box 
    SDL_Rect rotationBox = { rotationLabelRect.x + rotationLabelRect.w + 10 , rotationLabelRect.y - 6, 60, 30 };
    SDL_SetRenderDrawColor(renderer, color_white);
    SDL_RenderFillRect(renderer, &rotationBox);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &rotationBox);

    // end
    SDL_FreeSurface(textSurface);
    SDL_FreeSurface(xLabelSurface);
    SDL_FreeSurface(yLabelSurface);
    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(xLabelTexture);
    SDL_DestroyTexture(yLabelTexture);
}

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
    SDL_Rect topBar = { SPRITE_BOX.x, SPRITE_BOX.y, SPRITE_BOX.w, 100 };
    SDL_SetRenderDrawColor(renderer, 211, 211, 211, 255);
    SDL_RenderFillRect(renderer, &topBar);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &topBar);

    // Sprite Area
    SDL_Rect spriteArea = { SPRITE_BOX.x, topBar.y + topBar.h, SPRITE_BOX.w, SPRITE_BOX.h - topBar.h };
    SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
    SDL_RenderFillRect(renderer, &spriteArea);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &spriteArea);

    // Draw top bar of sprite box
    DrawTopBarOfSpriteBox(renderer, font, topBar);

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

        // if selected
        bool isSelected = ( SelectedSpriteID == sprite.id );                   
        
        // background color 
        SDL_SetRenderDrawColor( renderer , 230, 230, 230, 255);
        SDL_RenderFillRect(renderer, &thumbRect);                     

        // image of thumbnail
        SDL_RenderCopy (renderer, sprite.nowCustome, nullptr, &thumbRect);

        // hovering effect
        if ( isSelected ) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0 , 255); // color = red for selected sprite

            // Drawin a thick border for selected sprite
            SDL_Rect border1 = thumbRect;
            SDL_RenderDrawRect(renderer, &border1);
            SDL_Rect border2 = { thumbRect.x - 1, thumbRect.y - 1, thumbRect.w + 2, thumbRect.h + 2 };
            SDL_RenderDrawRect(renderer, &border2);
            SDL_Rect border3 = { thumbRect.x - 2, thumbRect.y - 2, thumbRect.w + 4, thumbRect.h + 4 };
            SDL_RenderDrawRect(renderer, &border3);
            SDL_Rect border4 = { thumbRect.x - 3, thumbRect.y - 3, thumbRect.w + 6, thumbRect.h + 6 };
            SDL_RenderDrawRect(renderer, &border4);
            SDL_Rect border5 = { thumbRect.x - 4, thumbRect.y - 4, thumbRect.w + 8, thumbRect.h + 8 };
            SDL_RenderDrawRect(renderer, &border5); 
        }
        else if (ishovered) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 100, 150, 255, 80); // blue with some transparency
            SDL_RenderFillRect(renderer, &thumbRect);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }

        // Draw border
        SDL_SetRenderDrawColor(renderer, color_black);
        SDL_RenderDrawRect(renderer, &thumbRect);

        // hovering border & selected border
        if (ishovered) {
            SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255); // blue = for hovered sprite
            SDL_RenderDrawRect(renderer, &thumbRect);
        } 
        else {
            SDL_SetRenderDrawColor(renderer, color_black);
            SDL_RenderDrawRect(renderer, &thumbRect);
        }

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
