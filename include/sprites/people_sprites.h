#pragma once

#include <SDL2/SDL.h>
#include <array>

constexpr int FLAG_PERSON_WIDTH = 24;
constexpr int FLAG_PERSON_HEIGHT = 40;
constexpr int CROWD_PERSON_WIDTH = 16;
constexpr int CROWD_PERSON_HEIGHT = 32;

extern const std::array<Uint32, FLAG_PERSON_WIDTH * FLAG_PERSON_HEIGHT> FLAG_PERSON_PIXELS;
extern const std::array<Uint32, CROWD_PERSON_WIDTH * CROWD_PERSON_HEIGHT> CROWD_PERSON_PIXELS;

