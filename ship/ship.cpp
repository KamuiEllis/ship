#include <SDL3/SDL.h>
#include <iostream>

#include "Player.h"
#include "Enemy.h"


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

        
        Enemy enemy(500.0f, 300.0f);
        Player player(500.0f, 600.0f);


        // Load sprite sheet
        if (!player.loadTexture(renderer, "sprites/download.png", "sprites/projectile.png", "sprites/damaged_ship.png", "sprites/explosion.png"), !enemy.loadTexture(renderer, "sprites/enemy_sprite.png"))
        {
            std::cout << "Could not load ship!" << std::endl;
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

            // Handle events
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_EVENT_QUIT)
                {
                    running = false;
                }


                
            }

            player.handleInput(deltaTime);

            // Update player animation
            player.update(deltaTime);
            player.updateProjectiles(deltaTime);
            enemy.update(deltaTime);


            // Background
            SDL_SetRenderDrawColor(
                renderer, 10, 10, 26, 255
            );

            SDL_RenderClear(renderer);

            // Draw spaceship
            enemy.render(renderer);
            player.render(renderer);
            player.renderProjectiles(renderer);

            SDL_FRect playerBounds = player.getBounds();
            SDL_FRect enemyBounds = enemy.getBounds();

            if (SDL_HasRectIntersectionFloat(
                &playerBounds,
                &enemyBounds
            ))
            {
                std::cout << "Player collided with enemy" << std::endl;
                player.takeDamage();
            }

            SDL_RenderPresent(renderer);
        }
    

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}