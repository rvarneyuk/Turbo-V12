#include "sprites/people_sprites.h"
#include "sprites/sprite_utils.h"

namespace {
constexpr auto flagRows = std::array<std::string_view, FLAG_PERSON_HEIGHT>{
    "....RRRRRR..",
    "....R..R....",
    "BBBBRRRRRR..",
    "BBBBR..R....",
    "..ssTTss....",
    "..ssTTss....",
    "...sTTs.....",
    "...sTTs.....",
    "...sTTs.....",
    "...sTTs.....",
    "...sTTs.....",
    "...sTTs.....",
    "...sTTs.....",
    "..bTTTTb....",
    "..bTTTTb....",
    "..bTTTTb....",
    "..bTTTTb....",
    "..bTTTTb....",
    "..bTT..b....",
    "..bTT..b....",
};

constexpr auto crowdRows = std::array<std::string_view, CROWD_PERSON_HEIGHT>{
    "..HHHH..",
    "..H..H..",
    "..HHHH..",
    "..H..H..",
    "..sTTs..",
    "..sTTs..",
    "..sTTs..",
    "..sTTs..",
    "..sTTs..",
    "..sTTs..",
    "..sTTs..",
    "..sTTs..",
    "..bTTb..",
    "..bTTb..",
    "..bTTb..",
    "..bTTb..",
};

constexpr auto flagPalette = std::array{
    std::pair{'R', rgba(200, 0, 0)},
    std::pair{'B', rgba(240, 240, 240)},
    std::pair{'s', rgba(255, 210, 150)},
    std::pair{'T', rgba(20, 60, 120)},
    std::pair{'b', rgba(40, 30, 20)},
};

constexpr auto crowdPalette = std::array{
    std::pair{'H', rgba(200, 0, 0)},
    std::pair{'s', rgba(230, 200, 150)},
    std::pair{'T', rgba(10, 120, 60)},
    std::pair{'b', rgba(50, 40, 30)},
};
} // namespace

const std::array<Uint32, FLAG_PERSON_WIDTH * FLAG_PERSON_HEIGHT> FLAG_PERSON_PIXELS =
    makeSpriteFromArt<FLAG_PERSON_WIDTH, FLAG_PERSON_HEIGHT, flagPalette.size()>(flagRows, flagPalette);

const std::array<Uint32, CROWD_PERSON_WIDTH * CROWD_PERSON_HEIGHT> CROWD_PERSON_PIXELS =
    makeSpriteFromArt<CROWD_PERSON_WIDTH, CROWD_PERSON_HEIGHT, crowdPalette.size()>(crowdRows, crowdPalette);

