#pragma once

#include <SDL2/SDL.h>
#include <array>

constexpr int OBSTACLE_WIDTH = 20;
constexpr int OBSTACLE_HEIGHT = 28;

extern const std::array<Uint32, OBSTACLE_WIDTH * OBSTACLE_HEIGHT> OBSTACLE_PIXELS;

