#include <SDL2/SDL.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <random>
#include <string>
#include <vector>

#include "sprites/obstacle_sprite.h"
#include "sprites/people_sprites.h"
#include "sprites/player_car_sprite.h"
#include "sprites/traffic_car_sprite.h"
#include "text/tiny_font.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace {
constexpr int kScreenWidth = 960;
constexpr int kScreenHeight = 540;
constexpr float kLevelLength = 3200.0f;
constexpr float kLevelTimerSeconds = 120.0f;
constexpr float kMaxSpeed = 220.0f;
constexpr float kAccelRate = 200.0f;
constexpr float kBrakeRate = 320.0f;
constexpr float kDragForce = 85.0f;
constexpr float kRoadNear = kScreenWidth * 0.45f;
constexpr float kRoadFar = kScreenWidth * 0.08f;
constexpr int kHorizonY = kScreenHeight / 3;

float lerp(float a, float b, float t) { return a + (b - a) * t; }

struct LevelTheme {
    SDL_Color skyTop;
    SDL_Color skyBottom;
    SDL_Color road;
    SDL_Color roadEdge;
    SDL_Color horizonAccent;
    SDL_Color text;
};

enum class HorizonStyle { Sun, Mountains, City, Aurora, Desert };

struct LevelDescription {
    LevelTheme theme;
    HorizonStyle horizon;
    std::string name;
};

const std::array<LevelDescription, 5> kLevels{LevelDescription{
                                                   {{90, 160, 255, 255}, {10, 30, 80, 255}, {60, 60, 60, 255},
                                                    {220, 220, 220, 255}, {255, 190, 60, 255}, {255, 255, 255, 255}},
                                                   HorizonStyle::Sun,
                                                   "AZURE COAST"},
                                               LevelDescription{{{180, 90, 255, 255}, {20, 0, 40, 255}, {40, 40, 50, 255},
                                                                  {255, 140, 220, 255}, {60, 255, 180, 255}, {255, 230, 255, 255}},
                                                                  HorizonStyle::Aurora,
                                                                  "NEON NIGHT"},
                                               LevelDescription{{{255, 210, 140, 255}, {200, 120, 60, 255}, {80, 60, 40, 255},
                                                                  {255, 255, 255, 255}, {255, 255, 200, 255}, {60, 30, 0, 255}},
                                                                  HorizonStyle::Desert,
                                                                  "SAHARA SALSA"},
                                               LevelDescription{{{120, 200, 255, 255}, {40, 90, 140, 255}, {50, 50, 50, 255},
                                                                  {200, 200, 200, 255}, {255, 110, 110, 255}, {255, 255, 255, 255}},
                                                                  HorizonStyle::City,
                                                                  "MEGACITY DASH"},
                                               LevelDescription{{{120, 200, 120, 255}, {10, 60, 10, 255}, {40, 60, 40, 255},
                                                                  {255, 255, 255, 255}, {220, 80, 60, 255}, {230, 255, 230, 255}},
                                                                  HorizonStyle::Mountains,
                                                                  "ALPINE SMOKE"}};

SDL_Color lerpColor(SDL_Color a, SDL_Color b, float t) {
    SDL_Color out{};
    out.r = static_cast<Uint8>(std::clamp(lerp(a.r, b.r, t), 0.0f, 255.0f));
    out.g = static_cast<Uint8>(std::clamp(lerp(a.g, b.g, t), 0.0f, 255.0f));
    out.b = static_cast<Uint8>(std::clamp(lerp(a.b, b.b, t), 0.0f, 255.0f));
    out.a = 255;
    return out;
}

struct Projection {
    int x = 0;
    int y = 0;
    float scale = 1.0f;
    bool visible = false;
};

float curvaturePattern(float distance) {
    float scaled = distance * 0.0025f;
    return std::sin(scaled * 0.7f) * 0.6f + std::sin(scaled * 1.1f + 1.0f) * 0.4f;
}

float roadCenterOffset(float distance) { return curvaturePattern(distance) * (kScreenWidth * 0.18f); }

Projection project(float lane, float z, float playerDistance) {
    constexpr float minZ = 20.0f;
    constexpr float maxZ = 400.0f;
    float clamped = std::clamp((z - minZ) / (maxZ - minZ), 0.0f, 1.0f);
    float depth = 1.0f - clamped;
    Projection p;
    p.visible = z >= minZ && z <= maxZ;
    p.y = static_cast<int>(kHorizonY + depth * (kScreenHeight - kHorizonY - 40));
    float roadHalf = lerp(kRoadFar, kRoadNear, depth);
    float nearOffset = roadCenterOffset(playerDistance);
    float farOffset = roadCenterOffset(playerDistance + z * 1.8f);
    float center = kScreenWidth / 2 + lerp(farOffset, nearOffset, depth);
    p.x = static_cast<int>(center + lane * roadHalf);
    p.scale = lerp(0.35f, 3.2f, depth);
    return p;
}

struct SmokeParticle {
    float x;
    float y;
    float vx;
    float vy;
    float life;
    float maxLife;
};

struct TrafficCar {
    float lane = 0.0f;
    float z = 200.0f;
    float speed = 60.0f;
    float swerveTimer = 0.0f;
    float swerveDirection = 0.0f;
    const std::array<Uint32, TRAFFIC_CAR_WIDTH * TRAFFIC_CAR_HEIGHT> *sprite = &TRAFFIC_CAR_PIXELS_RED;
};

struct RoadObstacle {
    float z = 100.0f;
    bool left = true;
};

enum class GameState { Welcome, StartingLine, Playing, LevelComplete, LevelFailed, GameWon };

struct DieselAudio {
    SDL_AudioDeviceID device = 0;
    SDL_AudioSpec obtained{};
    std::atomic<float> throttle{0.0f};
    std::atomic<float> horn{0.0f};
    float enginePhase = 0.0f;
    float chatterPhase = 0.0f;
    float hornPhase = 0.0f;
    float melodyPhase = 0.0f;
    float harmonyPhase = 0.0f;
    float melodyTimer = 0.0f;
    size_t melodyIndex = 0;
    uint32_t noiseState = 0x12345678u;
    inline static constexpr std::array<float, 12> kMelodyNotes{
        174.61f, 196.0f, 220.0f, 246.94f, 196.0f, 220.0f, 261.63f, 246.94f, 220.0f, 196.0f, 174.61f, 155.56f};
    inline static constexpr float kMelodyNoteDuration = 0.35f;

    static void callback(void *userdata, Uint8 *stream, int len) {
        auto *self = static_cast<DieselAudio *>(userdata);
        float *buffer = reinterpret_cast<float *>(stream);
        int samples = len / sizeof(float);
        float throttle = self->throttle.load();
        float horn = self->horn.load();
        if (self->obtained.freq == 0) {
            std::fill(buffer, buffer + samples, 0.0f);
            return;
        }
        for (int i = 0; i < samples; ++i) {
            float engineFreq = 40.0f + throttle * 210.0f;
            self->enginePhase += engineFreq / static_cast<float>(self->obtained.freq);
            if (self->enginePhase > 1.0f) self->enginePhase -= 1.0f;
            float engine = std::sin(self->enginePhase * 2.0f * static_cast<float>(M_PI));

            float chatterFreq = 6.0f + throttle * 10.0f;
            self->chatterPhase += chatterFreq / static_cast<float>(self->obtained.freq);
            if (self->chatterPhase > 1.0f) self->chatterPhase -= 1.0f;
            float chatter = self->chatterPhase < 0.5f ? 1.0f : -1.0f;

            self->noiseState = self->noiseState * 1664525u + 1013904223u;
            float noise = static_cast<float>((self->noiseState >> 9) & 0x7FFFFF) / static_cast<float>(0x7FFFFF);
            noise = noise * 2.0f - 1.0f;

            self->hornPhase += (350.0f + horn * 150.0f) / static_cast<float>(self->obtained.freq);
            if (self->hornPhase > 1.0f) self->hornPhase -= 1.0f;
            float hornWave = std::sin(self->hornPhase * 2.0f * static_cast<float>(M_PI));

            self->melodyTimer += 1.0f / static_cast<float>(self->obtained.freq);
            if (self->melodyTimer >= kMelodyNoteDuration) {
                self->melodyTimer -= kMelodyNoteDuration;
                self->melodyIndex = (self->melodyIndex + 1) % kMelodyNotes.size();
            }
            float melodyFreq = kMelodyNotes[self->melodyIndex];
            self->melodyPhase += melodyFreq / static_cast<float>(self->obtained.freq);
            self->harmonyPhase += (melodyFreq * 1.5f) / static_cast<float>(self->obtained.freq);
            if (self->melodyPhase > 1.0f) self->melodyPhase -= 1.0f;
            if (self->harmonyPhase > 1.0f) self->harmonyPhase -= 1.0f;
            float melody = (std::sin(self->melodyPhase * 2.0f * static_cast<float>(M_PI)) +
                            0.5f * std::sin(self->harmonyPhase * 2.0f * static_cast<float>(M_PI))) *
                           0.18f;

            float sample = engine * (0.4f + throttle * 0.3f) + chatter * 0.15f + noise * 0.05f + melody;
            sample += horn * hornWave * 0.5f;
            buffer[i] = sample * 0.3f;
        }
    }

    bool init() {
        SDL_AudioSpec desired{};
        desired.freq = 44100;
        desired.format = AUDIO_F32SYS;
        desired.channels = 1;
        desired.samples = 1024;
        desired.callback = &DieselAudio::callback;
        desired.userdata = this;
        device = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
        if (!device) return false;
        SDL_PauseAudioDevice(device, 0);
        return true;
    }

    ~DieselAudio() { if (device) SDL_CloseAudioDevice(device); }
};

template <size_t Size>
void drawSprite(SDL_Renderer *renderer, int x, int y, float scale,
                const std::array<Uint32, Size> &pixels, int width, int height) {
    for (int py = 0; py < height; ++py) {
        for (int px = 0; px < width; ++px) {
            Uint32 pixel = pixels[py * width + px];
            Uint8 a = static_cast<Uint8>((pixel >> 24) & 0xFF);
            if (a == 0) continue;
            Uint8 r = static_cast<Uint8>((pixel >> 16) & 0xFF);
            Uint8 g = static_cast<Uint8>((pixel >> 8) & 0xFF);
            Uint8 b = static_cast<Uint8>(pixel & 0xFF);
            SDL_SetRenderDrawColor(renderer, r, g, b, a);
            SDL_Rect rect{x + static_cast<int>(px * scale), y + static_cast<int>(py * scale),
                          std::max(1, static_cast<int>(scale + 0.5f)), std::max(1, static_cast<int>(scale + 0.5f))};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void drawBackground(SDL_Renderer *renderer, const LevelTheme &theme) {
    for (int y = 0; y < kScreenHeight; ++y) {
        float t = static_cast<float>(y) / kScreenHeight;
        SDL_Color color = lerpColor(theme.skyTop, theme.skyBottom, t);
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
        SDL_RenderDrawLine(renderer, 0, y, kScreenWidth, y);
    }
}

void drawCircle(SDL_Renderer *renderer, int cx, int cy, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int y = -radius; y <= radius; ++y) {
        int dx = static_cast<int>(std::sqrt(static_cast<float>(radius * radius - y * y)));
        SDL_RenderDrawLine(renderer, cx - dx, cy + y, cx + dx, cy + y);
    }
}

void drawTriangle(SDL_Renderer *renderer, SDL_Point apex, int width, int height, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int y = 0; y < height; ++y) {
        float t = static_cast<float>(y) / height;
        int halfWidth = static_cast<int>((width / 2) * t);
        SDL_RenderDrawLine(renderer, apex.x - halfWidth, apex.y + y, apex.x + halfWidth, apex.y + y);
    }
}

void drawParallaxObjects(SDL_Renderer *renderer, const LevelDescription &level, float playerDistance) {
    std::array<float, 3> speeds{0.02f, 0.05f, 0.09f};
    for (size_t layer = 0; layer < speeds.size(); ++layer) {
        float speed = speeds[layer];
        float offset = std::fmod(playerDistance * speed, 200.0f);
        int baseY = kHorizonY - 25 - static_cast<int>(layer) * 16;
        SDL_Color color = level.theme.horizonAccent;
        color.a = static_cast<Uint8>(200 - layer * 40);
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        for (int x = -200; x < kScreenWidth + 200; x += 80 - static_cast<int>(layer) * 10) {
            int posX = x - static_cast<int>(offset) + static_cast<int>(roadCenterOffset(playerDistance + layer * 120.0f) * (0.2f + layer * 0.15f));
            SDL_Rect rect{posX, baseY - 20, 10 + static_cast<int>(layer) * 4, 20 + static_cast<int>(layer) * 6};
            SDL_RenderFillRect(renderer, &rect);
            SDL_RenderDrawLine(renderer, posX + rect.w / 2, baseY - 20, posX + rect.w / 2,
                               baseY - 35 - static_cast<int>(layer) * 4);
        }
    }
}

void drawHorizon(SDL_Renderer *renderer, const LevelDescription &level, float anim, float playerDistance) {
    float horizonShift = roadCenterOffset(playerDistance + 500.0f) * 0.25f;
    switch (level.horizon) {
        case HorizonStyle::Sun:
            drawCircle(renderer, kScreenWidth / 2 + static_cast<int>(horizonShift), kHorizonY - 40, 40,
                       level.theme.horizonAccent);
            break;
        case HorizonStyle::Aurora: {
            SDL_Color accent = level.theme.horizonAccent;
            for (int i = 0; i < 5; ++i) {
                int waveY = kHorizonY - 60 - i * 12;
                SDL_SetRenderDrawColor(renderer, accent.r, accent.g, accent.b, 80 + i * 20);
                for (int x = 0; x < kScreenWidth; ++x) {
                    float offset = std::sin((x * 0.01f) + anim * 2.0f + i) * 20.0f;
                    SDL_RenderDrawPoint(renderer, x + static_cast<int>(horizonShift), waveY + static_cast<int>(offset));
                }
            }
            break;
        }
        case HorizonStyle::Desert:
            drawTriangle(renderer, SDL_Point{kScreenWidth / 3 + static_cast<int>(horizonShift), kHorizonY - 10}, 220, 90,
                         level.theme.horizonAccent);
            drawTriangle(renderer, SDL_Point{2 * kScreenWidth / 3 + static_cast<int>(horizonShift), kHorizonY - 20}, 260,
                         110, level.theme.horizonAccent);
            break;
        case HorizonStyle::City:
            SDL_SetRenderDrawColor(renderer, level.theme.horizonAccent.r, level.theme.horizonAccent.g,
                                   level.theme.horizonAccent.b, 255);
            for (int i = 0; i < 20; ++i) {
                int buildingWidth = 20 + (i % 3) * 10;
                int buildingHeight = 40 + ((i * 13) % 70);
                int x = i * (kScreenWidth / 20) + static_cast<int>(horizonShift * 0.7f);
                SDL_Rect rect{x, kHorizonY - buildingHeight, buildingWidth, buildingHeight};
                SDL_RenderFillRect(renderer, &rect);
            }
            break;
        case HorizonStyle::Mountains:
            drawTriangle(renderer, SDL_Point{kScreenWidth / 4 + static_cast<int>(horizonShift * 0.5f), kHorizonY - 5}, 200,
                         120, level.theme.horizonAccent);
            drawTriangle(renderer, SDL_Point{kScreenWidth / 2 + static_cast<int>(horizonShift * 0.6f), kHorizonY - 15}, 260,
                         150, level.theme.horizonAccent);
            drawTriangle(renderer, SDL_Point{3 * kScreenWidth / 4 + static_cast<int>(horizonShift * 0.7f), kHorizonY - 5},
                         200, 100, level.theme.horizonAccent);
            break;
    }
    drawParallaxObjects(renderer, level, playerDistance);
}

void drawRoad(SDL_Renderer *renderer, const LevelDescription &level, float playerDistance) {
    float nearOffset = roadCenterOffset(playerDistance);
    for (int y = kHorizonY; y < kScreenHeight; ++y) {
        float t = static_cast<float>(y - kHorizonY) / (kScreenHeight - kHorizonY);
        float roadHalf = lerp(kRoadFar, kRoadNear, t);
        float depth = 1.0f - t;
        float aheadDistance = playerDistance + depth * 600.0f;
        float centerOffset = lerp(roadCenterOffset(aheadDistance), nearOffset, t);
        int center = static_cast<int>(kScreenWidth / 2 + centerOffset);
        int left = center - static_cast<int>(roadHalf);
        int right = center + static_cast<int>(roadHalf);
        SDL_SetRenderDrawColor(renderer, level.theme.road.r, level.theme.road.g, level.theme.road.b, 255);
        SDL_RenderDrawLine(renderer, left, y, right, y);
        SDL_SetRenderDrawColor(renderer, level.theme.roadEdge.r, level.theme.roadEdge.g, level.theme.roadEdge.b, 255);
        SDL_RenderDrawLine(renderer, left - 4, y, left, y);
        SDL_RenderDrawLine(renderer, right, y, right + 4, y);
        if (((static_cast<int>(playerDistance * 0.2f + t * 120) / 6) % 2) == 0) {
            int stripeCenter = kScreenWidth / 2;
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawLine(renderer, stripeCenter - 4 + static_cast<int>(centerOffset * 0.5f), y,
                               stripeCenter + 4 + static_cast<int>(centerOffset * 0.5f), y);
        }
    }
}

std::string formatTime(float timeLeft) {
    if (timeLeft < 0) timeLeft = 0;
    int total = static_cast<int>(std::ceil(timeLeft));
    int minutes = total / 60;
    int seconds = total % 60;
    char buffer[8];
    std::snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
    return buffer;
}

const std::array<Uint32, TRAFFIC_CAR_WIDTH * TRAFFIC_CAR_HEIGHT> &randomTrafficSprite(std::mt19937 &rng) {
    switch (rng() % 3) {
        case 0:
            return TRAFFIC_CAR_PIXELS_RED;
        case 1:
            return TRAFFIC_CAR_PIXELS_BLUE;
        default:
            return TRAFFIC_CAR_PIXELS_PURPLE;
    }
}

void resetTrafficCar(TrafficCar &car, std::mt19937 &rng, float minZ) {
    std::uniform_real_distribution<float> laneDist(-0.6f, 0.6f);
    std::uniform_real_distribution<float> zDist(minZ + 40.0f, minZ + 280.0f);
    std::uniform_real_distribution<float> speedDist(70.0f, 130.0f);
    car.lane = laneDist(rng);
    car.z = zDist(rng);
    car.speed = speedDist(rng);
    car.sprite = &randomTrafficSprite(rng);
    car.swerveDirection = 0.0f;
    car.swerveTimer = 0.0f;
}

void resetObstacle(RoadObstacle &obstacle, std::mt19937 &rng, float offset) {
    std::uniform_real_distribution<float> zDist(offset + 60.0f, offset + 320.0f);
    obstacle.z = zDist(rng);
    obstacle.left = rng() % 2 == 0;
}

} // namespace

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        SDL_Log("SDL init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Single Turbo D4", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                         kScreenWidth, kScreenHeight, 0);
    if (!window) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    DieselAudio audio;
    audio.init();

    std::mt19937 rng{std::random_device{}()};
    std::vector<TrafficCar> traffic(6);
    std::vector<RoadObstacle> obstacles(8);
    std::vector<SmokeParticle> smoke;
    smoke.reserve(200);

    int currentLevel = 0;
    float levelProgress = 0.0f;
    float levelTimer = kLevelTimerSeconds;
    float playerSpeed = 0.0f;
    float playerLane = 0.0f;
    float hornTimer = 0.0f;
    GameState state = GameState::Welcome;
    float stateTimer = 0.0f;

    auto setupLevel = [&](int levelIndex) {
        currentLevel = levelIndex;
        levelProgress = 0.0f;
        levelTimer = kLevelTimerSeconds;
        playerSpeed = 0.0f;
        playerLane = 0.0f;
        hornTimer = 0.0f;
        for (size_t i = 0; i < traffic.size(); ++i) {
            resetTrafficCar(traffic[i], rng, 100.0f + static_cast<float>(i) * 40.0f);
            traffic[i].z += i * 50.0f;
        }
        for (size_t i = 0; i < obstacles.size(); ++i) {
            resetObstacle(obstacles[i], rng, static_cast<float>(i) * 80.0f);
        }
    };

    setupLevel(0);

    bool running = true;
    uint32_t lastTicks = SDL_GetTicks();
    bool leftPressed = false;
    bool rightPressed = false;
    bool upPressed = false;
    bool downPressed = false;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN && !event.key.repeat) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) leftPressed = true;
                if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) rightPressed = true;
                if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) upPressed = true;
                if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) downPressed = true;
                if (event.key.keysym.sym == SDLK_SPACE) {
                    hornTimer = 0.75f;
                    audio.horn.store(1.0f);
                    for (auto &car : traffic) {
                        car.swerveTimer = 1.0f;
                        car.swerveDirection = (car.lane < playerLane) ? -1.0f : 1.0f;
                    }
                }
                if (event.key.keysym.sym == SDLK_RETURN) {
                    if (state == GameState::Welcome) {
                        state = GameState::StartingLine;
                        stateTimer = 3.0f;
                        setupLevel(0);
                    } else if (state == GameState::LevelComplete) {
                        if (currentLevel + 1 < static_cast<int>(kLevels.size())) {
                            setupLevel(currentLevel + 1);
                            state = GameState::StartingLine;
                            stateTimer = 3.0f;
                        } else {
                            state = GameState::GameWon;
                            stateTimer = 0.0f;
                        }
                    } else if (state == GameState::LevelFailed) {
                        setupLevel(currentLevel);
                        state = GameState::StartingLine;
                        stateTimer = 3.0f;
                    }
                }
            } else if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) leftPressed = false;
                if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) rightPressed = false;
                if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) upPressed = false;
                if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) downPressed = false;
                if (event.key.keysym.sym == SDLK_SPACE) {
                    audio.horn.store(0.0f);
                }
            }
        }

        uint32_t now = SDL_GetTicks();
        float dt = (now - lastTicks) / 1000.0f;
        if (dt > 0.05f) dt = 0.05f;
        lastTicks = now;

        const LevelDescription &level = kLevels[currentLevel];

        if (hornTimer > 0.0f) {
            hornTimer -= dt;
            if (hornTimer <= 0.0f) {
                audio.horn.store(0.0f);
            }
        }

        switch (state) {
            case GameState::Welcome:
                break;
            case GameState::StartingLine:
                stateTimer -= dt;
                if (stateTimer <= 0.0f) {
                    state = GameState::Playing;
                }
                break;
            case GameState::Playing: {
                float steer = 0.0f;
                if (leftPressed) steer -= 1.0f;
                if (rightPressed) steer += 1.0f;
                playerLane += steer * dt * 1.1f;
                float curveInfluence = (roadCenterOffset(levelProgress + 120.0f) - roadCenterOffset(levelProgress)) /
                                       (kScreenWidth * 0.18f);
                playerLane -= curveInfluence * dt * (playerSpeed / kMaxSpeed) * 0.6f;
                playerLane = std::clamp(playerLane, -1.0f, 1.0f);

                float acceleration = 0.0f;
                if (upPressed) acceleration += kAccelRate;
                if (downPressed) acceleration -= kBrakeRate;
                acceleration -= kDragForce;
                playerSpeed += acceleration * dt;
                playerSpeed = std::clamp(playerSpeed, 0.0f, kMaxSpeed);
                float speedRatio = (kMaxSpeed > 0.0f) ? (playerSpeed / kMaxSpeed) : 0.0f;
                levelProgress += playerSpeed * dt;
                levelTimer -= dt;
                audio.throttle.store(speedRatio);

                for (auto &car : traffic) {
                    float relativeSpeed = playerSpeed - car.speed;
                    car.z -= relativeSpeed * dt;
                    if (car.swerveTimer > 0.0f) {
                        car.lane += car.swerveDirection * dt * 0.5f;
                        car.swerveTimer -= dt;
                    }
                    car.lane = std::clamp(car.lane, -0.95f, 0.95f);
                    if (car.z < 20.0f) {
                        resetTrafficCar(car, rng, 200.0f);
                        car.z += 200.0f;
                    }
                    if (car.z < 90.0f && car.z > 30.0f) {
                        float laneDiff = std::abs(car.lane - playerLane);
                        if (laneDiff < 0.12f) {
                            playerSpeed = std::min(playerSpeed, kMaxSpeed * 0.6f);
                            car.z += 60.0f;
                            for (int i = 0; i < 6; ++i) {
                                smoke.push_back(SmokeParticle{static_cast<float>(kScreenWidth / 2),
                                                              static_cast<float>(kScreenHeight - 160),
                                                              (rng() % 100 - 50) * 0.2f,
                                                              -30.0f - (rng() % 40), 0.0f, 0.8f});
                            }
                        }
                    }
                }

                for (auto &obstacle : obstacles) {
                    obstacle.z -= playerSpeed * dt;
                    if (obstacle.z < 30.0f) {
                        resetObstacle(obstacle, rng, 200.0f);
                        obstacle.z += 200.0f;
                    }
                    float laneEdge = obstacle.left ? -0.95f : 0.95f;
                    if (obstacle.z < 80.0f && obstacle.z > 30.0f) {
                        if (std::abs(playerLane - laneEdge) < 0.1f) {
                            playerSpeed = std::min(playerSpeed, kMaxSpeed * 0.5f);
                        }
                    }
                }

                if (std::abs(playerLane) > 0.92f) {
                    playerSpeed = std::min(playerSpeed, kMaxSpeed * 0.7f);
                }

                if (levelTimer <= 0.0f) {
                    state = GameState::LevelFailed;
                    stateTimer = 3.0f;
                }

                if (levelProgress >= kLevelLength) {
                    state = GameState::LevelComplete;
                    stateTimer = 0.0f;
                }
                break;
            }
            case GameState::LevelComplete:
                break;
            case GameState::LevelFailed:
                stateTimer -= dt;
                if (stateTimer <= 0.0f) {
                    setupLevel(currentLevel);
                    state = GameState::StartingLine;
                    stateTimer = 3.0f;
                }
                break;
            case GameState::GameWon:
                break;
        }

        // Smoke update
        float roadCenter = kScreenWidth / 2 + roadCenterOffset(levelProgress);
        int playerScreenX = static_cast<int>(roadCenter + playerLane * kRoadNear);
        int playerScreenY = kScreenHeight - 150;
        float speedRatio = (kMaxSpeed > 0.0f) ? (playerSpeed / kMaxSpeed) : 0.0f;
        if (state == GameState::Playing) {
            int spawnCount = 2 + static_cast<int>(speedRatio * 6.0f);
            for (int i = 0; i < spawnCount; ++i) {
                smoke.push_back(SmokeParticle{static_cast<float>(playerScreenX + (rng() % 30 - 15)),
                                              static_cast<float>(playerScreenY + 20), (rng() % 40 - 20) * (1.0f + speedRatio),
                                              -30.0f - (rng() % 20) - speedRatio * 60.0f, 0.0f,
                                              0.7f + speedRatio * 0.9f});
            }
        }
        for (auto &particle : smoke) {
            particle.life += dt;
            particle.x += particle.vx * dt;
            particle.y += particle.vy * dt;
            particle.vy -= 10.0f * dt;
        }
        smoke.erase(std::remove_if(smoke.begin(), smoke.end(), [](const SmokeParticle &p) { return p.life > p.maxLife; }),
                    smoke.end());

        // Rendering
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        drawBackground(renderer, level.theme);
        drawHorizon(renderer, level, now * 0.001f, levelProgress);
        drawRoad(renderer, level, levelProgress);

        // Obstacles
        for (const auto &obstacle : obstacles) {
            auto proj = project(obstacle.left ? -0.98f : 0.98f, obstacle.z, levelProgress);
            if (!proj.visible) continue;
            drawSprite(renderer, proj.x - static_cast<int>(OBSTACLE_WIDTH * proj.scale / 2),
                       proj.y - static_cast<int>(OBSTACLE_HEIGHT * proj.scale), proj.scale, OBSTACLE_PIXELS,
                       OBSTACLE_WIDTH, OBSTACLE_HEIGHT);
        }

        // Traffic cars
        for (const auto &car : traffic) {
            auto proj = project(car.lane, car.z, levelProgress);
            if (!proj.visible) continue;
            drawSprite(renderer, proj.x - static_cast<int>(TRAFFIC_CAR_WIDTH * proj.scale / 2),
                       proj.y - static_cast<int>(TRAFFIC_CAR_HEIGHT * proj.scale), proj.scale, *car.sprite,
                       TRAFFIC_CAR_WIDTH, TRAFFIC_CAR_HEIGHT);
        }

        // Player car
        drawSprite(renderer, playerScreenX - static_cast<int>(PLAYER_CAR_WIDTH * 3.4f / 2), playerScreenY,
                   3.4f, PLAYER_CAR_PIXELS, PLAYER_CAR_WIDTH, PLAYER_CAR_HEIGHT);

        // Smoke render
        for (const auto &particle : smoke) {
            float alpha = 1.0f - (particle.life / particle.maxLife);
            int shade = static_cast<int>(lerp(120.0f, 220.0f, std::clamp(playerSpeed / kMaxSpeed, 0.0f, 1.0f)));
            SDL_SetRenderDrawColor(renderer, shade, shade, shade, static_cast<Uint8>(alpha * 200));
            int size = 6 + static_cast<int>(std::clamp(playerSpeed / kMaxSpeed, 0.0f, 1.0f) * 8.0f);
            SDL_Rect rect{static_cast<int>(particle.x), static_cast<int>(particle.y), size, size};
            SDL_RenderFillRect(renderer, &rect);
        }

        // HUD
        SDL_Color textColor = level.theme.text;
        drawTinyText(renderer, 20, 20, 3.0f, textColor,
                     "LEVEL " + std::to_string(currentLevel + 1) + " / " + std::to_string(kLevels.size()));
        drawTinyText(renderer, 20, 50, 3.0f, textColor, level.name);
        drawTinyText(renderer, kScreenWidth - 200, 20, 3.0f, textColor, "TIME " + formatTime(levelTimer));
        drawTinyText(renderer, kScreenWidth - 200, 50, 3.0f, textColor,
                     "SPEED " + std::to_string(static_cast<int>(playerSpeed)) + " KM/H");

        switch (state) {
            case GameState::Welcome:
                drawTinyText(renderer, 140, 120, 4.0f, textColor, "WELCOME TO SINGLE TURBO D4");
                drawTinyText(renderer, 120, 180, 3.0f, textColor,
                             "BLAZE YELLOW 306 IS READY TO SOOT EVERYTHING");
                drawTinyText(renderer, 120, 220, 3.0f, textColor,
                             "UP TO ACCEL, DOWN TO BRAKE, LEFT/RIGHT TO STEER");
                drawTinyText(renderer, 120, 260, 3.0f, textColor, "SPACE HONKS, ENTER GREETS THE START CREW");
                break;
            case GameState::StartingLine: {
                drawTinyText(renderer, 250, 120, 4.0f, textColor, "START LINE MAYHEM");
                drawTinyText(renderer, 280, 160, 3.0f, textColor, "FLAG DROPS IN " + formatTime(stateTimer));
                int crowdY = kScreenHeight - 260;
                for (int i = 0; i < 6; ++i) {
                    int offsetX = 120 + i * 60;
                    drawSprite(renderer, offsetX, crowdY, 2.0f, CROWD_PERSON_PIXELS, CROWD_PERSON_WIDTH,
                               CROWD_PERSON_HEIGHT);
                    drawSprite(renderer, kScreenWidth - offsetX - CROWD_PERSON_WIDTH * 2, crowdY, 2.0f,
                               CROWD_PERSON_PIXELS, CROWD_PERSON_WIDTH, CROWD_PERSON_HEIGHT);
                }
                drawSprite(renderer, kScreenWidth / 2 - 80, crowdY + 20, 2.5f, FLAG_PERSON_PIXELS, FLAG_PERSON_WIDTH,
                           FLAG_PERSON_HEIGHT);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_Rect lineRect{static_cast<int>(kScreenWidth / 2 - kRoadNear), kScreenHeight - 190,
                                   static_cast<int>(kRoadNear * 2), 6};
                SDL_RenderFillRect(renderer, &lineRect);
                break;
            }
            case GameState::Playing:
                break;
            case GameState::LevelComplete:
                drawTinyText(renderer, 220, 200, 4.0f, textColor, "STAGE CLEARED");
                drawTinyText(renderer, 260, 240, 3.0f, textColor,
                             (currentLevel + 1 < static_cast<int>(kLevels.size())) ? "PRESS ENTER FOR NEXT LEVEL"
                                                                                      : "PRESS ENTER TO CELEBRATE" );
                break;
            case GameState::LevelFailed:
                drawTinyText(renderer, 220, 200, 4.0f, textColor, "TIME RAN OUT");
                drawTinyText(renderer, 200, 240, 3.0f, textColor, "THE DIESEL DIVA NEEDS ANOTHER GO");
                break;
            case GameState::GameWon:
                drawTinyText(renderer, 200, 200, 4.0f, textColor, "ALL LEVELS MASTERED");
                drawTinyText(renderer, 220, 240, 3.0f, textColor, "OUTRUN ROYALTY ACHIEVED");
                break;
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

