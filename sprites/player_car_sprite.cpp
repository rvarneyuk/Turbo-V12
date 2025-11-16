#include "sprites/player_car_sprite.h"
#include "sprites/sprite_utils.h"

namespace {
constexpr auto rows = std::array<std::string_view, PLAYER_CAR_HEIGHT>{
    "..................bbbbbbbbbbbb..................",
    ".................bbLLLLLLLLLLbb.................",
    "................bbLLYYYYYYYYLLbb................",
    "...............bbLLYYYYYYYYYYLLbb...............",
    "..............bbLLYYYYYYYYYYYYLLbb..............",
    ".............bbYYYYYYYYYYYYYYYYYYbb.............",
    "............bbYYYYYYYYYYYYYYYYYYYYbb............",
    "...........bbYYYYYYYYYYYYYYYYYYYYYYbb...........",
    "..........bbYYYYYYggggggggggggYYYYYYbb..........",
    ".........bbYYYYYYggggggggggggggYYYYYYbb.........",
    ".........bbYYYYYYggwwwwwwwwwwggYYYYYYbb.........",
    ".........bbYYYYYYggwwwwwwwwwwggYYYYYYbb.........",
    ".......bbYYYYYYYYggggggggggggggYYYYYYYYbb.......",
    ".......bbYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYbb.......",
    "......bbYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYbb......",
    "......bbYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYbb......",
    "......bbYYYYYYyyyyyyyyyyyyyyyyyyyyYYYYYYbb......",
    "......bbYYYYYYyyyyyyyyyyyyyyyyyyyyYYYYYYbb......",
    ".....bbYYYYYYyyyyyyyyyyyyyyyyyyyyyyYYYYYYbb.....",
    ".....bbYYYYYYyyyyyyyyyyyyyyyyyyyyyyYYYYYYbb.....",
    ".....bbYYYYYYssYYYYYYYYYYYYYYYYYYssYYYYYYbb.....",
    ".....bbYYYYYYssYYYYYYYYYYYYYYYYYYssYYYYYYbb.....",
    ".....bbYYYYYYssYYYYYYYYYYYYYYYYYYssYYYYYYbb.....",
    ".....bbYYYYYYssYYYYYYYYYYYYYYYYYYssYYYYYYbb.....",
    "......bbYYYYYYggggggggggggggggggggYYYYYYbb......",
    "......bbYYYYYYggggggggggggggggggggYYYYYYbb......",
    ".......bbYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYbb.......",
    "........bbYYYYYYYYYYYYYYYYYYYYYYYYYYYYbb........",
    "..........bbttYYYYYYYYYYYYYYYYYYYYttbb..........",
    "............bbttYYYYYYYYYYYYYYYYttbb............",
    ".............bbttBBBBBBBBBBBBBBttbb.............",
    "..............bbbbbbbbbbbbbbbbbbbb.............."};

constexpr auto palette = std::array{
    std::pair{'b', rgba(20, 20, 20)},
    std::pair{'B', rgba(55, 55, 55)},
    std::pair{'Y', rgba(255, 190, 20)},
    std::pair{'y', rgba(230, 150, 0)},
    std::pair{'g', rgba(90, 140, 190)},
    std::pair{'w', rgba(210, 235, 255)},
    std::pair{'L', rgba(255, 255, 200)},
    std::pair{'s', rgba(140, 110, 40)},
    std::pair{'t', rgba(255, 70, 60)},
};
} // namespace

const std::array<Uint32, PLAYER_CAR_WIDTH * PLAYER_CAR_HEIGHT> PLAYER_CAR_PIXELS =
    makeSpriteFromArt<PLAYER_CAR_WIDTH, PLAYER_CAR_HEIGHT, palette.size()>(rows, palette);

