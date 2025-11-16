#pragma once

#include <SDL2/SDL.h>
#include <array>

constexpr int TRAFFIC_CAR_WIDTH = 40;
constexpr int TRAFFIC_CAR_HEIGHT = 28;

extern const std::array<Uint32, TRAFFIC_CAR_WIDTH * TRAFFIC_CAR_HEIGHT> TRAFFIC_CAR_PIXELS_RED;
extern const std::array<Uint32, TRAFFIC_CAR_WIDTH * TRAFFIC_CAR_HEIGHT> TRAFFIC_CAR_PIXELS_BLUE;
extern const std::array<Uint32, TRAFFIC_CAR_WIDTH * TRAFFIC_CAR_HEIGHT> TRAFFIC_CAR_PIXELS_PURPLE;

