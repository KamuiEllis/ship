#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <cmath>
#include <vector>
#include "Particle.h"

struct Projectile {
	float x;
	float y;

	float width = 30.0f;
	float height = 30.0f;

	float speed = 600.0f;

};

class Player
{
private:
	float posX;
	float posY;
	int lives;
	bool alive;
	SDL_Texture* texture = nullptr;
	SDL_Texture* damageTexture = nullptr;
	SDL_Texture* projectileTexture = nullptr;
	SDL_Texture* explosionTexture = nullptr;
	std::vector<Projectile> projectiles;
	float fireCooldown = 0.2f;
	float invunerableTimer = 0.0f;
	float flashTime = 0.0f;
	bool showDamageSprite = false;
	float flashInterval = 0.2;
	float fireTimer = 0.0f;
	bool invunrable = false;


	float speed = 500.0f;

	int currentFrame = 0;

	int explosionFrame = 0;
	float explosionTimer = 0.0f;
	float explosionSpeed = 0.08f;

	bool explosionFinished = false;

	static constexpr int FRAME_WIDTH = 256;
	static constexpr int FRAME_HEIGHT = 320;
	static constexpr int COLUMNS = 4;
	static constexpr int TOTAL_FRAMES = 4;

	static constexpr int EXPLOSION_FRAME_WIDTH = 192;
	static constexpr int EXPLOSION_FRAME_HEIGHT = 192;
	static constexpr int EXPLOSION_COLUMNS = 4;
	static constexpr int EXPLOSION_TOTAL_FRAMES = 12;

	float animationTimer = 0.0f;
	float animationSpeed = 0.12f;

public:
	~Player();
	Player(float pX, float pY);
	
	bool loadTexture(SDL_Renderer* renderer,
			const std::string& path, const std::string& projectilePath, const std::string& damaged_ship_path, const std::string& explosion_path);

	void handleInput(float deltaTime);

	void move(float x, float y);
	void fire();
	bool isAlive();
	bool getVunrability();
	void update(float deltaTime);
	void updateProjectiles(float deltaTime);
	std::vector<Projectile>& getProjectiles();
	void renderProjectiles(SDL_Renderer* renderer);
	void render(SDL_Renderer* renderer);
	void takeDamage();
	SDL_FRect getBounds() const;
};
