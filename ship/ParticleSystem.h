#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include "Particle.h"



class ParticleSystem
{
private:
    std::vector<Particle> particles;

public:
    void emit(float x, float y, int count);
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
};