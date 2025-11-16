#include "sprites/traffic_car_sprite.h"
#include "sprites/sprite_utils.h"

namespace {
constexpr auto rows = std::array<std::string_view, TRAFFIC_CAR_HEIGHT>{
    ".....bb......bb.....",
    "....bWWb....bWWb....",
    "...bWWWWbbbbWWWWb...",
    "...bWWWWbbbbWWWWb...",
    "...bWWWWbbbbWWWWb...",
    "...bWWWWbbbbWWWWb...",
    "...bWWWWbbbbWWWWb...",
    "...bWWWWbbbbWWWWb...",
    "...bWWWWbbbbWWWWb...",
    "....bbbbbbbbbbbb....",
    "....bbbbbbbbbbbb....",
    ".....rrrrrrrrrr.....",
    ".....rrrrrrrrrr.....",
    ".....bbbbbbbbbb.....",
};

template <Uint8 R, Uint8 G, Uint8 B>
constexpr auto makeCar() {
    constexpr auto palette = std::array{
        std::pair{'b', rgba(30, 30, 30)},
        std::pair{'W', rgba(R, G, B)},
        std::pair{'r', rgba(180, 0, 0)},
    };
    return makeSpriteFromArt<TRAFFIC_CAR_WIDTH, TRAFFIC_CAR_HEIGHT, palette.size()>(rows, palette);
}
} // namespace

const std::array<Uint32, TRAFFIC_CAR_WIDTH * TRAFFIC_CAR_HEIGHT> TRAFFIC_CAR_PIXELS_RED =
    makeCar<220, 60, 60>();
const std::array<Uint32, TRAFFIC_CAR_WIDTH * TRAFFIC_CAR_HEIGHT> TRAFFIC_CAR_PIXELS_BLUE =
    makeCar<60, 90, 220>();
const std::array<Uint32, TRAFFIC_CAR_WIDTH * TRAFFIC_CAR_HEIGHT> TRAFFIC_CAR_PIXELS_PURPLE =
    makeCar<150, 90, 200>();

