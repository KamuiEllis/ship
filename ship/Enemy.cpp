#include "Enemy.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <algorithm>
#include <string>

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


bool Enemy::loadTexture(SDL_Renderer* renderer)
{

    SDL_Texture* loadedTexture;
    SDL_Texture* loadedDamagedTexture;
    string enemyTexturePath;
    string enemyDamagedTexturePath;

    if (enemyType == "basic") {
        enemyTexturePath = "sprites/enemy_sprite.png";
        enemyDamagedTexturePath = "sprites/enemy_damaged.png";
        loadedTexture =
            IMG_LoadTexture(renderer, enemyTexturePath.c_str());
        loadedDamagedTexture = IMG_LoadTexture(renderer, enemyDamagedTexturePath.c_str());
            

        
    }
    else {
        loadedTexture = IMG_LoadTexture(renderer, enemyTexturePath.c_str());
        loadedDamagedTexture = IMG_LoadTexture(renderer, enemyDamagedTexturePath.c_str());
    }
    

    if (!loadedTexture || !loadedDamagedTexture)
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

    texture = loadedTexture;
    damageTexture = loadedDamagedTexture;

    // Preserve sharp pixel-art edges
    SDL_SetTextureScaleMode(
        texture,
        SDL_SCALEMODE_NEAREST
    );

    SDL_SetTextureScaleMode(
        damageTexture,
        SDL_SCALEMODE_NEAREST
    );

    return true;
}

void Enemy::update(float deltaTime)
{

    

    if (alive) {

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





  