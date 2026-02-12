#include "logger.h"
#include "ui.h"
#include "color.h"
#include <algorithm>

void logger_log(Logger &logger, const std::string &text, LogLevel level)
{
    logger.entries.push_back({text, level});

    if (logger.entries.size() > 300)
        logger.entries.erase(logger.entries.begin());
}

void render_logger(SDL_Renderer *renderer, TTF_Font *font, const Logger &logger)
{
    // Background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &LOGGER_Box);

    // Border
    SDL_SetRenderDrawColor(renderer, 60, 30, 230, 255);
    SDL_RenderDrawRect(renderer, &LOGGER_Box);

    int lineHeight = TTF_FontHeight(font);
    int maxLines = LOGGER_Box.h / lineHeight;

    int start = std::max(
        0,
        (int)logger.entries.size() - maxLines - logger.scroll_offset);

    int y = LOGGER_Box.y + 5;

    for (int i = start; i < logger.entries.size() && y < LOGGER_Box.y + LOGGER_Box.h; i++)
    {

        SDL_Color color;
        switch (logger.entries[i].level)
        {
        case LogLevel::INFO:
            color = {40, 40, 40, 255};
            break;
        case LogLevel::WARN:
            color = {200, 140, 0, 255};
            break;
        case LogLevel::ERROR:
            color = {180, 30, 30, 255};
            break;
        }

        SDL_Texture *tex = renderText(
            renderer,
            font,
            logger.entries[i].text.c_str(),
            color);

        if (!tex)
            continue;

        int w, h;
        SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);

        SDL_Rect dst = {LOGGER_Box.x + 6, y, w, h};
        SDL_RenderCopy(renderer, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);

        y += lineHeight;
    }
}
