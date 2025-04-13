#include "raylib.h"
#include "raymath.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <vector>

Camera2D camera = {0};
float zoom = 2.0f; // Increased default zoom
float minY = 200.0f; // Adjust to a height where the background ends
float maxY = 600.0f; // Add a max Y boundary if needed
float minX = 100.0f; // Add min X boundary
float maxX = 1800.0f; // Add max X boundary.
// Bullet structure
struct Bullet {
    Vector2 position;
    Vector2 velocity;
    bool active;
    bool flipped;
    float rotation;
    bool collided;
    int collisionFrame;
    float collisionTimer;
};
// Player settings
float playerX = 320.0f;
float playerY = 240.0f;
float playerSpeed = 100.0f;

float redDotOffsetX = 10.0f;
float redDotOffsetY = 0.0f; // Initialize with a default value


// Background texture
Texture2D backgroundTexture;
Texture2D backgroundBorder;

// Player animation
Texture2D playerTexture;
Rectangle playerIdleFrames[4];
Rectangle playerWalkFrames[7];
int currentFrame = 0;
float animationTimer = 0.0f;
float animationSpeed = 0.1f;
bool isMoving = false;
Vector2 movement = {0.0f, 0.0f};
bool playerFlipped = false;
bool gameOver = false;

// Collision box settings
Rectangle collisionBox = {playerX + 15, playerY, 17, 20};

// Borders
Rectangle leftBorderCollision = {0, 0, 20, 480};
Rectangle topBorderCollision = {0, 0, 640, 17};
Rectangle bottomBorderCollision = {0, 463, 640, 17};
Rectangle rightBorderCollision = {623, 0, 25, 480};

// Gun settings
Texture2D gunTexture;
Rectangle gunFrames[4];
int currentGunFrame = 0;
float gunAnimationTimer = 0.0f;
float gunAnimationSpeed = 0.1f;
bool isShooting = false;
float gunRotation = 0.0f;

// Bullet settings
Texture2D bulletTexture;
Bullet bullets[100];

// Object variables
struct GameObject {
    Texture2D texture;
    Rectangle sourceRect;
    Rectangle collisionBox;
    Vector2 position;
};

Music backgroundMusicGlobal;
Sound shotSoundGlobal;
Sound gameOverSound;
Sound winSound;


GameObject objects[30];
Texture2D objectTexture;

// Object tile positions
struct ObjectPlacement {
    int tile;
    Vector2 pixel;
};

// Zombie variables
struct Zombie {
    Texture2D texture;
    Rectangle walkFrames[7];
    Rectangle spawnFrames[5];
    int currentFrame = 0;
    float frameTimer = 0.0f;
    float frameSpeed = 0.15f;
    Vector2 position;
    bool hit = false;
    float hitTimer = 0.0f;
    float collisionWidth = 10;
    float collisionHeight = 10;
    Rectangle collisionBox;
    bool flipped = false;
    Vector2 target;
    float moveTimer = 0.0f;
    float moveInterval = 2.0f;
    float detectionRadius = 200.0f;
    int health = 6;
    bool dead = false;
    float deathTimer = 0.0f;
    bool spawning = true;
    float spawnTimer = 0.0f;
    float spawnAnimationSpeed = 0.2f;
    float moveSpeed;
    float deathBlinkTimer = 0.0f;
    bool deathBlink = false;
};

std::vector<Zombie> zombies;

// Ghost variables
struct Ghost {
    Texture2D texture;
    Rectangle walkFrames[7];
    int currentFrame = 0;
    float frameTimer = 0.0f;
    float frameSpeed = 0.15f;
    Vector2 position;
    bool hit = false;
    float hitTimer = 0.0f;
    float collisionWidth = 10;
    float collisionHeight = 10;
    Rectangle collisionBox;
    bool flipped = false;
    Vector2 target;
    float moveTimer = 0.0f;
    float moveInterval = 2.0f;
    float detectionRadius = 200.0f;
    int health = 6;
    bool dead = false;
    float deathTimer = 0.0f;
    float deathBlinkTimer = 0.0f;
    bool deathBlink = false;
    float moveSpeed;
};

std::vector<Ghost> ghosts;

// Zombie2 variables
struct Zombie2 {
    Texture2D texture;
    Rectangle walkFrames[7];
    Rectangle spawnFrames[5];
    int currentFrame = 0;
    float frameTimer = 0.0f;
    float frameSpeed = 0.15f;
    Vector2 position;
    bool hit = false;
    float hitTimer = 0.0f;
    float collisionWidth = 10;
    float collisionHeight = 10;
    Rectangle collisionBox;
    bool flipped = false;
    Vector2 target;
    float moveTimer = 0.0f;
    float moveInterval = 2.0f;
    float detectionRadius = 200.0f;
    int health = 6;
    bool dead = false;
    float deathTimer = 0.0f;
    bool spawning = true;
    float spawnTimer = 0.0f;
    float spawnAnimationSpeed = 0.2f;
    float moveSpeed;
    float deathBlinkTimer = 0.0f;
    bool deathBlink = false;
};

std::vector<Zombie2> zombies2;

// Player health
int playerHealth = 10;
float playerDamageCooldown = 0.0f;
float playerDamageCooldownDuration = 1.0f;

// Player variables
bool playerHit = false;
float playerHitTimer = 0.0f;
float playerHitDuration = 0.2f;

// Spawn timers and counters
float spawnTimer = 0.0f;
float spawnInterval = 2.0f;
int maxEnemies = 20;

// Monster kill counter and win condition
int monstersKilled = 0;
int monstersToKill = 20;
bool playerWon = false;


// Function to spawn a new zombie
void SpawnZombie() {
    Zombie newZombie;
    newZombie.texture = LoadTexture("/Users/sergiobenavides/Desktop/final_game/assets/images/zombie.png");
    for (int i = 0; i < 7; i++) {
        newZombie.walkFrames[i] = (Rectangle){(float)(i * 23), 0, 23, 23};
    }
    for (int i = 0; i < 5; i++) {
        newZombie.spawnFrames[i] = (Rectangle){(float)((i + 7) * 23), 0, 23, 23};
    }
    newZombie.position = {(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())};
    newZombie.collisionBox = {newZombie.position.x + 5, newZombie.position.y + 10, 10, 10};
    newZombie.target = newZombie.position;
    newZombie.moveSpeed = 50.0f;
    newZombie.frameSpeed = 0.15f;
    zombies.push_back(newZombie);
}

// Function to spawn a new ghost
void SpawnGhost() {
    Ghost newGhost;
    newGhost.texture = LoadTexture("/Users/sergiobenavides/Desktop/final_game/assets/images/ghost.png");
    for (int i = 0; i < 7; i++) {
        newGhost.walkFrames[i] = (Rectangle){(float)(i * 23), 0, 23, 23};
    }
    newGhost.position = {(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())};
    newGhost.collisionBox = {newGhost.position.x + 5, newGhost.position.y + 10, 10, 10};
    newGhost.target = newGhost.position;
    newGhost.moveSpeed = 80.0f;
    newGhost.frameSpeed = 0.15f;
    ghosts.push_back(newGhost);
}

// Function to spawn a new zombie2
void SpawnZombie2() {
    Zombie2 newZombie2;
    newZombie2.texture = LoadTexture("/Users/sergiobenavides/Desktop/final_game/assets/images/zombie2.png"); // Replace with the correct path
    for (int i = 0; i < 7; i++) {
        newZombie2.walkFrames[i] = (Rectangle){(float)(i * 23), 0, 23, 23};
    }
    for (int i = 0; i < 5; i++) {
        newZombie2.spawnFrames[i] = (Rectangle){(float)((i + 7) * 23), 0, 23, 23};
    }
    newZombie2.position = {(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())};
    newZombie2.collisionBox = {newZombie2.position.x + 5, newZombie2.position.y + 10, 10, 10};
    newZombie2.target = newZombie2.position;
    newZombie2.moveSpeed = 60.0f; // Adjust as needed
    newZombie2.frameSpeed = 0.15f;
    zombies2.push_back(newZombie2);
}

// Function to update the game
void UpdateGame() {
    if (!gameOver && !playerWon) {
        Vector2 movement = {0.0f, 0.0f};

        if (IsKeyDown(KEY_W)) movement.y = -1;
        if (IsKeyDown(KEY_S)) movement.y = 1;
        if (IsKeyDown(KEY_A)) movement.x = -1;
        if (IsKeyDown(KEY_D)) movement.x = 1;

        Vector2 newPlayerPosition = {playerX + movement.x * playerSpeed * GetFrameTime(), playerY + movement.y * playerSpeed * GetFrameTime()};
        Rectangle newCollisionBox = {newPlayerPosition.x + 15, newPlayerPosition.y, 17, 20};

        Vector2 tempPlayerPositionX = {playerX + movement.x * playerSpeed * GetFrameTime(), playerY};
        Rectangle tempCollisionBoxX = {tempPlayerPositionX.x + 15, tempPlayerPositionX.y, 17, 20};

        Vector2 tempPlayerPositionY = {playerX, playerY + movement.y * playerSpeed * GetFrameTime()};
        Rectangle tempCollisionBoxY;
        tempCollisionBoxY.x = tempPlayerPositionY.x + 15;
        tempCollisionBoxY.y = tempPlayerPositionY.y;
        tempCollisionBoxY.width = 17;
        tempCollisionBoxY.height = 20;

        bool collisionX = false;
        bool collisionY = false;

        for (int i = 0; i < sizeof(objects) / sizeof(objects[0]); i++) {
            if (CheckCollisionRecs(tempCollisionBoxX, objects[i].collisionBox)) {
                collisionX = true;
                break;
            }
        }

        for (int i = 0; i < sizeof(objects) / sizeof(objects[0]); i++) {
            if (CheckCollisionRecs(tempCollisionBoxY, objects[i].collisionBox)) {
                collisionY = true;
                break;
            }
        }

        if (!collisionX) {
            playerX = tempPlayerPositionX.x;
            collisionBox.x = tempCollisionBoxX.x;
        }
        if (!collisionY) {
            playerY = tempPlayerPositionY.y;
            collisionBox.y = tempCollisionBoxY.y;
        }

        if (collisionBox.x < leftBorderCollision.width) collisionBox.x = leftBorderCollision.width;
        if (collisionBox.x > rightBorderCollision.x - collisionBox.width) collisionBox.x = rightBorderCollision.x - collisionBox.width;
        if (collisionBox.y < topBorderCollision.height) collisionBox.y = topBorderCollision.height;
        if (collisionBox.y > GetScreenHeight() - collisionBox.height - bottomBorderCollision.height) collisionBox.y = GetScreenHeight() - collisionBox.height - bottomBorderCollision.height;

        playerX = collisionBox.x - 15;
        playerY = collisionBox.y;

        isMoving = (movement.x != 0.0f || movement.y != 0.0f);

        animationTimer += GetFrameTime();
        if (animationTimer >= animationSpeed) {
            animationTimer = 0.0f;
            if (isMoving) {
                currentFrame = (currentFrame + 1) % 7;
            } else {
                currentFrame = (currentFrame + 1) % 4;
            }
        }

        Vector2 mousePos = GetMousePosition();
        Vector2 playerCenter = {playerX + 24, playerY + 15};

        Vector2 worldMousePos = {
            (mousePos.x - camera.offset.x) / camera.zoom + camera.target.x,
            (mousePos.y - camera.offset.y) / camera.zoom + camera.target.y
        };

        float deltaX = worldMousePos.x - playerCenter.x;
        float deltaY = worldMousePos.y - playerCenter.y;
        gunRotation = atan2(deltaY, deltaX) * RAD2DEG;

        if (worldMousePos.x < playerX + 24) {
            playerFlipped = true;
        } else {
            playerFlipped = false;
        }

        float redDotOffsetX = 10.0f;
        float redDotOffsetY = playerFlipped ? 0.5f : -3.5f;
        float gunCenterX = playerX + 24 + redDotOffsetX * cos(gunRotation * DEG2RAD) - redDotOffsetY * sin(gunRotation * DEG2RAD);
        float gunCenterY = playerY + 15 + redDotOffsetX * sin(gunRotation * DEG2RAD) + redDotOffsetY * cos(gunRotation * DEG2RAD);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            isShooting = true;
            gunAnimationTimer = 0.0f;
            currentGunFrame = 1;

            for (int i = 0; i < 100; i++) {
                if (!bullets[i].active) {
                    bullets[i].position = {gunCenterX, gunCenterY};
                    bullets[i].velocity = {cos(gunRotation * DEG2RAD) * 250, sin(gunRotation * DEG2RAD) * 250};
                    bullets[i].active = true;
                    bullets[i].flipped = playerFlipped;
                    bullets[i].rotation = gunRotation;
                    bullets[i].collided = false;
                    bullets[i].collisionFrame = 0;
                    bullets[i].collisionTimer = 0.0f;
                    PlaySound(shotSoundGlobal);
                    break;
                }
            }
        }

        if (isShooting) {
            gunAnimationTimer += GetFrameTime();
            if (gunAnimationTimer >= gunAnimationSpeed) {
                gunAnimationTimer = 0.0f;
                currentGunFrame++;
                if (currentGunFrame > 3) {
                    isShooting = false;
                    currentGunFrame = 0;
                }
            }
        }

        for (int i = 0; i < 100; i++) {
            if (bullets[i].active) {
                if (!bullets[i].collided) {
                    bullets[i].position.x += bullets[i].velocity.x * GetFrameTime();
                    bullets[i].position.y += bullets[i].velocity.y * GetFrameTime();

                    Rectangle bulletRect = {bullets[i].position.x, bullets[i].position.y, 32 * 0.5f, 32 * 0.5f};
                    bulletRect.width -= 14;
                    bulletRect.height -= 15;

                    bool bulletCollided = false;
                    for (int j = 0; j < sizeof(objects) / sizeof(objects[0]); j++) {
                        if (CheckCollisionRecs(bulletRect, objects[j].collisionBox)) {
                            bulletCollided = true;
                            break;
                        }
                    }

                    for (size_t z = 0; z < zombies.size(); z++) {
                        zombies[z].collisionBox.x = zombies[z].position.x + 5;
                        zombies[z].collisionBox.y = zombies[z].position.y + 10;

                        if (CheckCollisionRecs(bulletRect, zombies[z].collisionBox) && !zombies[z].dead) {
                            bullets[i].collided = true;
                            bullets[i].collisionFrame = 0;
                            bullets[i].collisionTimer = 0.0f;
                            bullets[i].velocity = {0, 0};
                            zombies[z].hit = true;
                            zombies[z].hitTimer = 0.0f;
                            zombies[z].health--;

                            if (zombies[z].health <= 0) {
                                zombies[z].dead = true;
                                zombies[z].deathTimer = 0.0f;
                            }
                        }
                    }

                    for (size_t g = 0; g < ghosts.size(); g++) {
                        if (CheckCollisionRecs(bulletRect, ghosts[g].collisionBox) && !ghosts[g].dead) {
                            bullets[i].collided = true;
                            bullets[i].collisionFrame = 0;
                            bullets[i].collisionTimer = 0.0f;
                            bullets[i].velocity = {0, 0};
                            ghosts[g].hit = true;
                            ghosts[g].hitTimer = 0.0f;
                            ghosts[g].health--;

                            if (ghosts[g].health <= 0) {
                                ghosts[g].dead = true;
                                ghosts[g].deathTimer = 0.0f;
                            }
                        }
                    }

                    for (size_t z2 = 0; z2 < zombies2.size(); z2++) {
                        zombies2[z2].collisionBox.x = zombies2[z2].position.x + 5;
                        zombies2[z2].collisionBox.y = zombies2[z2].position.y + 10;
                        if (CheckCollisionRecs(bulletRect, zombies2[z2].collisionBox) && !zombies2[z2].dead) {
                            bullets[i].collided = true;
                            bullets[i].collisionFrame = 0;
                            bullets[i].collisionTimer = 0.0f;
                            bullets[i].velocity = {0, 0};
                            zombies2[z2].hit = true;
                            zombies2[z2].hitTimer = 0.0f;
                            zombies2[z2].health--;

                            if (zombies2[z2].health <= 0) {
                                zombies2[z2].dead = true;
                                zombies2[z2].deathTimer = 0.0f;
                            }
                        }
                    }

                    if (bulletCollided ||
                        CheckCollisionRecs(bulletRect, leftBorderCollision) ||
                        CheckCollisionRecs(bulletRect, topBorderCollision) ||
                        CheckCollisionRecs(bulletRect, bottomBorderCollision) ||
                        CheckCollisionRecs(bulletRect, rightBorderCollision)) {
                        bullets[i].collided = true;
                        bullets[i].collisionFrame = 0;
                        bullets[i].collisionTimer = 0.0f;
                        bullets[i].velocity = {0, 0};
                    }

                    if (bullets[i].position.x < 0 || bullets[i].position.x > GetScreenWidth() ||
                        bullets[i].position.y < 0 || bullets[i].position.y > GetScreenHeight()) {
                        bullets[i].active = false;
                    }
                } else {
                    bullets[i].collisionTimer += GetFrameTime();
                    if (bullets[i].collisionTimer >= 0.1f) {
                        bullets[i].collisionTimer = 0.0f;
                        bullets[i].collisionFrame++;
                        if (bullets[i].collisionFrame > 3) {
                            bullets[i].active = false;
                        }
                    }
                }
            }
        }

        for (size_t z = 0; z < zombies.size(); z++) {
            if (zombies[z].dead && zombies[z].deathTimer >= 0.5f) {
                monstersKilled++;
            }
        }
        for (size_t g = 0; g < ghosts.size(); g++) {
            if (ghosts[g].dead && ghosts[g].deathTimer >= 0.5f) {
                monstersKilled++;
            }
        }
        for (size_t z2 = 0; z2 < zombies2.size(); z2++) {
            if (zombies2[z2].dead && zombies2[z2].deathTimer >= 0.5f) {
                monstersKilled++;
            }
        }

        zombies.erase(std::remove_if(zombies.begin(), zombies.end(), [](const Zombie& z) { return z.dead && z.deathTimer >= 0.5f; }), zombies.end());
        ghosts.erase(std::remove_if(ghosts.begin(), ghosts.end(), [](const Ghost& g) { return g.dead && g.deathTimer >= 0.5f; }), ghosts.end());
        zombies2.erase(std::remove_if(zombies2.begin(), zombies2.end(), [](const Zombie2& z2) { return z2.dead && z2.deathTimer >= 0.5f; }), zombies2.end());

        if (monstersKilled >= monstersToKill) {
            playerWon = true;
            PlaySound(winSound);
        }

        spawnTimer += GetFrameTime();
        if (spawnTimer >= spawnInterval) {
            spawnTimer = 0.0f;
            if (zombies.size() + ghosts.size() + zombies2.size() < maxEnemies) {
                int spawnType = GetRandomValue(0, 2);
                if (spawnType == 0) {
                    SpawnZombie();
                } else if (spawnType == 1) {
                    SpawnGhost();
                } else {
                    SpawnZombie2();
                }
            }
        }

        for (size_t z = 0; z < zombies.size(); z++) {
            if (!zombies[z].dead) {
                if (zombies[z].spawning) {
                    zombies[z].spawnTimer += GetFrameTime();
                    if (zombies[z].spawnTimer >= zombies[z].spawnAnimationSpeed) {
                        zombies[z].spawnTimer = 0.0f;
                        zombies[z].currentFrame++;
                        if (zombies[z].currentFrame >= 5) {
                            zombies[z].spawning = false;
                            zombies[z].currentFrame = 0;
                        }
                    }
                } else {
                    float zombieDeltaX = playerX - zombies[z].position.x;
                    float zombieDeltaY = playerY - zombies[z].position.y;
                    float zombieDistance = sqrt(pow(zombieDeltaX, 2) + pow(zombieDeltaY, 2));

                    if (zombieDistance < zombies[z].detectionRadius) {
                        float zombieAngle = atan2(zombieDeltaY, zombieDeltaX) * RAD2DEG;
                        if (abs(zombieAngle) > 90 && abs(zombieAngle) < 270) {
                            zombies[z].flipped = true;
                        } else {
                            zombies[z].flipped = false;
                        }
                        Vector2 zombieNewPosition = {
                            zombies[z].position.x + static_cast<float>(cos(zombieAngle * DEG2RAD) * zombies[z].moveSpeed * GetFrameTime()),
                            zombies[z].position.y + static_cast<float>(sin(zombieAngle * DEG2RAD) * zombies[z].moveSpeed * GetFrameTime())
                        };
                        Rectangle zombieNewCollisionBox = {zombieNewPosition.x + 5, zombieNewPosition.y + 10, zombies[z].collisionWidth, zombies[z].collisionHeight};

                        bool zombieCollision = false;
                        for (int i = 0; i < sizeof(objects) / sizeof(objects[0]); i++) {
                            if (CheckCollisionRecs(zombieNewCollisionBox, objects[i].collisionBox)) {
                                zombieCollision = true;
                                break;
                            }
                        }

                        if (!zombieCollision) {
                            zombies[z].position = zombieNewPosition;
                        }
                    } else {
                        zombies[z].moveTimer += GetFrameTime();
                        if (zombies[z].moveTimer >= zombies[z].moveInterval) {
                            zombies[z].moveTimer = 0.0f;
                            zombies[z].target = {
                                (float)GetRandomValue(0, GetScreenWidth()),
                                (float)GetRandomValue(0, GetScreenHeight())
                            };
                        }
                        float targetDeltaX = zombies[z].target.x - zombies[z].position.x;
                        float targetDeltaY = zombies[z].target.y - zombies[z].position.y;
                        float targetAngle = atan2(targetDeltaY, targetDeltaX) * RAD2DEG;
                        if (abs(targetAngle) > 90 && abs(targetAngle) < 270) {
                            zombies[z].flipped = true;
                        } else {
                            zombies[z].flipped = false;
                        }
                        Vector2 zombieNewPosition = {
                            zombies[z].position.x + static_cast<float>(cos(targetAngle * DEG2RAD) * (zombies[z].moveSpeed * 0.6) * GetFrameTime()),
                            zombies[z].position.y + static_cast<float>(sin(targetAngle * DEG2RAD) * (zombies[z].moveSpeed * 0.6) * GetFrameTime())
                        };
                        Rectangle zombieNewCollisionBox = {zombieNewPosition.x + 5, zombieNewPosition.y + 10, zombies[z].collisionWidth, zombies[z].collisionHeight};

                        bool zombieCollision = false;
                        for (int i = 0; i < sizeof(objects) / sizeof(objects[0]); i++) {
                            if (CheckCollisionRecs(zombieNewCollisionBox, objects[i].collisionBox)) {
                                zombieCollision = true;
                                break;
                            }
                        }
                        if (sqrt(pow(targetDeltaX, 2) + pow(targetDeltaY, 2)) > 5.0f && !zombieCollision) {
                            zombies[z].position = zombieNewPosition;
                        } else if (sqrt(pow(targetDeltaX, 2) + pow(targetDeltaY, 2)) > 5.0f && zombieCollision) {
                            zombies[z].target = {
                                (float)GetRandomValue(0, GetScreenWidth()),
                                (float)GetRandomValue(0, GetScreenHeight())
                            };
                        }
                    }
                }
            } else {
                zombies[z].deathTimer += GetFrameTime();
                zombies[z].deathBlinkTimer += GetFrameTime();

                if (zombies[z].deathBlinkTimer >= 0.07f) {
                    zombies[z].deathBlinkTimer = 0.0f;
                    zombies[z].deathBlink = !zombies[z].deathBlink;
                }
            }

            zombies[z].collisionBox.x = zombies[z].position.x + 5;
            zombies[z].collisionBox.y = zombies[z].position.y + 7;
            zombies[z].collisionBox.height = zombies[z].collisionHeight + 3;


            if (zombies[z].hit) {
                zombies[z].hitTimer += GetFrameTime();
                if (zombies[z].hitTimer >= 0.2f) {
                    zombies[z].hit = false;
                }
            }

            zombies[z].frameTimer += GetFrameTime();
            if (zombies[z].frameTimer >= zombies[z].frameSpeed && !zombies[z].spawning) {
                zombies[z].frameTimer = 0.0f;
                zombies[z].currentFrame = (zombies[z].currentFrame + 1) % 7;
            }
        }

        for (size_t g = 0; g < ghosts.size(); g++) {
            if (!ghosts[g].dead) {
                float ghostDeltaX = playerX - ghosts[g].position.x;
                float ghostDeltaY = playerY - ghosts[g].position.y;
                float ghostAngle = atan2(ghostDeltaY, ghostDeltaX) * RAD2DEG;

                if (abs(ghostAngle) > 90 && abs(ghostAngle) < 270) {
                    ghosts[g].flipped = true;
                } else {
                    ghosts[g].flipped = false;
                }

                Vector2 ghostNewPosition = {
                    ghosts[g].position.x + static_cast<float>(cos(ghostAngle * DEG2RAD) * ghosts[g].moveSpeed * GetFrameTime()),
                    ghosts[g].position.y + static_cast<float>(sin(ghostAngle * DEG2RAD) * ghosts[g].moveSpeed * GetFrameTime())
                };
                ghosts[g].position = ghostNewPosition;
            } else {
                ghosts[g].deathTimer += GetFrameTime();
                ghosts[g].deathBlinkTimer += GetFrameTime();

                if (ghosts[g].deathBlinkTimer >= 0.07f) {
                    ghosts[g].deathBlinkTimer = 0.0f;
                    ghosts[g].deathBlink = !ghosts[g].deathBlink;
                }
            }

            ghosts[g].collisionBox.x = ghosts[g].position.x + 5;
            ghosts[g].collisionBox.y = ghosts[g].position.y + 4;
            ghosts[g].collisionBox.height = ghosts[g].collisionHeight + 8;


            if (ghosts[g].hit) {
                ghosts[g].hitTimer += GetFrameTime();
                if (ghosts[g].hitTimer >= 0.2f) {
                    ghosts[g].hit = false;
                }
            }

            ghosts[g].frameTimer += GetFrameTime();
            if (ghosts[g].frameTimer >= ghosts[g].frameSpeed) {
                ghosts[g].frameTimer = 0.0f;
                ghosts[g].currentFrame = (ghosts[g].currentFrame + 1) % 7;
            }
        }

        for (size_t z2 = 0; z2 < zombies2.size(); z2++) {
            if (!zombies2[z2].dead) {
                if (zombies2[z2].spawning) {
                    zombies2[z2].spawnTimer += GetFrameTime();
                    if (zombies2[z2].spawnTimer >= zombies2[z2].spawnAnimationSpeed) {
                        zombies2[z2].spawnTimer = 0.0f;
                        zombies2[z2].currentFrame++;
                        if (zombies2[z2].currentFrame >= 5) {
                            zombies2[z2].spawning = false;
                            zombies2[z2].currentFrame = 0;
                        }
                    }
                } else {
                    float zombie2DeltaX = playerX - zombies2[z2].position.x;
                    float zombie2DeltaY = playerY - zombies2[z2].position.y;
                    float zombie2Distance = sqrt(pow(zombie2DeltaX, 2) + pow(zombie2DeltaY, 2));

                    if (zombie2Distance < zombies2[z2].detectionRadius) {
                        float zombie2Angle = atan2(zombie2DeltaY, zombie2DeltaX) * RAD2DEG;
                        if (abs(zombie2Angle) > 90 && abs(zombie2Angle) < 270) {
                            zombies2[z2].flipped = true;
                        } else {
                            zombies2[z2].flipped = false;
                        }
                        Vector2 zombie2NewPosition = {
                            zombies2[z2].position.x + static_cast<float>(cos(zombie2Angle * DEG2RAD) * zombies2[z2].moveSpeed * GetFrameTime()),
                            zombies2[z2].position.y + static_cast<float>(sin(zombie2Angle * DEG2RAD) * zombies2[z2].moveSpeed * GetFrameTime())
                        };
                        Rectangle zombie2NewCollisionBox = {zombie2NewPosition.x + 5, zombie2NewPosition.y + 10, zombies2[z2].collisionWidth, zombies2[z2].collisionHeight};

                        bool zombie2Collision = false;
                        for (int i = 0; i < sizeof(objects) / sizeof(objects[0]); i++) {
                            if (CheckCollisionRecs(zombie2NewCollisionBox, objects[i].collisionBox)) {
                                zombie2Collision = true;
                                break;
                            }
                        }

                        if (!zombie2Collision) {
                            zombies2[z2].position = zombie2NewPosition;
                        }
                    } else {
                        zombies2[z2].moveTimer += GetFrameTime();
                        if (zombies2[z2].moveTimer >= zombies2[z2].moveInterval) {
                            zombies2[z2].moveTimer = 0.0f;
                            zombies2[z2].target = {
                                (float)GetRandomValue(0, GetScreenWidth()),
                                (float)GetRandomValue(0, GetScreenHeight())
                            };
                        }
                        float targetDeltaX = zombies2[z2].target.x - zombies2[z2].position.x;
                        float targetDeltaY = zombies2[z2].target.y - zombies2[z2].position.y;
                        float targetAngle = atan2(targetDeltaY, targetDeltaX) * RAD2DEG;
                        if (abs(targetAngle) > 90 && abs(targetAngle) < 270) {
                            zombies2[z2].flipped = true;
                        } else {
                            zombies2[z2].flipped = false;
                        }
                        Vector2 zombie2NewPosition = {
                            zombies2[z2].position.x + static_cast<float>(cos(targetAngle * DEG2RAD) * (zombies2[z2].moveSpeed * 0.6) * GetFrameTime()),
                            zombies2[z2].position.y + static_cast<float>(sin(targetAngle * DEG2RAD) * (zombies2[z2].moveSpeed * 0.6) * GetFrameTime())
                        };
                        Rectangle zombie2NewCollisionBox = {zombie2NewPosition.x + 5, zombie2NewPosition.y + 10, zombies2[z2].collisionWidth, zombies2[z2].collisionHeight};

                        bool zombie2Collision = false;
                        for (int i = 0; i < sizeof(objects) / sizeof(objects[0]); i++) {
                            if (CheckCollisionRecs(zombie2NewCollisionBox, objects[i].collisionBox)) {
                                zombie2Collision = true;
                                break;
                            }
                        }
                        if (sqrt(pow(targetDeltaX, 2) + pow(targetDeltaY, 2)) > 5.0f && !zombie2Collision) {
                            zombies2[z2].position = zombie2NewPosition;
                        } else if (sqrt(pow(targetDeltaX, 2) + pow(targetDeltaY, 2)) > 5.0f && zombie2Collision) {
                            zombies2[z2].target = {
                                (float)GetRandomValue(0, GetScreenWidth()),
                                (float)GetRandomValue(0, GetScreenHeight())
                            };
                        }
                    }
                }
            } else {
                zombies2[z2].deathTimer += GetFrameTime();
                zombies2[z2].deathBlinkTimer += GetFrameTime();

                if (zombies2[z2].deathBlinkTimer >= 0.07f) {
                    zombies2[z2].deathBlinkTimer = 0.0f;
                    zombies2[z2].deathBlink = !zombies2[z2].deathBlink;
                }
            }

            zombies2[z2].collisionBox.x = zombies2[z2].position.x + 5;
            zombies2[z2].collisionBox.y = zombies2[z2].position.y + 7;
            zombies2[z2].collisionBox.height = zombies2[z2].collisionHeight + 3;


            if (zombies2[z2].hit) {
                zombies2[z2].hitTimer += GetFrameTime();
                if (zombies2[z2].hitTimer >= 0.2f) {
                    zombies2[z2].hit = false;
                }
            }

            zombies2[z2].frameTimer += GetFrameTime();
            if (zombies2[z2].frameTimer >= zombies2[z2].frameSpeed && !zombies2[z2].spawning) {
                zombies2[z2].frameTimer = 0.0f;
                zombies2[z2].currentFrame = (zombies2[z2].currentFrame + 1) % 7;
            }
        }

        for (size_t g = 0; g < ghosts.size(); g++) {
            if (CheckCollisionRecs(collisionBox, ghosts[g].collisionBox) && !ghosts[g].dead) {
                if (playerDamageCooldown <= 0.0f) {
                    playerHealth--;
                    playerHit = true;
                    playerHitTimer = 0.0f;
                    playerDamageCooldown = playerDamageCooldownDuration;
                }
            }
        }

        for (size_t z = 0; z < zombies.size(); z++) {
            if (CheckCollisionRecs(collisionBox, zombies[z].collisionBox) && !zombies[z].dead) {
                if (playerDamageCooldown <= 0.0f) {
                    playerHealth--;
                    playerHit = true;
                    playerHitTimer = 0.0f;
                    playerDamageCooldown = playerDamageCooldownDuration;
                }
            }
        }

        for (size_t z2 = 0; z2 < zombies2.size(); z2++) {
            if (CheckCollisionRecs(collisionBox, zombies2[z2].collisionBox) && !zombies2[z2].dead) {
                if (playerDamageCooldown <= 0.0f) {
                    playerHealth--;
                    playerHit = true;
                    playerHitTimer = 0.0f;
                    playerDamageCooldown = playerDamageCooldownDuration;
                }
            }
        }

        collisionBox.x = playerX + 15;
        collisionBox.y = playerY;

        if (playerDamageCooldown > 0.0f) {
            playerDamageCooldown -= GetFrameTime();
        }

        if (playerHit){
            playerHitTimer += GetFrameTime();
            if (playerHitTimer >= playerHitDuration){
                playerHit = false;
            }
        }

        if (playerHealth <= 0) {
            gameOver = true;
            PlaySound(gameOverSound);
        }
        UpdateMusicStream(backgroundMusicGlobal);
    } else {
        StopMusicStream(backgroundMusicGlobal);

        if (IsKeyPressed(KEY_R)) {
            playerHealth = 10;
            playerX = 320.0f;
            playerY = 240.0f;
            playerDamageCooldown = 0.0f;
            zombies.clear();
            ghosts.clear();
            zombies2.clear();
            gameOver = false;
            playerWon = false;
            monstersKilled = 0;
            PlayMusicStream(backgroundMusicGlobal);
        }
    }

    Vector2 target = {playerX + 24, playerY + 15};
    camera.target.x = camera.target.x + (target.x - camera.target.x) * 0.1f;
    camera.target.y = camera.target.y + (target.y - camera.target.y) * 0.1f;

    if (IsKeyDown(KEY_Z)) {
        zoom += 0.01f;
    }
    if (IsKeyDown(KEY_X)) {
        zoom -= 0.01f;
        if (zoom < 0.7f) zoom = 0.7f;
    }
    camera.zoom = zoom;

    if (camera.target.y < 122) {
        camera.target.y = 122;
    }
    if (camera.target.y > 358) {
        camera.target.y = 358;
    }
    if (camera.target.x < 162) {
        camera.target.x = 162;
    }
    if (camera.target.x > 479) {
        camera.target.x = 479;
    }
}
// Function to initialize the game
void InitGame() {
    InitWindow(640, 480, "Raylib Game");
    InitAudioDevice();


    backgroundTexture = LoadTexture("/Users/sergiobenavides/Desktop/final_game/assets/images/backgroundfinal.png");
    backgroundBorder = LoadTexture("/Users/sergiobenavides/Desktop/final_game/assets/images/border.png");
    playerTexture = LoadTexture("/Users/sergiobenavides/Desktop/final_game/assets/images/Character-and-Zombie.png");
    gunTexture = LoadTexture("/Users/sergiobenavides/Desktop/final_game/assets/images/gun.png");
    bulletTexture = LoadTexture("/Users/sergiobenavides/Desktop/final_game/assets/images/bullet.png");
    objectTexture = LoadTexture("/Users/sergiobenavides/Desktop/final_game/assets/images/objects.png");

    backgroundMusicGlobal = LoadMusicStream("/Users/sergiobenavides/Desktop/final_game/assets/sounds/song.mp3");
    shotSoundGlobal = LoadSound("/Users/sergiobenavides/Desktop/final_game/assets/sounds/shot.mp3");
    gameOverSound = LoadSound("/Users/sergiobenavides/Desktop/final_game/assets/sounds/gameover.mp3");
    winSound = LoadSound("/Users/sergiobenavides/Desktop/final_game/assets/sounds/win.mp3");

    camera.target = {playerX + 24, playerY + 15}; // Center on player
    camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f}; // Center offset
    camera.rotation = 0.0f;
    camera.zoom = zoom; // Set initial zoom level


    // Store sounds in global variables for later use
    // Play background music in a loop
    PlayMusicStream(backgroundMusicGlobal);
    SetMusicVolume(backgroundMusicGlobal, 0.8f);
    SetSoundVolume(shotSoundGlobal, 0.2f); // Reduce shot sound volume (0.0f - 1.0f)

    if (!IsMusicStreamPlaying(backgroundMusicGlobal)) {
        PlayMusicStream(backgroundMusicGlobal);
    }

    UpdateMusicStream(backgroundMusicGlobal);


    for (int i = 0; i < 4; i++) {
        playerIdleFrames[i] = (Rectangle){(float)(i * 48), 63.0f, 48.0f, 30.0f};
    }

    for (int i = 0; i < 7; i++) {
        playerWalkFrames[i] = (Rectangle){(float)(i * 48), 93.0f, 48.0f, 30.0f};
    }

    for (int i = 0; i < 4; i++) {
        gunFrames[i] = (Rectangle){(float)(i * 48 + 14), 7.0f, 29.0f, 17.0f};
    }

    for (int i = 0; i < 100; i++) {
        bullets[i].active = false;
        bullets[i].collided = false;
        bullets[i].collisionFrame = 0;
        bullets[i].collisionTimer = 0.0f;
    }

    int objectIndex = 0;

    ObjectPlacement object1Placements[] = {
        {73, {0, 0}}, {74, {0, 0}}, {88, {0, 0}}, {89, {0, 0}},
        {167, {0, 0}}, {168, {0, 0}}, {175, {0, 0}}, {174, {0, 0}}
    };

    ObjectPlacement object2Placements[] = {
        {41, {0, 0}}, {59, {0, 0}}, {77, {0, 0}}, {95, {0, 0}}, {49, {0, 0}}, {67, {0, 0}},
        {85, {0, 0}}, {103, {0, 0}}, {165, {0, 0}}, {224, {0, 0}}, {242, {0, 0}}
    };

    ObjectPlacement object3Placements[] = {
        {203, {0, 0}}, {221, {0, 0}}, {211, {0, 0}}, {229, {0, 0}}
    };

    ObjectPlacement object4Placements[] = {
        {115, {0, 0}}, {116, {0, 0}}, {117, {0, 0}}, {118, {0, 0}}, {119, {0, 0}},
        {170, {0, 0}}, {171, {0, 0}}, {172, {0, 0}}
    };

    for (int i = 0; i < sizeof(object1Placements) / sizeof(object1Placements[0]); i++) {
        if (objectIndex < 30) {
            objects[objectIndex].texture = objectTexture;
            objects[objectIndex].sourceRect = {0, 0, 34, 34};
            objects[objectIndex].position = {
                (float)((object1Placements[i].tile % (GetScreenWidth() / 34)) * 34),
                (float)((object1Placements[i].tile / (GetScreenWidth() / 34)) * 34)
            };
            objects[objectIndex].collisionBox = {objects[objectIndex].position.x, objects[objectIndex].position.y, 34, 34};
            objects[objectIndex].collisionBox.y += 5;
            objects[objectIndex].collisionBox.height -= 8;
            objects[objectIndex].collisionBox.x += 6;
            objects[objectIndex].collisionBox.width -= 12;
            objectIndex++;
        }
    }

    for (int i = 0; i < sizeof(object2Placements) / sizeof(object2Placements[0]); i++) {
        if (objectIndex < 30) {
            objects[objectIndex].texture = objectTexture;
            objects[objectIndex].sourceRect = {34, 0, 34, 34};
            objects[objectIndex].position = {
                (float)((object2Placements[i].tile % (GetScreenWidth() / 34)) * 34),
                (float)((object2Placements[i].tile / (GetScreenWidth() / 34)) * 34)
            };
            objects[objectIndex].collisionBox = {objects[objectIndex].position.x, objects[objectIndex].position.y, 34, 34};
            objects[objectIndex].collisionBox.y += 5;
            objects[objectIndex].collisionBox.height -= 8;
            objects[objectIndex].collisionBox.x += 6;
            objects[objectIndex].collisionBox.width -= 12;
            objectIndex++;
        }
    }

    for (int i = 0; i < sizeof(object3Placements) / sizeof(object3Placements[0]); i++) {
        if (objectIndex < 30) {
            objects[objectIndex].texture = objectTexture;
            objects[objectIndex].sourceRect = {68, 0, 34, 34};
            objects[objectIndex].position = {
                (float)((object3Placements[i].tile % (GetScreenWidth() / 34)) * 34),
                (float)((object3Placements[i].tile / (GetScreenWidth() / 34)) * 34)
            };
            objects[objectIndex].collisionBox = {objects[objectIndex].position.x, objects[objectIndex].position.y, 34, 34};
            objects[objectIndex].collisionBox.y += 5;
            objects[objectIndex].collisionBox.height -= 8;
            objects[objectIndex].collisionBox.x += 6;
            objects[objectIndex].collisionBox.width -= 12;
            objectIndex++;
        }
    }

    for (int i = 0; i < sizeof(object4Placements) / sizeof(object4Placements[0]); i++) {
        if (objectIndex < 30) {
            objects[objectIndex].texture = objectTexture;
            objects[objectIndex].sourceRect = {102, 0, 34, 34};
            objects[objectIndex].position = {
                (float)((object4Placements[i].tile % (GetScreenWidth() / 34)) * 34),
                (float)((object4Placements[i].tile / (GetScreenWidth() / 34)) * 34)
            };
            objects[objectIndex].collisionBox = {objects[objectIndex].position.x, objects[objectIndex].position.y, 34, 34};
            objects[objectIndex].collisionBox.y += 5;
            objects[objectIndex].collisionBox.height -= 8;
            objects[objectIndex].collisionBox.x += 6;
            objects[objectIndex].collisionBox.width -= 12;
            objectIndex++;
        }
    }
    
    PlayMusicStream(backgroundMusicGlobal);// Play background music when game starts.

    SetTargetFPS(60);
}

void DrawGame() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode2D(camera); // Start camera mode

    // Draw the background texture.
    DrawTexture(backgroundTexture, 0, 0, WHITE);

    // Draw all objects and other game elements.
    for (int i = 0; i < sizeof(objects) / sizeof(objects[0]); i++) {
        DrawTextureRec(objects[i].texture, objects[i].sourceRect, objects[i].position, WHITE);
    }

    float bulletScale = 0.5;
    float bulletWidth = 32.0f * bulletScale;
    float bulletHeight = 32.0f * bulletScale;

    Rectangle bulletCollisionFrames[4] = {
        {70, 32, 32, 32},
        {117, 32, 32, 32},
        {164, 32, 32, 32},
        {212, 32, 32, 32}
    };

    for (int i = 0; i < 100; i++) {
        if (bullets[i].active) {
            if (bullets[i].collided) {
                float offsetX = cos(bullets[i].rotation * DEG2RAD) * (bulletWidth / 2);
                float offsetY = sin(bullets[i].rotation * DEG2RAD) * (bulletHeight / 2);
                DrawTexturePro(bulletTexture, bulletCollisionFrames[bullets[i].collisionFrame], {bullets[i].position.x + offsetX, bullets[i].position.y + offsetY, bulletWidth, bulletHeight}, {bulletWidth / 2, bulletHeight / 2}, bullets[i].rotation, WHITE);
            } else {
                DrawTexturePro(bulletTexture, {0, 0, 32, 32}, {bullets[i].position.x, bullets[i].position.y, bulletWidth, bulletHeight}, {bulletWidth /2, bulletHeight / 2}, bullets[i].rotation, WHITE);
            }
        }
    }

    for (size_t z = 0; z < zombies.size(); z++) {
        Color zombieColor = WHITE;
        if (zombies[z].hit) {
            zombieColor = RED;
        }

        Rectangle zombieSourceRect;
        if (zombies[z].spawning) {
            zombieSourceRect = zombies[z].spawnFrames[zombies[z].currentFrame];
        } else {
            zombieSourceRect = zombies[z].walkFrames[zombies[z].currentFrame];
        }

        if (zombies[z].dead && zombies[z].deathBlink) {
            DrawTextureRec(zombies[z].texture, zombieSourceRect, zombies[z].position, WHITE);
        } else if (!zombies[z].dead) {
            if (zombies[z].flipped) {
                zombieSourceRect.width = -zombieSourceRect.width;
                DrawTexturePro(zombies[z].texture, zombieSourceRect, {zombies[z].position.x, zombies[z].position.y, zombieSourceRect.width, zombieSourceRect.height}, {0, 0}, 0, zombieColor);
            } else {
                DrawTextureRec(zombies[z].texture, zombieSourceRect, zombies[z].position, zombieColor);
            }
        }

        // Remove the zombie collision box drawing
        // DrawRectangleRec(zombies[z].collisionBox, Fade(RED, 0.5f));
    }

    for (size_t g = 0; g < ghosts.size(); g++) {
        Color ghostColor = WHITE;
        if (ghosts[g].hit) {
            ghostColor = RED;
        }

        Rectangle ghostSourceRect = ghosts[g].walkFrames[ghosts[g].currentFrame];

        if (ghosts[g].dead && ghosts[g].deathBlink) {
            DrawTextureRec(ghosts[g].texture, ghostSourceRect, ghosts[g].position, WHITE);
        } else if (!ghosts[g].dead) {
            if (ghosts[g].flipped) {
                ghostSourceRect.width = -ghostSourceRect.width;
                DrawTexturePro(ghosts[g].texture, ghostSourceRect, {ghosts[g].position.x, ghosts[g].position.y, ghostSourceRect.width, ghostSourceRect.height}, {0, 0}, 0, ghostColor);
            } else {
                DrawTextureRec(ghosts[g].texture, ghostSourceRect, ghosts[g].position, ghostColor);
            }
        }
        // Remove the ghost collision box drawing
        // DrawRectangleRec(ghosts[g].collisionBox, Fade(BLUE, 0.5f));
    }

    for (size_t z2 = 0; z2 < zombies2.size(); z2++) {
        Color zombie2Color = WHITE;
        if (zombies2[z2].hit) {
            zombie2Color = RED;
        }

        Rectangle zombie2SourceRect;
        if (zombies2[z2].spawning) {
            zombie2SourceRect = zombies2[z2].spawnFrames[zombies2[z2].currentFrame];
        } else {
            zombie2SourceRect = zombies2[z2].walkFrames[zombies2[z2].currentFrame];
        }

        if (zombies2[z2].dead && zombies2[z2].deathBlink) {
            DrawTextureRec(zombies2[z2].texture, zombie2SourceRect, zombies2[z2].position, WHITE);
        } else if (!zombies2[z2].dead) {
            if (zombies2[z2].flipped) {
                zombie2SourceRect.width = -zombie2SourceRect.width;
                DrawTexturePro(zombies2[z2].texture, zombie2SourceRect, {zombies2[z2].position.x, zombies2[z2].position.y, zombie2SourceRect.width, zombie2SourceRect.height}, {0, 0}, 0, zombie2Color);
            } else {
                DrawTextureRec(zombies2[z2].texture, zombie2SourceRect, zombies2[z2].position, zombie2Color);
            }
        }

        // Remove the zombie2 collision box drawing
        // DrawRectangleRec(zombies2[z2].collisionBox, Fade(MAGENTA, 0.5f));
    }

    Rectangle sourceRect = isMoving ? playerWalkFrames[currentFrame] : playerIdleFrames[currentFrame];
    Vector2 origin = {24, 15};

    Color playerColor = WHITE;
    if (playerHit) {
        playerColor = RED;
    }

    if (playerFlipped) {
        sourceRect.width = -sourceRect.width;
        DrawTexturePro(playerTexture, sourceRect, {playerX + 24, playerY + 15, sourceRect.width, sourceRect.height}, origin, 0, playerColor);
    } else {
        DrawTexturePro(playerTexture, sourceRect, {playerX + 24, playerY + 15, sourceRect.width, sourceRect.height}, origin, 0, playerColor);
    }

    float gunScale = 0.5f;
    float gunWidth = gunFrames[currentGunFrame].width * gunScale;
    float gunHeight = gunFrames[currentGunFrame].height * gunScale;

    float gunX = playerX + 24;
    float gunY = playerY + 15;

    Vector2 gunOrigin = {0.0f, gunFrames[currentGunFrame].height / 3.0f};

    if (playerFlipped) {
        Rectangle gunSourceRect = gunFrames[currentGunFrame];
        gunSourceRect.height = -gunSourceRect.height;
        DrawTexturePro(gunTexture, gunSourceRect, {gunX, gunY, gunWidth, gunHeight}, gunOrigin, gunRotation, WHITE);
    } else {
        DrawTexturePro(gunTexture, gunFrames[currentGunFrame], {gunX, gunY, gunWidth, gunHeight}, gunOrigin, gunRotation, WHITE);
    }

    // Remove the red dot drawing
    // float redDotOffsetX = 10.0f;
    // float redDotOffsetY = playerFlipped ? 0.5f : -3.5f;

    // // Calculate world space offsets
    // float worldRedDotOffsetX = redDotOffsetX * cos(gunRotation * DEG2RAD) - redDotOffsetY * sin(gunRotation * DEG2RAD);
    // float worldRedDotOffsetY = redDotOffsetX * sin(gunRotation * DEG2RAD) + redDotOffsetY * cos(gunRotation * DEG2RAD);

    // //Apply the world space offset to the player center.
    // float gunCenterX = playerX + 24 + worldRedDotOffsetX;
    // float gunCenterY = playerY + 15 + worldRedDotOffsetY;

    // DrawCircle(gunCenterX, gunCenterY, 1.5, RED);

    DrawTexture(backgroundBorder, 0, 0, WHITE);

    int healthBarWidth = 30;
    int healthBarHeight = 3;
    int healthBarOffsetY = 25;

    DrawRectangle(playerX + 10, playerY + healthBarOffsetY, healthBarWidth, healthBarHeight, LIGHTGRAY);
    DrawRectangle(playerX + 10, playerY + healthBarOffsetY, (int)(healthBarWidth * ((float)playerHealth / 10.0f)), healthBarHeight, GREEN);

    EndMode2D(); // End camera mode

    // Draw the monster kill counter outside of camera mode
    DrawText(TextFormat("Monsters Killed: %i/%i", monstersKilled, monstersToKill), 10, 5, 20, WHITE);

    if (gameOver) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
        DrawText("Game Over", GetScreenWidth() / 2 - MeasureText("Game Over", 40) / 2, GetScreenHeight() / 2 - 20, 40, RED);
        DrawText("Press 'R' to Restart", GetScreenWidth() / 2 - MeasureText("Press 'R' to Restart", 20) / 2, GetScreenHeight() / 2 + 30, 20, WHITE);
    }
    if (playerWon) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
        DrawText("You Win!", GetScreenWidth() / 2 - MeasureText("You Win!", 40) / 2, GetScreenHeight() / 2 - 20, 40, GREEN);
        DrawText("Press 'R' to Restart", GetScreenWidth() / 2 - MeasureText("Press 'R' to Restart", 20) / 2, GetScreenHeight() / 2 + 30, 20, WHITE);
    }

    EndDrawing();
}

// Function to unload the game
void UnloadGame() {
    UnloadTexture(backgroundTexture);
    UnloadTexture(backgroundBorder);
    UnloadTexture(playerTexture);
    UnloadTexture(gunTexture);
    UnloadTexture(bulletTexture);
    UnloadTexture(objectTexture);

    for (auto& zombie : zombies) {
        UnloadTexture(zombie.texture);
    }
    for (auto& ghost : ghosts) {
        UnloadTexture(ghost.texture);
    }
    for (auto& zombie2 : zombies2) {
        UnloadTexture(zombie2.texture);
    }
    UnloadMusicStream(backgroundMusicGlobal);
    UnloadSound(shotSoundGlobal);
    UnloadSound(gameOverSound);
    UnloadSound(winSound);

    CloseAudioDevice(); // Close the audio device
    CloseWindow();
}

int main() {
    InitGame();

    while (!WindowShouldClose()) {
        UpdateGame();
        DrawGame();
    }

    UnloadGame();

    return 0;
}
