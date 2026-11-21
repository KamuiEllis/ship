#include "Enemy.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <memory>
#include <vector>
#include <random>
std::vector<std::unique_ptr<Enemy>> enemies;


using namespace std;

Enemy::Enemy(float pX, float pY, string type) {
    posX = pX;
    posY = pY;
    lives = 3;
    alive = true;
    enemyType = type;
}

Enemy::~Enemy()
{
    if (texture)
    {
        SDL_DestroyTexture(texture);
    }
}

bool Enemy::isAlive() {
    return alive;
}


bool Enemy::loadTexture(SDL_Renderer* renderer)
{

    SDL_Texture* loadedTexture;
    SDL_Texture* loadedDamagedTexture;

    SDL_Texture* loadedExplosionTexture;
    string enemyTexturePath;
    string enemyDamagedTexturePath;
    string explosion_path;

    if (enemyType == "basic") {
        enemyTexturePath = "sprites/enemy_sprite.png";
        enemyDamagedTexturePath = "sprites/enemy_damaged.png";
        explosion_path = "sprites/explosion.png";
        loadedTexture =
            IMG_LoadTexture(renderer, enemyTexturePath.c_str());
        loadedDamagedTexture = IMG_LoadTexture(renderer, enemyDamagedTexturePath.c_str());
        
        loadedExplosionTexture =
            IMG_LoadTexture(renderer, explosion_path.c_str());

        
    }
    else {
        loadedTexture = IMG_LoadTexture(renderer, enemyTexturePath.c_str());
        loadedDamagedTexture = IMG_LoadTexture(renderer, enemyDamagedTexturePath.c_str());
        loadedExplosionTexture =
            IMG_LoadTexture(renderer, explosion_path.c_str());
    }
    

    if (!loadedTexture || !loadedDamagedTexture|| !loadedExplosionTexture)
    {
        std::cout << "Texture loading failed: "
            << SDL_GetError() << std::endl;

        return false;
    }

    if (texture)
    {
        SDL_DestroyTexture(texture);
    }

    if (damageTexture) {
        SDL_DestroyTexture(damageTexture);
    }

    if (explosionTexture) {
        SDL_DestroyTexture(explosionTexture);
    }

    texture = loadedTexture;
    damageTexture = loadedDamagedTexture;
    explosionTexture = loadedExplosionTexture;

    // Preserve sharp pixel-art edges
    SDL_SetTextureScaleMode(
        texture,
        SDL_SCALEMODE_NEAREST
    );

    SDL_SetTextureScaleMode(
        damageTexture,
        SDL_SCALEMODE_NEAREST
    );

    SDL_SetTextureScaleMode(
        explosionTexture,
        SDL_SCALEMODE_NEAREST
    );

    return true;
}

void Enemy::update(float deltaTime)
{

    

    if (alive) {

        posY += 220.0f * deltaTime;

        if (showDamageSprite == true) {
            flashTime += deltaTime;

            if (flashTime >= flashInterval)
            {
                flashTime -= flashInterval;

                showDamageSprite = !showDamageSprite;
            }
        }
       

        animationTimer += deltaTime;

        while (animationTimer >= animationSpeed)
        {
            animationTimer -= animationSpeed;

            currentFrame =
                (currentFrame + 1) % TOTAL_FRAMES;
        }
    }
    else if (!explosionFinished) {
        explosionTimer += deltaTime;

        while (explosionTimer >= explosionSpeed)
        {
            explosionTimer -= explosionSpeed;
            explosionFrame++;

            if (explosionFrame >= EXPLOSION_TOTAL_FRAMES)
            {
                explosionFrame = EXPLOSION_TOTAL_FRAMES - 1;
                explosionFinished = true;
                break;
            }
        }
    }
   
}

bool Enemy::isExplosionFinished() const
{
    return explosionFinished;
}


void Enemy::render(SDL_Renderer* renderer)
{
    if (!texture)
        return;

    int column = currentFrame % COLUMNS;
    int row = currentFrame / COLUMNS;

    int explosionColumn = currentFrame % EXPLOSION_COLUMNS;
    int explosionRow = currentFrame / EXPLOSION_COLUMNS;

    if (alive) {
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

        SDL_FRect damagedSource = {
           0.0f,
           0.0f,
           190.0f,
           192.0f
        };

        SDL_Texture* selectedTexture =
            showDamageSprite ? damageTexture : texture;

        SDL_FRect* selectedSource =
            showDamageSprite ? &damagedSource : &source;

        SDL_RenderTexture(
            renderer,
            selectedTexture,
            selectedSource,
            &destination
        );
    }
    else {
        SDL_FRect normalSource = {
        static_cast<float>(explosionColumn * EXPLOSION_FRAME_WIDTH),
        static_cast<float>(explosionRow * EXPLOSION_FRAME_HEIGHT),
        static_cast<float>(EXPLOSION_FRAME_WIDTH),
        static_cast<float>(EXPLOSION_FRAME_HEIGHT)
        };

        SDL_FRect destination = {
            posX,
            posY,
            128.0f,
            160.0f
        };

        SDL_Texture* selectedTexture = explosionTexture;
        SDL_FRect* selectedSource = &normalSource;

        SDL_RenderTexture(
            renderer,
            selectedTexture,
            selectedSource,
            &destination
        );

        if (!explosionFinished)
        {
            int explosionColumn = explosionFrame % EXPLOSION_COLUMNS;
            int explosionRow = explosionFrame / EXPLOSION_COLUMNS;

            SDL_FRect source = {
                static_cast<float>(explosionColumn * EXPLOSION_FRAME_WIDTH),
                static_cast<float>(explosionRow * EXPLOSION_FRAME_HEIGHT),
                static_cast<float>(EXPLOSION_FRAME_WIDTH),
                static_cast<float>(EXPLOSION_FRAME_HEIGHT)
            };

            SDL_FRect destination = {
                posX,
                posY,
                128.0f,
                160.0f
            };

            SDL_RenderTexture(renderer, explosionTexture, &source, &destination);
        }
        else {
            SDL_DestroyTexture(explosionTexture);
        }
    }

    
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


void Enemy::takeDamage()
{
    if (!alive)
        return;

    lives--;

    if (lives <= 0)
    {
        lives = 0;
        alive = false;

        explosionFrame = 0;
        explosionTimer = 0.0f;
        explosionFinished = false;

        return;
    }

    flashTime = 0.0f;
    showDamageSprite = true;
}





  