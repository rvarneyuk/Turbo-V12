#pragma once

#include <SDL2/SDL.h>
#include <array>

constexpr int PLAYER_CAR_WIDTH = 48;
constexpr int PLAYER_CAR_HEIGHT = 32;

enum class PlayerCarFrame { HardLeft = 0, SoftLeft = 1, Straight = 2, SoftRight = 3, HardRight = 4 };

extern const std::array<const std::array<Uint32, PLAYER_CAR_WIDTH * PLAYER_CAR_HEIGHT> *, 5> PLAYER_CAR_FRAMES;

inline const std::array<Uint32, PLAYER_CAR_WIDTH * PLAYER_CAR_HEIGHT> &playerCarPixels(PlayerCarFrame frame) {
    return *PLAYER_CAR_FRAMES[static_cast<size_t>(frame)];
}

