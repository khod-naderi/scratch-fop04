#ifndef LOGGER_H
#define LOGGER_H

#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

enum LogLevel
{
    INFO,
    WARN,
    ERROR
};

struct LogEntry
{
    std::string text;
    LogLevel level;
};

struct Logger
{
    std::vector<LogEntry> entries;
    int scroll_offset = 0;
};

/* API */
void logger_log(const std::string &text, LogLevel level = LogLevel::INFO);
void render_logger(SDL_Renderer *renderer, TTF_Font *font);

#endif
