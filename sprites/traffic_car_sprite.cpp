#include "sprites/traffic_car_sprite.h"
#include "sprites/sprite_utils.h"

namespace {
constexpr auto rows = std::array<std::string_view, TRAFFIC_CAR_HEIGHT>{
    "..............bbbbbbbbbbbb..............",
    "..............bbhhhhhhhhbb..............",
    "..............bbhhPPPPHHbb..............",
    "..............bbPPPPPPPPbb..............",
    "..............bbPPggggPPbb..............",
    "..............bbPPggggPPbb..............",
    "..............bPPPPPPPPPPb..............",
    "..............bPPPPssPPPPb..............",
    "..............bPPPPssPPPPb..............",
    "..............bPPPPPPPPPPb..............",
    ".............bPPPPkkkkPPPb..............",
    ".............bPPPPkkkkPPPb..............",
    "..............bPPPPPPPPPPb..............",
    "..............bPPPPPPPPPPb..............",
    "..............bPPPPPPPPPPb..............",
    "..............bPPPPPPPPPPb..............",
    "..............bPPPPPPPPPPb..............",
    "..............bPPPPPPPPPPb..............",
    "..............bPPPPPPPPPPb..............",
    "..............bPPPPPPPPPPb..............",
    "..............bPPPPPPPPPPb..............",
    "..............bPPPPPPPPPPb..............",
    "..............bPPPPPPPPPPb..............",
    "..............bPPPPPPPPPPb..............",
    "..............bbttPPPPttbb..............",
    "..............bbttPPPPttbb..............",
    "..............bbbbBBBBbbbb..............",
    "..............bbbbbbbbbbbb.............."};

template <Uint8 R, Uint8 G, Uint8 B>
constexpr auto makeCar() {
    constexpr auto palette = std::array{
        std::pair{'b', rgba(25, 25, 25)},
        std::pair{'B', rgba(70, 70, 70)},
        std::pair{'P', rgba(R, G, B)},
        std::pair{'k', rgba(90, 40, 40)},
        std::pair{'g', rgba(150, 210, 240)},
        std::pair{'s', rgba(230, 230, 230)},
        std::pair{'h', rgba(255, 250, 210)},
        std::pair{'H', rgba(255, 220, 150)},
        std::pair{'t', rgba(255, 90, 70)},
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

