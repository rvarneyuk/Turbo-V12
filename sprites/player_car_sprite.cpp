#include "sprites/player_car_sprite.h"
#include "sprites/sprite_utils.h"

namespace {
constexpr auto rows = std::array<std::string_view, PLAYER_CAR_HEIGHT>{
    "..........bbbb..........",
    ".........bYYYYb.........",
    "........bYYYYYYb........",
    "........bYYYYYYb........",
    ".......bYYYYYYYYb.......",
    ".......bYYYYYYYYb.......",
    "......bYYYwwwwYYYb......",
    ".....bYYYwwwwwwYYYb.....",
    ".....bYYYbbbbbbYYYb.....",
    "....bYYYbbbbbbbbYYYb....",
    "....bYYYbbbbbbbbYYYb....",
    "....bYYYrrrrrrrYYYb....",
    "....bYYYrrrrrrrYYYb....",
    "....bYYYbbbbbbbYYYb....",
    ".....bbbbb....bbbbb.....",
    "......bbb......bbb......"};

constexpr auto palette = std::array{
    std::pair{'b', rgba(10, 10, 10)},
    std::pair{'Y', rgba(255, 180, 0)},
    std::pair{'w', rgba(220, 220, 220)},
    std::pair{'r', rgba(120, 0, 0)},
};
} // namespace

const std::array<Uint32, PLAYER_CAR_WIDTH * PLAYER_CAR_HEIGHT> PLAYER_CAR_PIXELS =
    makeSpriteFromArt<PLAYER_CAR_WIDTH, PLAYER_CAR_HEIGHT, palette.size()>(rows, palette);

