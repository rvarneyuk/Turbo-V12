#include "sprites/traffic_car_sprite.h"
#include "sprites/sprite_utils.h"

namespace {
constexpr int kCarWidth = 40;
constexpr int kCarHeight = 28;
// NOTE: keep these dimensions in sync with the ASCII art below. The previous
// values were larger than the actual sprite rows, which caused
// makeSpriteFromArt to read past the end of the strings during global
// initialization and crash before main().
constexpr int kBusWidth = 48;
constexpr int kBusHeight = 28;
constexpr int kLorryWidth = 48;
constexpr int kLorryHeight = 32;

constexpr auto carRows = std::array<std::string_view, kCarHeight>{
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

constexpr auto busRows = std::array<std::string_view, kBusHeight>{
    "............bbbbbbbbbbbbbbbbbbbbbbbb............",
    "............bbRRRRRRRRRRRRRRRRRRRRRR............",
    "............bbRRRRRRRRRRRRRRRRRRRRRR............",
    "............bbRRRRRRRRRRRRRRRRRRRRRR............",
    "............bbRRQQQQQQQQQQQQQQQQRRbb............",
    "............bRRQQQQQQQQQQQQQQQQQQRRb............",
    "............bRRQQQQQQQQQQQQQQQQQQRRb............",
    "............bRRQQQQQQQQQQQQQQQQQQRRb............",
    "...........bRRQQQQQQQQQQQQQQQQQQQQRRb...........",
    "...........bRRQQQQQQQQQQQQQQQQQQQQRRb...........",
    "...........bRRQQQQQQQQQQQQQQQQQQQQRRb...........",
    "...........bRRQQQQQQQQQQQQQQQQQQQQRRb...........",
    "...........bRRQQQQQQQQQQQQQQQQQQQQRRb...........",
    "...........bRRQQQQQQQQQQQQQQQQQQQQRRb...........",
    "...........bRRQQQQQQQQQQQQQQQQQQQQRRb...........",
    "...........bRRQQQQQQQQQQQQQQQQQQQQRRb...........",
    "............bRRrrrrrrrrrrrrrrrrrrRRb............",
    "............bRRrrrrrrrrrrrrrrrrrrRRb............",
    "............bRRrrrrrrrrrrrrrrrrrrRRb............",
    "............bRRrrrrrrrrrrrrrrrrrrRRb............",
    "............bRRrrrrrrrrrrrrrrrrrrRRb............",
    "............bRRrrrrrrrrrrrrrrrrrrRRb............",
    "............bbWWWWWWWWWWWWWWWWWWWWbb............",
    "............bbWWWWWWWWWWWWWWWWWWWWbb............",
    "............bbttttttttttttttttttttbb............",
    "............bbttttttttttttttttttttbb............",
    "............bbbbBBBBBBBBBBBBBBBBbbbb............",
    "............bbbbbbbbbbbbbbbbbbbbbbbb............"};

constexpr auto lorryRows = std::array<std::string_view, kLorryHeight>{
    ".........bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbGGGGGGGGGGGGGGGGGGGGGGGGGGbb.........",
    ".........bbbbmmmmmmmmmmmmmmmmmmmmmmmmbb.........",
    ".........bbbbmmmmmmmmmmmmmmmmmmmmmmmmbb.........",
    ".........bbTTTTTTTTTTTTTTTTTTTTTTTTTTbb.........",
    ".........bbTTTTTTTTTTTTTTTTTTTTTTTTTTbb.........",
    ".........bbttttttttttttttttttttttttttbb.........",
    ".........bbttttttttttttttttttttttttttbb.........",
    ".........bbbbBBBBBBBBBBBBBBBBBBBBBBBBbb.........",
    ".........bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb........."};

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
    return makeSpriteFromArt<kCarWidth, kCarHeight, palette.size()>(carRows, palette);
}

constexpr auto busPalette = std::array{
    std::pair{'b', rgba(25, 25, 25)},
    std::pair{'B', rgba(70, 70, 70)},
    std::pair{'R', rgba(250, 210, 60)},
    std::pair{'Q', rgba(220, 240, 255)},
    std::pair{'r', rgba(200, 80, 70)},
    std::pair{'W', rgba(255, 255, 255)},
    std::pair{'t', rgba(255, 90, 70)},
};

constexpr auto lorryPalette = std::array{
    std::pair{'b', rgba(30, 30, 30)},
    std::pair{'B', rgba(80, 80, 80)},
    std::pair{'G', rgba(220, 230, 235)},
    std::pair{'m', rgba(190, 90, 50)},
    std::pair{'T', rgba(60, 60, 60)},
    std::pair{'t', rgba(255, 90, 70)},
};

const std::array<Uint32, kCarWidth * kCarHeight> kRedPixels = makeCar<220, 60, 60>();
const std::array<Uint32, kCarWidth * kCarHeight> kBluePixels = makeCar<60, 90, 220>();
const std::array<Uint32, kCarWidth * kCarHeight> kPurplePixels = makeCar<150, 90, 200>();
const std::array<Uint32, kBusWidth * kBusHeight> kBusPixels =
    makeSpriteFromArt<kBusWidth, kBusHeight, busPalette.size()>(busRows, busPalette);
const std::array<Uint32, kLorryWidth * kLorryHeight> kLorryPixels =
    makeSpriteFromArt<kLorryWidth, kLorryHeight, lorryPalette.size()>(lorryRows, lorryPalette);
} // namespace

const TrafficSprite TRAFFIC_RED_SPRITE{kRedPixels.data(), kCarWidth, kCarHeight, 1.0f};
const TrafficSprite TRAFFIC_BLUE_SPRITE{kBluePixels.data(), kCarWidth, kCarHeight, 1.0f};
const TrafficSprite TRAFFIC_PURPLE_SPRITE{kPurplePixels.data(), kCarWidth, kCarHeight, 1.0f};
const TrafficSprite TRAFFIC_BUS_SPRITE{kBusPixels.data(), kBusWidth, kBusHeight, 1.18f};
const TrafficSprite TRAFFIC_LORRY_SPRITE{kLorryPixels.data(), kLorryWidth, kLorryHeight, 1.22f};

