#include "logger.h"
#include "ui.h"
#include "color.h"
#include <algorithm>

static std::vector<LogEntry> entries;
static int scroll_offset = 0;

void logger_log(const std::string &text, LogLevel level)
{
    entries.push_back({text, level});

    if (entries.size() > 300)
        entries.erase(entries.begin());
}

void render_logger(SDL_Renderer *renderer, TTF_Font *font)
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
        (int)entries.size() - maxLines - scroll_offset);

    int y = LOGGER_Box.y + 5;

    for (int i = start; i < entries.size() && y < LOGGER_Box.y + LOGGER_Box.h; i++)
    {
        SDL_Color color;

        switch (entries[i].level)
        {
        case INFO:
            color = {50, 50, 50, 255};
            break;
        case WARN:
            color = {200, 140, 0, 255};
            break;
        case ERROR:
            color = {180, 30, 30, 255};
            break;
        }

        SDL_Texture *tex = renderText(
            renderer,
            font,
            entries[i].text.c_str(),
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
