#include "Player.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <algorithm>

Player::Player(float pX, float pY) {
	posX = pX;
	posY = pY;
	lives = 3;
	alive = true;
}

Player::~Player()
{
	if (texture)
	{
		SDL_DestroyTexture(texture);
	}
}


bool Player::loadTexture(SDL_Renderer* renderer,
    const std::string& path, const std::string& projectilePath, const std::string& damaged_ship_path)
{
    SDL_Texture* loadedTexture =
        IMG_LoadTexture(renderer, path.c_str());
    
    SDL_Texture* loadedDamagedShipTexture =
        IMG_LoadTexture(renderer, damaged_ship_path.c_str());

    SDL_Texture* loadedProjectileTexture = IMG_LoadTexture(renderer, projectilePath.c_str());

    if (!loadedTexture || !loadedProjectileTexture || !loadedDamagedShipTexture)
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

    if (damageTexture) {
        SDL_DestroyTexture(loadedProjectileTexture);
    }

    texture = loadedTexture;
    projectileTexture = loadedProjectileTexture;
    damageTexture = loadedDamagedShipTexture;

    // Preserve sharp pixel-art edges
    SDL_SetTextureScaleMode(
        texture,
        SDL_SCALEMODE_NEAREST
    );

    return true;
}

void Player::update(float deltaTime)
{
    animationTimer += deltaTime;

    while (animationTimer >= animationSpeed)
    {
        animationTimer -= animationSpeed;

        currentFrame =
            (currentFrame + 1) % TOTAL_FRAMES;
    }
}

void Player::handleInput(float deltaTime)
{
    const bool* keys = SDL_GetKeyboardState(nullptr);

    float directionX = 0.0f;
    float directionY = 0.0f;

    // Horizontal movement
    if (keys[SDL_SCANCODE_LEFT])
        directionX -= 1.0f;

    if (keys[SDL_SCANCODE_RIGHT])
        directionX += 1.0f;

    // Vertical movement
    if (keys[SDL_SCANCODE_UP])
        directionY -= 1.0f;

    if (keys[SDL_SCANCODE_DOWN])
        directionY += 1.0f;

    // Normalize diagonal movement
    float length = std::sqrt(
        directionX * directionX +
        directionY * directionY
    );

    if (length > 0.0f)
    {
        directionX /= length;
        directionY /= length;
    }

    // Apply movement
    move(
        directionX * speed * deltaTime,
        directionY * speed * deltaTime
    );

    if (fireTimer > 0.0f)
    {
        fireTimer -= deltaTime;
    }

    if (invunerableTimer > 0.0f)
    {
        invunerableTimer -= deltaTime;

        flashTime += deltaTime;

        if (flashTime >= flashInterval)
        {
            flashTime -= flashInterval;

            showDamageSprite = !showDamageSprite;
        }

        if (invunerableTimer <= 0.0f)
        {
            invunerableTimer = 0.0f;
            invunrable = false;
            showDamageSprite = false;
        }
    }


    // Handle shooting
    if (keys[SDL_SCANCODE_SPACE] && fireTimer <= 0.0f)
    {
        fire();

        fireTimer = fireCooldown;
    }

}

void Player::render(SDL_Renderer* renderer)
{
    if (!texture || !alive || !damageTexture)
        return;

    int column = currentFrame % COLUMNS;
    int row = currentFrame / COLUMNS;

    SDL_FRect normalSource = {
        static_cast<float>(column * FRAME_WIDTH),
        static_cast<float>(row * FRAME_HEIGHT),
        static_cast<float>(FRAME_WIDTH),
        static_cast<float>(FRAME_HEIGHT)
    };

    SDL_FRect damagedSource = {
        0.0f,
        0.0f,
        192.0f,
        241.0f
    };

    SDL_FRect destination = {
        posX,
        posY,
        128.0f,
        160.0f
    };

    SDL_Texture* selectedTexture =
        showDamageSprite ? damageTexture : texture;

    SDL_FRect* selectedSource =
        showDamageSprite ? &damagedSource : &normalSource;

    SDL_RenderTexture(
        renderer,
        selectedTexture,
        selectedSource,
        &destination
    );
    
}

void Player::move(float x, float y) {
	posX += x;
	posY += y;


    posX = std::clamp(posX, 0.0f, 1200.0f - 128.0f);
    posY = std::clamp(posY, 0.0f, 1000.0f - 160.0f);
}

void Player::fire() {
    Projectile bullet;

    bullet.x = posX + 48;
    bullet.y = posY + 80.0f;

    projectiles.push_back(bullet);


}

void Player::updateProjectiles(float deltaTime)
{
    for (Projectile& bullet : projectiles)
    {
        bullet.y -= bullet.speed * deltaTime;
    }

    // Remove bullets that leave the screen
    std::erase_if(projectiles, [](const Projectile& bullet)
        {
            return bullet.x > 1200.0f;
        });
}

SDL_FRect Player::getBounds() const {
    return {
        posX, posY, FRAME_WIDTH, FRAME_HEIGHT
    };
}

void Player::renderProjectiles(SDL_Renderer* renderer)
{
    //SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);

    for (const Projectile& bullet : projectiles)
    {
        SDL_FRect rect = {
            bullet.x,
            bullet.y,
            bullet.width,
            bullet.height
        };

        SDL_FRect drawFrom = { 0,0, 161, 161 };
        
        SDL_RenderTexture(
            renderer,
            projectileTexture,
            &drawFrom,
            &rect
        );

        //SDL_RenderFillRect(renderer, &rect);
    }
}

void Player::takeDamage() {
    if (invunrable)
        return;

    invunrable = true;
    invunerableTimer = 8.0f;

    flashTime = 0.0f;
    showDamageSprite = true;
    lives -= 1;
}