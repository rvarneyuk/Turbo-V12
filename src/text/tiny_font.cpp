#include "text/tiny_font.h"

#include <array>
#include <cctype>
#include <string_view>

namespace {
TinyFontGlyph makeGlyph(const std::array<std::string_view, 7> &pattern) {
    TinyFontGlyph glyph{};
    for (size_t y = 0; y < 7; ++y) {
        uint8_t bits = 0;
        for (size_t x = 0; x < 5; ++x) {
            if (pattern[y][x] == '#') {
                bits |= static_cast<uint8_t>(1u << (4 - x));
            }
        }
        glyph.rows[y] = bits;
    }
    return glyph;
}

const TinyFontGlyph unknownGlyph = makeGlyph({"#####", "#...#", "#...#", "#...#", "#...#", "#...#", "#####"});

const auto glyphPairs = [] {
    return std::array{
        std::pair{'A', makeGlyph({".###.", "#...#", "#...#", "#####", "#...#", "#...#", "#...#"})},
    std::pair{'B', makeGlyph({"####.", "#...#", "#...#", "####.", "#...#", "#...#", "####."})},
    std::pair{'C', makeGlyph({".###.", "#...#", "#....", "#....", "#....", "#...#", ".###."})},
    std::pair{'D', makeGlyph({"####.", "#...#", "#...#", "#...#", "#...#", "#...#", "####."})},
    std::pair{'E', makeGlyph({"#####", "#....", "#....", "###..", "#....", "#....", "#####"})},
    std::pair{'F', makeGlyph({"#####", "#....", "#....", "###..", "#....", "#....", "#...."})},
    std::pair{'G', makeGlyph({".###.", "#...#", "#....", "#.###", "#...#", "#...#", ".###."})},
    std::pair{'H', makeGlyph({"#...#", "#...#", "#...#", "#####", "#...#", "#...#", "#...#"})},
    std::pair{'I', makeGlyph({"#####", "..#..", "..#..", "..#..", "..#..", "..#..", "#####"})},
    std::pair{'J', makeGlyph({"..###", "...#.", "...#.", "...#.", "#..#.", "#..#.", ".##.."})},
    std::pair{'K', makeGlyph({"#...#", "#..#.", "#.#..", "##...", "#.#..", "#..#.", "#...#"})},
    std::pair{'L', makeGlyph({"#....", "#....", "#....", "#....", "#....", "#....", "#####"})},
    std::pair{'M', makeGlyph({"#...#", "##.##", "#.#.#", "#...#", "#...#", "#...#", "#...#"})},
    std::pair{'N', makeGlyph({"#...#", "##..#", "#.#.#", "#..##", "#...#", "#...#", "#...#"})},
    std::pair{'O', makeGlyph({".###.", "#...#", "#...#", "#...#", "#...#", "#...#", ".###."})},
    std::pair{'P', makeGlyph({"####.", "#...#", "#...#", "####.", "#....", "#....", "#...."})},
    std::pair{'Q', makeGlyph({".###.", "#...#", "#...#", "#...#", "#.#.#", "#..#.", ".##.#"})},
    std::pair{'R', makeGlyph({"####.", "#...#", "#...#", "####.", "#.#..", "#..#.", "#...#"})},
    std::pair{'S', makeGlyph({".####", "#....", "#....", ".###.", "....#", "....#", "####."})},
    std::pair{'T', makeGlyph({"#####", "..#..", "..#..", "..#..", "..#..", "..#..", "..#.."})},
    std::pair{'U', makeGlyph({"#...#", "#...#", "#...#", "#...#", "#...#", "#...#", ".###."})},
    std::pair{'V', makeGlyph({"#...#", "#...#", "#...#", "#...#", "#...#", ".#.#.", "..#.."})},
    std::pair{'W', makeGlyph({"#...#", "#...#", "#...#", "#.#.#", "#.#.#", "##.##", "#...#"})},
    std::pair{'X', makeGlyph({"#...#", "#...#", ".#.#.", "..#..", ".#.#.", "#...#", "#...#"})},
    std::pair{'Y', makeGlyph({"#...#", "#...#", ".#.#.", "..#..", "..#..", "..#..", "..#.."})},
    std::pair{'Z', makeGlyph({"#####", "....#", "...#.", "..#..", ".#...", "#....", "#####"})},
    std::pair{'0', makeGlyph({".###.", "#...#", "#..##", "#.#.#", "##..#", "#...#", ".###."})},
    std::pair{'1', makeGlyph({"..#..", ".##..", "..#..", "..#..", "..#..", "..#..", ".###."})},
    std::pair{'2', makeGlyph({".###.", "#...#", "....#", "...#.", "..#..", ".#...", "#####"})},
    std::pair{'3', makeGlyph({".###.", "#...#", "....#", "..##.", "....#", "#...#", ".###."})},
    std::pair{'4', makeGlyph({"...#.", "..##.", ".#.#.", "#..#.", "#####", "...#.", "...#."})},
    std::pair{'5', makeGlyph({"#####", "#....", "#....", "####.", "....#", "#...#", ".###."})},
    std::pair{'6', makeGlyph({".###.", "#...#", "#....", "####.", "#...#", "#...#", ".###."})},
    std::pair{'7', makeGlyph({"#####", "....#", "...#.", "..#..", "..#..", "..#..", "..#.."})},
    std::pair{'8', makeGlyph({".###.", "#...#", "#...#", ".###.", "#...#", "#...#", ".###."})},
    std::pair{'9', makeGlyph({".###.", "#...#", "#...#", ".####", "....#", "#...#", ".###."})},
    std::pair{' ', makeGlyph({".....", ".....", ".....", ".....", ".....", ".....", "....."})},
    std::pair{':', makeGlyph({".....", "..#..", ".....", ".....", "..#..", ".....", "....."})},
    std::pair{'-', makeGlyph({".....", ".....", ".....", ".###.", ".....", ".....", "....."})},
    std::pair{'.', makeGlyph({".....", ".....", ".....", ".....", ".....", "..#..", "....."})},
    std::pair{'!', makeGlyph({"..#..", "..#..", "..#..", "..#..", "..#..", ".....", "..#.."})},
        std::pair{'?', makeGlyph({".###.", "#...#", "....#", "...#.", "..#..", ".....", "..#.."})},
    };
}();
} // namespace

const TinyFontGlyph &getTinyGlyph(char c) {
    char key = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    for (const auto &entry : glyphPairs) {
        if (entry.first == key) {
            return entry.second;
        }
    }
    return unknownGlyph;
}

void drawTinyText(SDL_Renderer *renderer, int x, int y, float scale, SDL_Color color, std::string_view text) {
    int cursorX = x;
    int cursorY = y;
    for (char c : text) {
        if (c == '\n') {
            cursorX = x;
            cursorY += static_cast<int>(scale * 8);
            continue;
        }
        const TinyFontGlyph &glyph = getTinyGlyph(c);
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        for (int row = 0; row < 7; ++row) {
            uint8_t bits = glyph.rows[row];
            for (int col = 0; col < 5; ++col) {
                if (bits & (1u << (4 - col))) {
                    SDL_Rect pixel{cursorX + static_cast<int>(col * scale),
                                    cursorY + static_cast<int>(row * scale),
                                    static_cast<int>(scale), static_cast<int>(scale)};
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }
        cursorX += static_cast<int>(scale * 6);
    }
}

