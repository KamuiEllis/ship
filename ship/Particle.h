#pragma once
#include <SDL3/SDL.h>

struct Particle
{
    float x, y;
    float velocityX, velocityY;

    float lifetime;
    float maxLifetime;

    float size;

    SDL_Color color;
};