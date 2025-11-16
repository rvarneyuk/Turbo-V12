#pragma once

#include <SDL2/SDL.h>
#include <array>
#include <string_view>
#include <utility>

constexpr Uint32 rgba(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255) {
    return (Uint32(a) << 24) | (Uint32(r) << 16) | (Uint32(g) << 8) | Uint32(b);
}

inline std::array<Uint32, 0> makeSpriteFromArt(std::string_view) = delete;

template <size_t Width, size_t Height, size_t PaletteSize>
constexpr std::array<Uint32, Width * Height> makeSpriteFromArt(
    const std::array<std::string_view, Height> &rows,
    const std::array<std::pair<char, Uint32>, PaletteSize> &palette,
    Uint32 defaultColor = 0) {
    std::array<Uint32, Width * Height> pixels{};
    for (size_t y = 0; y < Height; ++y) {
        for (size_t x = 0; x < Width; ++x) {
            char c = rows[y][x];
            Uint32 color = defaultColor;
            for (const auto &[symbol, value] : palette) {
                if (symbol == c) {
                    color = value;
                    break;
                }
            }
            pixels[y * Width + x] = color;
        }
    }
    return pixels;
}

