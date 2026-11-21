
#include "ParticleSystem.h"
#include <random>
#include <algorithm>
#include <cmath>
#include <array>

void ParticleSystem::emit(float x, float y, int count)
{
    static std::mt19937 rng(std::random_device{}());

    constexpr float PI = 3.14159265f;

    std::uniform_real_distribution<float> angleDist(
        0.0f, 2.0f * PI
    );

    std::uniform_real_distribution<float> speedDist(
        50.0f, 180.0f
    );

    std::uniform_real_distribution<float> lifeDist(
        0.25f, 0.65f
    );

    std::uniform_int_distribution<int> sizeDist(1, 3);
    std::uniform_int_distribution<int> colorDist(0, 3);

    // Limited retro color palette
    const std::array<SDL_Color, 4> colors = { {
        { 255, 245, 150, 255 }, // Pale yellow
        { 255, 190, 50, 255 },  // Gold
        { 255, 100, 25, 255 },  // Orange
        { 190, 35, 20, 255 }    // Dark red
    } };

    for (int i = 0; i < count; i++)
    {
        float angle = angleDist(rng);
        float speed = speedDist(rng);
        float life = lifeDist(rng);

        Particle particle{};

        particle.x = x;
        particle.y = y;

        particle.velocityX = std::cos(angle) * speed;
        particle.velocityY = std::sin(angle) * speed;

        particle.lifetime = life;
        particle.maxLifetime = life;

        // Particle sizes: 4, 8, or 12 pixels
        particle.size = static_cast<float>(
            sizeDist(rng) * 4
            );

        particle.color = colors[colorDist(rng)];

        particles.push_back(particle);
    }
}

void ParticleSystem::update(float deltaTime)
{
    for (Particle& particle : particles)
    {
        particle.x += particle.velocityX * deltaTime;
        particle.y += particle.velocityY * deltaTime;

        // Slight drag: particles slow down as they spread
        float drag = std::max(0.0f, 1.0f - 1.5f * deltaTime);

        particle.velocityX *= drag;
        particle.velocityY *= drag;

        particle.lifetime -= deltaTime;
    }

    std::erase_if(particles, [](const Particle& particle)
        {
            return particle.lifetime <= 0.0f;
        });
}

void ParticleSystem::render(SDL_Renderer* renderer)
{
    constexpr float PIXEL_GRID = 4.0f;

    for (const Particle& particle : particles)
    {
        float lifeRatio = std::clamp(
            particle.lifetime / particle.maxLifetime,
            0.0f,
            1.0f
        );

        // Discrete brightness stages instead of alpha fading
        SDL_Color color = particle.color;

        if (lifeRatio < 0.25f)
        {
            color = { 100, 25, 20, 255 };
        }
        else if (lifeRatio < 0.5f)
        {
            color = { 190, 55, 20, 255 };
        }

        SDL_SetRenderDrawColor(
            renderer,
            color.r,
            color.g,
            color.b,
            255
        );

        // Snap rendered positions to a 4-pixel grid
        float pixelX =
            std::floor(particle.x / PIXEL_GRID) * PIXEL_GRID;

        float pixelY =
            std::floor(particle.y / PIXEL_GRID) * PIXEL_GRID;

        // Shrink particles in discrete stages
        float size = particle.size;

        if (lifeRatio < 0.33f)
        {
            size = 4.0f;
        }
        else if (lifeRatio < 0.66f)
        {
            size = std::max(4.0f, size - 4.0f);
        }

        SDL_FRect rect = {
            pixelX,
            pixelY,
            size,
            size
        };

        SDL_RenderFillRect(renderer, &rect);
    }
}
