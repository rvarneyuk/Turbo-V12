#pragma once

#include <SDL2/SDL.h>
#include <array>

constexpr int PLAYER_CAR_WIDTH = 48;
constexpr int PLAYER_CAR_HEIGHT = 32;

extern const std::array<Uint32, PLAYER_CAR_WIDTH * PLAYER_CAR_HEIGHT> PLAYER_CAR_PIXELS;

