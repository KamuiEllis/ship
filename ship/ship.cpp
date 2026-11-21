#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <random>
#include <SDL3_ttf/SDL_ttf.h>

#include "Player.h"
#include "Enemy.h"
#include "ParticleSystem.h"
#include "SoundSynth.h"

enum class GameState {
    RUNNING,
    PLAYING,
    GAMEOVER
};

struct Star
{
    float x;
    float y;
    float speed;
    float size;
    SDL_Color color;
};

std::vector<Star> createStarField(
    int count,
    int screenWidth,
    int screenHeight,
    std::mt19937& rng
)
{
    std::vector<Star> stars;
    stars.reserve(count);

    std::uniform_real_distribution<float> xDist(0.0f, static_cast<float>(screenWidth));
    std::uniform_real_distribution<float> yDist(0.0f, static_cast<float>(screenHeight));
    std::uniform_real_distribution<float> speedDist(20.0f, 140.0f);
    std::uniform_int_distribution<int> sizeDist(1, 3);
    std::uniform_int_distribution<int> brightnessDist(150, 255);

    for (int i = 0; i < count; i++)
    {
        Star star{};
        star.x = xDist(rng);
        star.y = yDist(rng);
        star.speed = speedDist(rng);
        star.size = static_cast<float>(sizeDist(rng));

        Uint8 brightness = static_cast<Uint8>(brightnessDist(rng));
        star.color = { brightness, brightness, brightness, 255 };

        stars.push_back(star);
    }

    return stars;
}

void updateStarField(
    std::vector<Star>& stars,
    float deltaTime,
    int screenWidth,
    int screenHeight,
    std::mt19937& rng
)
{
    std::uniform_real_distribution<float> xDist(0.0f, static_cast<float>(screenWidth));
    std::uniform_real_distribution<float> speedDist(20.0f, 140.0f);
    std::uniform_int_distribution<int> sizeDist(1, 3);
    std::uniform_int_distribution<int> brightnessDist(150, 255);

    for (Star& star : stars)
    {
        star.y += star.speed * deltaTime;

        if (star.y > screenHeight)
        {
            star.y = -star.size;
            star.x = xDist(rng);
            star.speed = speedDist(rng);
            star.size = static_cast<float>(sizeDist(rng));

            Uint8 brightness = static_cast<Uint8>(brightnessDist(rng));
            star.color = { brightness, brightness, brightness, 255 };
        }
    }
}

void renderSpaceBackground(
    SDL_Renderer* renderer,
    const std::vector<Star>& stars
)
{
    // Deep-space background color
    SDL_SetRenderDrawColor(renderer, 5, 8, 20, 255);
    SDL_RenderClear(renderer);

    // Optional large faint stars / glow blocks
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    for (const Star& star : stars)
    {
        SDL_SetRenderDrawColor(
            renderer,
            star.color.r,
            star.color.g,
            star.color.b,
            star.color.a
        );

        SDL_FRect rect = {
            star.x,
            star.y,
            star.size,
            star.size
        };

        SDL_RenderFillRect(renderer, &rect);

        // Give slightly larger stars a tiny glow
        if (star.size >= 2.0f)
        {
            SDL_SetRenderDrawColor(
                renderer,
                star.color.r,
                star.color.g,
                star.color.b,
                80
            );

            SDL_FRect glow = {
                star.x - 1.0f,
                star.y - 1.0f,
                star.size + 2.0f,
                star.size + 2.0f
            };

            SDL_RenderFillRect(renderer, &glow);
        }
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}


static int playSound() {
     SDL_AudioSpec spec{};
    spec.format = SDL_AUDIO_F32;
    spec.channels = 1;
    spec.freq = SAMPLE_RATE;

    SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
        &spec,
        nullptr,
        nullptr
    );

    if (!stream)
    {
        std::cout << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Generate a quarter-second beep
    auto beep = generateBeep(440.0f, 0.25f);

    // Send samples to the stream
    if (!SDL_PutAudioStreamData(
        stream,
        beep.data(),
        static_cast<int>(beep.size() * sizeof(float))
    ))
    {
        std::cout << SDL_GetError() << std::endl;
    }

    // Start playback
    if (!SDL_ResumeAudioStreamDevice(stream))
    {
        std::cout << SDL_GetError() << std::endl;
    }

    // Keep the program alive while audio plays
    SDL_Delay(500);

    SDL_DestroyAudioStream(stream);
    SDL_Quit();

    return 0;
}


int main()
{

   

    int score = 0;

    const int SCREEN_WIDTH = 1200;
    const int SCREEN_HEIGHT = 1000;


    if (!SDL_Init(SDL_INIT_AUDIO))
    {
        std::cout << SDL_GetError() << std::endl;
        return 1;
    }

\

    if (!TTF_Init())
    {
        std::cout << "TTF initialization failed: "
            << SDL_GetError() << std::endl;
        return 1;
    }

    TTF_Font* font = TTF_OpenFont(
        "fonts/arcade.otf",
        32.0f
    );

    TTF_Font* healthFont = TTF_OpenFont(
        "fonts/arcade.otf",
        22.0f
    );

    if (!font)
    {
        std::cout << "Font loading failed: "
            << SDL_GetError() << std::endl;
        return 1;
    }

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Space Impact",
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0
    );

    if (!window)
    {
        std::cout << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    if (!renderer)
    {
        std::cout << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    TTF_TextEngine* textEngine = TTF_CreateRendererTextEngine(renderer);

    if (!textEngine) {
        std::cout << SDL_GetError() << std::endl;
        return 1;
    }
    Player player(500.0f, 600.0f);
    std::string text =
        "HEALTH: " + std::to_string(player.getLives());
    TTF_Text* scoreText = TTF_CreateText(textEngine, font, "SCORE 0", 0);
    TTF_Text* healthText = TTF_CreateText(textEngine, healthFont, text.c_str(), 0);

    if (!scoreText || !healthText)
    {
        std::cout << SDL_GetError() << std::endl;
        return 1;
    }

    TTF_SetTextColor(
        scoreText,
        255, 255, 255, 255
    );

    TTF_SetTextColor(
        healthText,
        255, 255, 255, 255
    );

   
    ParticleSystem particleSystem;
    std::vector<std::unique_ptr<Enemy>> enemies;

    std::mt19937 rng(std::random_device{}());

    // Create starfield
    std::vector<Star> stars = createStarField(
        140,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        rng
    );

    bool playerLoaded = player.loadTexture(
        renderer,
        "sprites/download.png",
        "sprites/projectile.png",
        "sprites/damaged_ship.png",
        "sprites/explosion.png"
    );

    if (!playerLoaded)
    {
        std::cout << "Could not load player textures!" << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;

    Uint64 lastTime = SDL_GetTicks();

    float spawnTimer = 0.0f;
    float spawnInterval = 1.0f;

    std::uniform_real_distribution<float> spawnX(
        0.0f,
        static_cast<float>(SCREEN_WIDTH - 128)
    );

    while (running)
    {
        Uint64 currentTime = SDL_GetTicks();

        float deltaTime =
            (currentTime - lastTime) / 1000.0f;

        lastTime = currentTime;

        deltaTime = std::min(deltaTime, 0.05f);

        // ==============================
        // 1. HANDLE EVENTS
        // ==============================
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                TTF_DestroyText(scoreText);
                TTF_DestroyRendererTextEngine(textEngine);
                TTF_CloseFont(font);
                running = false;
            }
        }

        // ==============================
        // 2. SPAWN ENEMIES
        // ==============================
        spawnTimer += deltaTime;

        if (spawnTimer >= spawnInterval)
        {
            spawnTimer -= spawnInterval;

            float randomX = spawnX(rng);

            auto enemy = std::make_unique<Enemy>(
                randomX,
                -160.0f,
                "basic"
            );

            if (enemy->loadTexture(renderer))
            {
                enemies.push_back(std::move(enemy));
            }
        }

        // ==============================
        // 3. UPDATE GAME OBJECTS
        // ==============================
        if (player.isAlive())
        {
            player.handleInput(deltaTime);
        }

        player.update(deltaTime);
        player.updateProjectiles(deltaTime);

        for (auto& enemy : enemies)
        {
            enemy->update(deltaTime);
        }

        updateStarField(
            stars,
            deltaTime,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            rng
        );

        // ==============================
        // 4. COLLISION DETECTION
        // ==============================
        auto& projectiles = player.getProjectiles();

        std::erase_if(projectiles, [&](const Projectile& bullet)
            {
                SDL_FRect bulletBounds = {
                    bullet.x,
                    bullet.y,
                    bullet.width,
                    bullet.height
                };

                for (auto& enemy : enemies)
                {
                    if (!enemy->isAlive())
                        continue;

                    SDL_FRect enemyBounds = enemy->getBounds();

                    if (SDL_HasRectIntersectionFloat(
                        &bulletBounds,
                        &enemyBounds
                    ))
                    {
                        bool enemyWasAlive = enemy->isAlive();

                        enemy->takeDamage();

                     

                        // Bullet hit particles
                        particleSystem.emit(
                            bullet.x + bullet.width / 2.0f,
                            bullet.y + bullet.height / 2.0f,
                            15
                        );

                        // Enemy death particles
                        if (enemyWasAlive && !enemy->isAlive())
                        {
                            score += 100;

                            std::string text =
                                "SCORE: " + std::to_string(score);

                            TTF_SetTextString(
                                scoreText,
                                text.c_str(),
                                0
                            );
                            particleSystem.emit(
                                enemyBounds.x + enemyBounds.w / 2.0f,
                                enemyBounds.y + enemyBounds.h / 2.0f,
                                80
                            );
                        }

                        return true;
                    }
                }

                return false;
            });

        if (player.isAlive())
        {
            SDL_FRect playerBounds = player.getBounds();

            for (auto& enemy : enemies)
            {
                if (!enemy->isAlive())
                    continue;

                SDL_FRect enemyBounds = enemy->getBounds();

                if (SDL_HasRectIntersectionFloat(
                    &playerBounds,
                    &enemyBounds
                ))
                {
                    if (!player.getVunrability())
                    {
                        bool playerWasAlive = player.isAlive();
                        bool enemyWasAlive = enemy->isAlive();

                        player.takeDamage();
                        enemy->takeDamage();

                        std::string text =
                            "HEALTH: " + std::to_string(player.getLives());

                        TTF_SetTextString(
                            healthText,
                            text.c_str(),
                            0
                        );

                        particleSystem.emit(
                            playerBounds.x + playerBounds.w / 2.0f,
                            playerBounds.y + playerBounds.h / 2.0f,
                            20
                        );

                        if (enemyWasAlive && !enemy->isAlive())
                        {
                            particleSystem.emit(
                                enemyBounds.x + enemyBounds.w / 2.0f,
                                enemyBounds.y + enemyBounds.h / 2.0f,
                                80
                            );
                        }

                        if (playerWasAlive && !player.isAlive())
                        {
                            particleSystem.emit(
                                playerBounds.x + playerBounds.w / 2.0f,
                                playerBounds.y + playerBounds.h / 2.0f,
                                100
                            );
                        }
                    }
                }
            }
        }

        // ==============================
        // 5. CLEAN UP ENEMIES
        // ==============================
        std::erase_if(enemies, [&](const std::unique_ptr<Enemy>& enemy)
            {
                SDL_FRect bounds = enemy->getBounds();

                bool offScreen = bounds.y > SCREEN_HEIGHT;

                bool destroyed =
                    !enemy->isAlive() &&
                    enemy->isExplosionFinished();

                return offScreen || destroyed;
            });

        // ==============================
        // 6. UPDATE PARTICLES
        // ==============================
        particleSystem.update(deltaTime);

        // ==============================
        // 7. RENDER
        // ==============================
        renderSpaceBackground(renderer, stars);

        for (auto& enemy : enemies)
        {
            enemy->render(renderer);
        }

        player.render(renderer);
        player.renderProjectiles(renderer);
        particleSystem.render(renderer);
        if (!TTF_DrawRendererText(scoreText, 30.0f, 20.0f))
        {
            std::cout << "Text rendering failed: "
                << SDL_GetError() << std::endl;
        }

        if (!TTF_DrawRendererText(healthText, 30.0f, 50.0f))
        {
            std::cout << "Text rendering failed: "
                << SDL_GetError() << std::endl;
        }

        SDL_RenderPresent(renderer);
    }

    TTF_DestroyText(scoreText);
    TTF_DestroyRendererTextEngine(textEngine);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}