#pragma once

#include <SDL2/SDL.h>
#include <array>
#include <string_view>

struct TinyFontGlyph {
    std::array<uint8_t, 7> rows{};
};

const TinyFontGlyph &getTinyGlyph(char c);
void drawTinyText(SDL_Renderer *renderer, int x, int y, float scale, SDL_Color color, std::string_view text);

