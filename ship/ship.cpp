
#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include "Player.h"
#include "Enemy.h"
#include "ParticleSystem.h"

enum class GameState {
    RUNNING,
    PLAYING,
    GAMEOVER
};

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Space Impact",
        1200,
        1000,
        0
    );

    if (!window)
    {
        std::cout << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, nullptr);

    if (!renderer)
    {
        std::cout << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Enemy enemy(500.0f, 300.0f, "basic");
    Player player(500.0f, 600.0f);

    ParticleSystem particleSystem;

    // Load textures
    bool playerLoaded = player.loadTexture(
        renderer,
        "sprites/download.png",
        "sprites/projectile.png",
        "sprites/damaged_ship.png",
        "sprites/explosion.png"
    );

    bool enemyLoaded = enemy.loadTexture(renderer);

    if (!playerLoaded || !enemyLoaded)
    {
        std::cout << "Could not load textures!" << std::endl;

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 1;
    }

    bool running = true;
    SDL_Event event;

    Uint64 lastTime = SDL_GetTicks();

    while (running)
    {
        Uint64 currentTime = SDL_GetTicks();

        float deltaTime =
            (currentTime - lastTime) / 1000.0f;

        lastTime = currentTime;

        // Prevent huge movement jumps after a pause
        deltaTime = std::min(deltaTime, 0.05f);

        // ==============================
        // 1. HANDLE EVENTS
        // ==============================

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }

        // ==============================
        // 2. UPDATE GAME OBJECTS
        // ==============================

        if (player.isAlive())
        {
            player.handleInput(deltaTime);
        }

        player.update(deltaTime);
        player.updateProjectiles(deltaTime);

        enemy.update(deltaTime);

        // ==============================
        // 3. COLLISION DETECTION
        // ==============================

        SDL_FRect playerBounds = player.getBounds();
        SDL_FRect enemyBounds = enemy.getBounds();

        auto& projectiles = player.getProjectiles();

        // Bullet collision with enemy
        std::erase_if(projectiles, [&](const Projectile& bullet)
            {
                SDL_FRect bulletBounds = {
                    bullet.x,
                    bullet.y,
                    bullet.width,
                    bullet.height
                };

                if (enemy.isAlive() &&
                    SDL_HasRectIntersectionFloat(
                        &bulletBounds,
                        &enemyBounds
                    ))
                {
                    bool enemyWasAlive = enemy.isAlive();

                    enemy.takeDamage();

                    // Small bullet impact particles
                    particleSystem.emit(
                        bullet.x + bullet.width / 2.0f,
                        bullet.y + bullet.height / 2.0f,
                        15
                    );

                    // Large explosion when enemy dies
                    if (enemyWasAlive && !enemy.isAlive())
                    {
                        particleSystem.emit(
                            enemyBounds.x + enemyBounds.w / 2.0f,
                            enemyBounds.y + enemyBounds.h / 2.0f,
                            80
                        );
                    }

                    // Remove projectile
                    return true;
                }

                return false;
            });

        // Player collision with enemy
        if (player.isAlive() &&
            enemy.isAlive() &&
            SDL_HasRectIntersectionFloat(
                &playerBounds,
                &enemyBounds
            ))
        {
            if (!player.getVunrability())
            {
                bool playerWasAlive = player.isAlive();
                bool enemyWasAlive = enemy.isAlive();

                player.takeDamage();
                enemy.takeDamage();

                // Collision impact particles
                particleSystem.emit(
                    playerBounds.x + playerBounds.w / 2.0f,
                    playerBounds.y + playerBounds.h / 2.0f,
                    20
                );

                // Player destruction explosion
                if (playerWasAlive && !player.isAlive())
                {
                    particleSystem.emit(
                        playerBounds.x + playerBounds.w / 2.0f,
                        playerBounds.y + playerBounds.h / 2.0f,
                        100
                    );
                }

                // Enemy destruction explosion
                if (enemyWasAlive && !enemy.isAlive())
                {
                    particleSystem.emit(
                        enemyBounds.x + enemyBounds.w / 2.0f,
                        enemyBounds.y + enemyBounds.h / 2.0f,
                        80
                    );
                }
            }
        }

        // ==============================
        // 4. UPDATE PARTICLES
        // ==============================

        particleSystem.update(deltaTime);

        // ==============================
        // 5. RENDER GAME
        // ==============================

        SDL_SetRenderDrawColor(
            renderer,
            10, 10, 26, 255
        );

        SDL_RenderClear(renderer);

        enemy.render(renderer);
        player.render(renderer);

        player.renderProjectiles(renderer);

        // Render particles above ships
        particleSystem.render(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
