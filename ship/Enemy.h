#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <cmath>
#include <vector>

using namespace std;



class Enemy
{
private:
	float posX;
	float posY;
	int lives;
	bool alive;
	SDL_Texture* texture = nullptr;
	SDL_Texture* projectileTexture = nullptr;
	SDL_Texture* damageTexture = nullptr;
	float flashTime = 0.0f;
	float fireCooldown = 0.2f;
	float fireTimer = 0.0f;
	float flashInterval = 0.2;
	string enemyType = "";
	bool showDamageSprite = false;

	float speed = 500.0f;

	int currentFrame = 0;

	int explosionFrame = 0;
	float explosionTimer = 0.0f;
	float explosionSpeed = 0.08f;

	bool explosionFinished = false;

	static constexpr int FRAME_WIDTH = 190;
	static constexpr int FRAME_HEIGHT = 192;
	static constexpr int COLUMNS = 2;
	static constexpr int TOTAL_FRAMES = 2;

	static constexpr int EXPLOSION_FRAME_WIDTH = 192;
	static constexpr int EXPLOSION_FRAME_HEIGHT = 192;
	static constexpr int EXPLOSION_COLUMNS = 4;
	static constexpr int EXPLOSION_TOTAL_FRAMES = 12;

	float animationTimer = 0.0f;
	float animationSpeed = 0.12f;

public:
	~Enemy();
	Enemy(float pX, float pY, string type);

	bool loadTexture(SDL_Renderer* renderer);

	void move(float x, float y);
	void fire();

	void update(float deltaTime);
	//void updateProjectiles(float deltaTime);

	//void renderProjectiles(SDL_Renderer* renderer);
	void render(SDL_Renderer* renderer);
	SDL_FRect getBounds() const;
	void takeDamage();
};
