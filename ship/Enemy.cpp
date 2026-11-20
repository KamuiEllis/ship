#include "Enemy.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <algorithm>

Enemy::Enemy(float pX, float pY) {
    posX = pX;
    posY = pY;
    lives = 3;
    alive = true;
}

Enemy::~Enemy()
{
    if (texture)
    {
        SDL_DestroyTexture(texture);
    }
}


bool Enemy::loadTexture(SDL_Renderer* renderer,
    const std::string& path)
{
    SDL_Texture* loadedTexture =
        IMG_LoadTexture(renderer, path.c_str());



    if (!loadedTexture)
    {
        std::cout << "Texture loading failed: "
            << SDL_GetError() << std::endl;

        return false;
    }

    if (texture)
    {
        SDL_DestroyTexture(texture);
    }

    if (projectileTexture)
    {
        SDL_DestroyTexture(projectileTexture);
    }

    texture = loadedTexture;

    // Preserve sharp pixel-art edges
    SDL_SetTextureScaleMode(
        texture,
        SDL_SCALEMODE_NEAREST
    );

    return true;
}

void Enemy::update(float deltaTime)
{
    animationTimer += deltaTime;

    while (animationTimer >= animationSpeed)
    {
        animationTimer -= animationSpeed;

        currentFrame =
            (currentFrame + 1) % TOTAL_FRAMES;
    }
}


void Enemy::render(SDL_Renderer* renderer)
{
    if (!texture || !alive)
        return;

    int column = currentFrame % COLUMNS;
    int row = currentFrame / COLUMNS;

    SDL_FRect source = {
        static_cast<float>(column * FRAME_WIDTH),
        static_cast<float>(row * FRAME_HEIGHT),
        static_cast<float>(FRAME_WIDTH),
        static_cast<float>(FRAME_HEIGHT)
    };

    SDL_FRect destination = {
        posX,
        posY,
        128.0f,
        160.0f
    };

    SDL_RenderTexture(
        renderer,
        texture,
        &source,
        &destination
    );
}

SDL_FRect Enemy::getBounds() const {
    return {
        posX, posY, FRAME_WIDTH - 60, FRAME_HEIGHT - 65
    };
}

void Enemy::move(float x, float y) {
    posX += x;
    posY += y;


    posX = std::clamp(posX, 0.0f, 1200.0f - 128.0f);
    posY = std::clamp(posY, 0.0f, 600.0f - 160.0f);
}





  