#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <cmath>
#include <vector>




class Enemy
{
private:
	float posX;
	float posY;
	int lives;
	bool alive;
	SDL_Texture* texture = nullptr;
	SDL_Texture* projectileTexture = nullptr;
	float fireCooldown = 0.2f;
	float fireTimer = 0.0f;

	float speed = 500.0f;

	int currentFrame = 0;

	static constexpr int FRAME_WIDTH = 190;
	static constexpr int FRAME_HEIGHT = 192;
	static constexpr int COLUMNS = 2;
	static constexpr int TOTAL_FRAMES = 2;

	float animationTimer = 0.0f;
	float animationSpeed = 0.12f;

public:
	~Enemy();
	Enemy(float pX, float pY);

	bool loadTexture(SDL_Renderer* renderer,
		const std::string& path);

	void move(float x, float y);
	void fire();

	void update(float deltaTime);
	//void updateProjectiles(float deltaTime);

	//void renderProjectiles(SDL_Renderer* renderer);
	void render(SDL_Renderer* renderer);
	SDL_FRect getBounds() const;
};
