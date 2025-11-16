#pragma once

#include <SDL2/SDL.h>
#include <array>

struct TrafficSprite {
    const Uint32 *pixels;
    int width;
    int height;
    float scale;
};

extern const TrafficSprite TRAFFIC_RED_SPRITE;
extern const TrafficSprite TRAFFIC_BLUE_SPRITE;
extern const TrafficSprite TRAFFIC_PURPLE_SPRITE;
extern const TrafficSprite TRAFFIC_LORRY_SPRITE;
extern const TrafficSprite TRAFFIC_BUS_SPRITE;

