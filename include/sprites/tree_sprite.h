#pragma once

#include <SDL2/SDL.h>
#include <array>

constexpr int TREE_WIDTH = 32;
constexpr int TREE_HEIGHT = 81;

extern const std::array<Uint32, TREE_WIDTH * TREE_HEIGHT> TREE_PIXELS;
