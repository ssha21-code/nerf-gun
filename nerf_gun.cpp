#include "raylib.h"
#include <iostream>
#include <vector>

constexpr int WINDOW_WIDTH = 1000;
constexpr int WINDOW_HEIGHT = 800;

const float groundY = WINDOW_HEIGHT - 100;
double lastUpdateTime = 0;
bool isShowingVictoryScreen = false;

Texture2D textureHero;
Texture2D textureHeroShooting;
Texture2D textureEnemy;
Texture2D textureEnemyShooting;
Texture2D textureLand;

Sound gunshot;

enum class State {
    PLAYING, 
    VICTORY, 
    DEFEAT,
};

bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
} 

void showVictoryScreen() {
    int victory_text_width = MeasureText("VICTORY!", 72);
    int victory_posX = (GetScreenWidth() - victory_text_width) / 2;
    DrawText("VICTORY!", victory_posX, 300, 72, GREEN);
    int text1_text_width = MeasureText("Thanks for playing!", 40);
    int text1_posX = (GetScreenWidth() - text1_text_width) / 2;
    DrawText("Thanks for playing!", text1_posX, 450, 40, GREEN);
    int text2_text_width = MeasureText("Made by: @ssha21-code", 40);
    int text2_posX = (GetScreenWidth() - text2_text_width) / 2;
    DrawText("Made by: @ssha21-code", text2_posX, 520, 40, GREEN);
}

void showDeathScreen() {
    int death_text_width = MeasureText("YOU DIED!", 72);
    int death_posX = (GetScreenWidth() - death_text_width) / 2;
    DrawText("YOU DIED!", death_posX, 300, 72, RED);
}

void loadImages() {
    Image imageHero = LoadImage("Graphics/hero.png");
    textureHero = LoadTextureFromImage(imageHero);
    Image imageHeroShooting = LoadImage("Graphics/hero_shooting.png");
    textureHeroShooting = LoadTextureFromImage(imageHeroShooting);
    Image imageEnemy = LoadImage("Graphics/enemy.png");
    textureEnemy = LoadTextureFromImage(imageEnemy);
    Image imageEnemyShooting = LoadImage("Graphics/enemy_shooting.png");
    textureEnemyShooting = LoadTextureFromImage(imageEnemyShooting);
    Image imageLand = LoadImage("Graphics/land.png");
    textureLand = LoadTextureFromImage(imageLand);
    gunshot = LoadSound("Sounds/gunshot.mp3");

    UnloadImage(imageHero);
    UnloadImage(imageHeroShooting);
    UnloadImage(imageEnemy);
    UnloadImage(imageEnemyShooting);
    UnloadImage(imageLand);
}

class Player {
public: 
    float width = 153;
    float height = 245;
    Vector2 position = {500, groundY - height};
    Rectangle rectangle = {position.x, position.y, width, height};
    float velocityX = 8.0f;
    float velocityY = 0;
    float gravity = 0.7f;
    float jumpStrength = -18.0f;
    bool isTouchingGround = true;
    bool isPlayerTransformed = false;
    bool shouldSpawnBullet = false;
    bool isAlive = true;
    int hp = 200;

    Player() {

    }

    void draw() {
        if (isPlayerTransformed) {
            DrawTexture(textureHeroShooting, position.x, position.y, WHITE);
        } else {
            DrawTexture(textureHero, position.x, position.y, WHITE);
        }   
    }

    void moveLeft() {
        if (position.x > 0) {
            position.x -= velocityX;
        }
    }

    void moveRight() {
        if (position.x + width + 100 < WINDOW_WIDTH) {
            position.x += velocityX;
        }
    }

    void update() {
        if (isPlayerTransformed) {
            width = 179;
        } else {
            width = 153;
        }
        if (IsKeyPressed(KEY_W) && isTouchingGround) {
            velocityY = jumpStrength;
            isTouchingGround = false;
        }

        if (IsKeyPressed(KEY_E)) {
            isPlayerTransformed = !isPlayerTransformed;
        }
        if (IsKeyPressed(KEY_SPACE) && isPlayerTransformed) {
            shouldSpawnBullet = true;
        }

        velocityY += gravity;
        position.y += velocityY;

        if (position.y + height >= groundY) {
            velocityY = 0;
            position.y = groundY - height;
            isTouchingGround = true;
        }
        if (hp < 0) {
            hp = 0;
        }
        rectangle = {position.x, position.y, width, height};
    }
};

class Bullet {
public: 
    Vector2 position = {};
    Rectangle rectangle = {position.x, position.y, 15, 5};
    bool isPlayerBullet;
    bool shouldBeDestroyed = false;
    float velocityX = 10;

    Bullet(float posX, float posY, bool isPlayerBullet) {
        this->position.x = posX;
        this->position.y = posY;
        this->rectangle = {position.x, position.y, 15, 5};
        this->isPlayerBullet = isPlayerBullet;
    }

    void draw() {
        rectangle = {position.x, position.y, 15, 5};
        DrawRectangle(rectangle.x, rectangle.y, rectangle.width, rectangle.height, GOLD);
    }

    void update() {
        if (isPlayerBullet) {
            position.x += velocityX;
        } else {
            position.x -= velocityX;
        }
    }
};

class Enemy {
public: 
    float width = 153;
    float height = 245;
    Vector2 position = {1500, groundY - height};
    Rectangle rectangle = {position.x, position.y, width, height};
    float velocityX = 6.5f;
    float velocityY = 0;
    float gravity = 0.7f;
    float jumpStrength = -18.0f;
    bool isTouchingGround = true;
    bool isEnemyTransformed = false;
    bool shouldSpawnBullet = false;
    bool shouldMoveTowardsPlayer = true;
    bool shouldMoveLeft = true;
    bool isAlive = true;
    bool isStandingInStartingPosition = false;
    bool isStandingInDesiredPosition = false;
    int hp = 200;

    Enemy() {

    }

    void draw() {
        if (isEnemyTransformed) {
            DrawTexture(textureEnemyShooting, position.x, position.y, WHITE);
        } else {
            DrawTexture(textureEnemy, position.x, position.y, WHITE);
        }   
    }

    void moveLeft() {
        if (position.x > 0) {
            position.x -= velocityX;
        }
    }

    void moveRight() {
        if (position.x + width < WINDOW_WIDTH) {
            position.x += velocityX;
        }
    }
    void update(Player player) {
        if (isEnemyTransformed) {
            width = 179;
        } else {
            width = 153;
        }

        velocityY += gravity;
        position.y += velocityY;

        if (position.y + height >= groundY) {
            velocityY = 0;
            position.y = groundY - height;
            isTouchingGround = true;
        }
        if (hp < 0) {
            hp = 0;
        }
        if (!isStandingInStartingPosition) {
            position.x -= velocityX;
            if (position.x < 800) {
                isStandingInStartingPosition = true;
                isEnemyTransformed = true;
            }
        } 
        if (shouldMoveLeft && isStandingInStartingPosition && !isStandingInDesiredPosition) {
            moveLeft();
        } else if ((!shouldMoveLeft) && isStandingInStartingPosition && !isStandingInDesiredPosition) {
            moveRight();
        }
        float distance = player.position.x - position.x;
        if (distance >= -200 && distance <= -190) {
            isStandingInDesiredPosition = true;
        } else if (distance * -1 >= 200 && distance * -1 <= 210) {
            isStandingInDesiredPosition = true;
        } else {
            isStandingInDesiredPosition = false;
        }
        if (player.position.x > position.x && distance < 200) {
            shouldMoveLeft = false;
        } else if (position.x > player.position.x && distance * -1 > 200) {
            shouldMoveLeft = true;
        }
        rectangle = {position.x, position.y, width, height};
    }
};

class Game {
public: 
    Player player = Player();
    Enemy enemy = Enemy();
    State state = State::PLAYING;
    std::vector<Bullet> bullets = {};

    Game() {

    }
    
    void draw() {
        if (player.isAlive) {
            player.draw();
        }
        if (enemy.isAlive) {
            enemy.draw();
        }
        for (auto &bullet: bullets) {
            bullet.draw();
        }
        DrawText(TextFormat("%i", player.hp), 40, 40, 40, BLACK);
        DrawText(TextFormat("%i", enemy.hp), WINDOW_WIDTH - 80, 40, 40, BLACK);
        if (state == State::VICTORY) {
            if (eventTriggered(2) || isShowingVictoryScreen) {
                showVictoryScreen();
                isShowingVictoryScreen = true;
            }
        }
        if (state == State::DEFEAT) {
            showDeathScreen();            
        }
    }

    void update() {
        if (player.isAlive) {
            if (IsKeyDown(KEY_A)) {
                player.moveLeft();
            }
            if (IsKeyDown(KEY_D)) {
                player.moveRight();
            }
            if (player.shouldSpawnBullet) {
                bullets.push_back(Bullet(player.position.x + player.width, player.position.y + 60, true));
                PlaySound(gunshot);
                player.shouldSpawnBullet = false;
            }
        }
        if (enemy.isAlive) {
            if (enemy.shouldSpawnBullet && enemy.isEnemyTransformed) {
                bullets.push_back(Bullet(enemy.position.x, enemy.position.y + 60, false));
                PlaySound(gunshot);
                enemy.shouldSpawnBullet = false;
            }
            if (eventTriggered(0.35)) {
                enemy.shouldSpawnBullet = true;
            }
        }
        if (enemy.hp == 0) {
            enemy.isAlive = false;
            enemy.position.x = WINDOW_WIDTH;
            state = State::VICTORY;
        }
        if (player.hp == 0) {
            player.isAlive = false;
            player.position.x = 0 - player.width;
            enemy.isEnemyTransformed = false;
            state = State::DEFEAT;
        }
        
        if (player.isAlive) {
            player.update();
        }
        if (enemy.isAlive) {
            enemy.update(player);
        }
        for (auto &bullet: bullets) {
            bullet.update();
        }
        checkForCollisions();
        checkForRemoval();
    }

    void checkForCollisions() {
        for (auto &bullet: bullets) {
            if (enemy.isAlive) {
                if (CheckCollisionRecs(bullet.rectangle, enemy.rectangle)) {
                    if (bullet.isPlayerBullet) {
                        enemy.hp -= GetRandomValue(1, 5);
                        if (enemy.hp < 0) enemy.hp = 0;
                        bullet.shouldBeDestroyed = true;
                    }
                    if (bullet.rectangle.x + bullet.rectangle.width > WINDOW_WIDTH) {
                        bullet.shouldBeDestroyed = true;
                    }
                }
            }
            if (player.isAlive) {
                if (CheckCollisionRecs(bullet.rectangle, player.rectangle)) {
                    if (!bullet.isPlayerBullet) {
                        player.hp -= GetRandomValue(1, 5);
                        if (player.hp < 0) player.hp = 0;
                        bullet.shouldBeDestroyed = true;
                    }
                    if (bullet.rectangle.x + bullet.rectangle.width > WINDOW_WIDTH) {
                        bullet.shouldBeDestroyed = true;
                    }
                }
            }
        }
    }
    void checkForRemoval() {
        for (int i = 0; i < bullets.size(); i++) {
            if (bullets.at(i).shouldBeDestroyed) {
                bullets.erase(bullets.begin() + i);
                i--;
            }
        }
    }
};

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Nerf Gun");
    SetTargetFPS(60);
    InitAudioDevice();

    loadImages();

    Game game = Game();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        DrawTexture(textureLand, 0, 0, WHITE);

        game.update();
        game.draw();

        EndDrawing();
    }

    CloseWindow();
    std::abort();
    return 0;
}