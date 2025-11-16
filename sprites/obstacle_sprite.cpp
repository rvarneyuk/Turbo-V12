#include "sprites/obstacle_sprite.h"
#include "sprites/sprite_utils.h"

namespace {
constexpr auto rows = std::array<std::string_view, OBSTACLE_HEIGHT>{
    "....rr....",
    "...rRRr...",
    "...rRRr...",
    "..rRRRRr..",
    "..rRRRRr..",
    "..rRRRRr..",
    "..rRRRRr..",
    "..rRRRRr..",
    "..rRRRRr..",
    "..rRRRRr..",
    "..rRRRRr..",
    "...bbbb...",
    "...bbbb...",
    "...bbbb...",
};

constexpr auto palette = std::array{
    std::pair{'r', rgba(240, 120, 60)},
    std::pair{'R', rgba(250, 240, 220)},
    std::pair{'b', rgba(50, 50, 50)},
};
} // namespace

const std::array<Uint32, OBSTACLE_WIDTH * OBSTACLE_HEIGHT> OBSTACLE_PIXELS =
    makeSpriteFromArt<OBSTACLE_WIDTH, OBSTACLE_HEIGHT, palette.size()>(rows, palette);

