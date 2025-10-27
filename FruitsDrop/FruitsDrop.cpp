#include "raylib.h"
#include <string>
#include <vector>
#include <algorithm>

const int screenWidth = 800;
const int screenHeight = 600;

// Структура для меню
struct Button {
    Rectangle rect;
    std::string text;
};

// Структура для fruits
struct Fruit {
    Rectangle rect;
    int type; // 0-3: хорошие, 4-5: плохие, 6-9: бонусные
    float speed;
    bool active;
    Color color;
};

// Структура для score
struct PlayerBestScores {
    int classicBest = 0;
    int survivalBest = 0;
    int timeAttackBest = 0;
    int twoPlayerBest = 0;
};

// Структура для игрока
struct Player {
    Rectangle basket;
    int score;
    int lives;
    Color color;
    std::string name;
};

// Global variables
int currentScore = 0;
int gameMode = 0; // 0: menu, 1: classic, 2: survival, 3: time attack, 4: two players
float gameTime = 120.0f;
int lives = 3;
int missedFruits = 0;
int maxMissedFruits = 10;
bool gameOver = false;
bool inMenu = true;
bool showTopRating = false;

float fruitSpeed = 3.0f;
float spawnRate = 1.0f;
float timeSinceLastSpawn = 0.0f;

// Бонусные эффекты
float slowMotionTimer = 0.0f;
float doublePointsTimer = 0.0f;
bool slowMotionActive = false;
bool doublePointsActive = false;

// Для одного игрока
PlayerBestScores playerScores;

// Для двух игроков
Player player1, player2;
bool twoPlayerGameOver = false;

// Функции для работы с рекордами
void LoadPlayerScores() {
    playerScores.classicBest = 0;
    playerScores.survivalBest = 0;
    playerScores.timeAttackBest = 0;
    playerScores.twoPlayerBest = 0;
}

void SavePlayerScore(int score, const std::string& mode) {
    if (mode == "Classic" && score > playerScores.classicBest) {
        playerScores.classicBest = score;
    }
    else if (mode == "Survival" && score > playerScores.survivalBest) {
        playerScores.survivalBest = score;
    }
    else if (mode == "Time Attack" && score > playerScores.timeAttackBest) {
        playerScores.timeAttackBest = score;
    }
    else if (mode == "Two Players" && score > playerScores.twoPlayerBest) {
        playerScores.twoPlayerBest = score;
    }
}

void DrawTopRating() {
    ClearBackground(SKYBLUE);

    DrawText("MY BEST SCORES", screenWidth / 2 - 150, 50, 40, DARKBLUE);

    int startY = 150;

    // Classic Mode
    DrawText("CLASSIC MODE", screenWidth / 2 - 100, startY, 30, GREEN);
    DrawText("Best Score:", screenWidth / 2 - 150, startY + 40, 25, WHITE);
    DrawText(TextFormat("%d", playerScores.classicBest), screenWidth / 2 + 50, startY + 40, 25, GOLD);

    // Survival Mode
    DrawText("SURVIVAL MODE", screenWidth / 2 - 100, startY + 100, 30, BLUE);
    DrawText("Best Score:", screenWidth / 2 - 150, startY + 140, 25, WHITE);
    DrawText(TextFormat("%d", playerScores.survivalBest), screenWidth / 2 + 50, startY + 140, 25, GOLD);

    // Time Attack Mode
    DrawText("TIME ATTACK MODE", screenWidth / 2 - 120, startY + 200, 30, ORANGE);
    DrawText("Best Score:", screenWidth / 2 - 150, startY + 240, 25, WHITE);
    DrawText(TextFormat("%d", playerScores.timeAttackBest), screenWidth / 2 + 50, startY + 240, 25, GOLD);

    // Two Players Mode
    DrawText("TWO PLAYERS MODE", screenWidth / 2 - 120, startY + 280, 30, MAGENTA);
    DrawText("Best Score:", screenWidth / 2 - 150, startY + 320, 25, WHITE);
    DrawText(TextFormat("%d", playerScores.twoPlayerBest), screenWidth / 2 + 50, startY + 320, 25, GOLD);

    DrawText("Press ESC to return", screenWidth / 2 - 100, screenHeight - 50, 20, WHITE);
}

void MoveRectangle(Rectangle& rec, bool useArrowKeys) {
    float moveSpeed = 10.0f;

    if (slowMotionActive) {
        moveSpeed = 6.0f;
    }

    if ((useArrowKeys && IsKeyDown(KEY_LEFT)) || (!useArrowKeys && IsKeyDown(KEY_A))) {
        rec.x -= moveSpeed;
    }
    if ((useArrowKeys && IsKeyDown(KEY_RIGHT)) || (!useArrowKeys && IsKeyDown(KEY_D))) {
        rec.x += moveSpeed;
    }
    if (rec.x + rec.width > screenWidth) {
        rec.x = screenWidth - rec.width;
    }
    if (rec.x < 0) {
        rec.x = 0;
    }
}

void MovePlayer(Player& player, bool useArrowKeys) {
    float moveSpeed = 10.0f;

    if (slowMotionActive) {
        moveSpeed = 6.0f;
    }

    if (useArrowKeys) {
        if (IsKeyDown(KEY_LEFT)) player.basket.x -= moveSpeed;
        if (IsKeyDown(KEY_RIGHT)) player.basket.x += moveSpeed;
    }
    else {
        if (IsKeyDown(KEY_A)) player.basket.x -= moveSpeed;
        if (IsKeyDown(KEY_D)) player.basket.x += moveSpeed;
    }

    if (player.basket.x < 0) player.basket.x = 0;
    if (player.basket.x + player.basket.width > screenWidth) {
        player.basket.x = screenWidth - player.basket.width;
    }
}

Fruit CreateFruit() {
    Fruit fruit;
    fruit.rect = { (float)GetRandomValue(50, screenWidth - 100), -50, 40, 40 };

    int randomChance = GetRandomValue(0, 100);

    if (randomChance < 8) {
        fruit.type = GetRandomValue(6, 8);
    }
    else if (randomChance < 35) {
        fruit.type = GetRandomValue(4, 5);
    }
    else {
        fruit.type = GetRandomValue(0, 3);
    }

    fruit.active = true;

    float baseSpeed = fruitSpeed + GetRandomValue(0, 2);
    if (slowMotionActive) baseSpeed *= 0.4f;

    fruit.speed = baseSpeed;

    switch (fruit.type) {
    case 0: fruit.color = RED; break;
    case 1: fruit.color = YELLOW; break;
    case 2: fruit.color = GREEN; break;
    case 3: fruit.color = PURPLE; break;
    case 4: fruit.color = BROWN; break;
    case 5: fruit.color = GRAY; break;
    case 6: fruit.color = BLUE; break;
    case 7: fruit.color = GOLD; break;
    case 8: fruit.color = PINK; break;
    }

    return fruit;
}

void UpdateBonusEffects() {
    if (slowMotionActive) {
        slowMotionTimer -= GetFrameTime();
        if (slowMotionTimer <= 0) {
            slowMotionActive = false;
        }
    }

    if (doublePointsActive) {
        doublePointsTimer -= GetFrameTime();
        if (doublePointsTimer <= 0) {
            doublePointsActive = false;
        }
    }
}

void ApplyBonusEffect(int bonusType) {
    switch (bonusType) {
    case 6: // Slow motion
        slowMotionActive = true;
        slowMotionTimer = 7.0f;
        break;

    case 7: // Double points
        doublePointsActive = true;
        doublePointsTimer = 10.0f;
        break;

    case 8: // Extra life
        if (gameMode == 4) {
            if (player1.lives < 5) player1.lives++;
            if (player2.lives < 5) player2.lives++;
        }
        else {
            if (lives < 5) lives++;
        }
        break;
    }
}

void UpdateFruits(std::vector<Fruit>& fruits, Rectangle& basket) {
    for (auto& fruit : fruits) {
        if (fruit.active) {
            float currentSpeed = fruit.speed;
            fruit.rect.y += currentSpeed;

            if (CheckCollisionRecs(fruit.rect, basket)) {
                if (fruit.type <= 3) {
                    int points = 100;
                    if (doublePointsActive) points *= 2;
                    currentScore += points;
                }
                else if (fruit.type <= 5) {
                    lives--;
                    if (lives <= 0) gameOver = true;
                }
                else {
                    ApplyBonusEffect(fruit.type);
                    currentScore += 200;
                }
                fruit.active = false;
            }

            if (fruit.rect.y > screenHeight) {
                if (fruit.type <= 3) {
                    missedFruits++;
                    if (missedFruits >= maxMissedFruits) gameOver = true;
                }
                fruit.active = false;
            }
        }
    }

    for (int i = fruits.size() - 1; i >= 0; i--) {
        if (!fruits[i].active) {
            fruits.erase(fruits.begin() + i);
        }
    }
}

void UpdateFruitsTwoPlayers(std::vector<Fruit>& fruits) {
    for (auto& fruit : fruits) {
        if (fruit.active) {
            float currentSpeed = fruit.speed;
            fruit.rect.y += currentSpeed;

            if (CheckCollisionRecs(fruit.rect, player1.basket)) {
                if (fruit.type <= 3) {
                    int points = 100;
                    if (doublePointsActive) points *= 2;
                    player1.score += points;
                }
                else if (fruit.type <= 5) {
                    player1.lives--;
                }
                else {
                    ApplyBonusEffect(fruit.type);
                    player1.score += 200;
                }
                fruit.active = false;
            }
            else if (CheckCollisionRecs(fruit.rect, player2.basket)) {
                if (fruit.type <= 3) {
                    int points = 100;
                    if (doublePointsActive) points *= 2;
                    player2.score += points;
                }
                else if (fruit.type <= 5) {
                    player2.lives--;
                }
                else {
                    ApplyBonusEffect(fruit.type);
                    player2.score += 200;
                }
                fruit.active = false;
            }

            if (fruit.rect.y > screenHeight) {
                fruit.active = false;
            }
        }
    }

    for (int i = fruits.size() - 1; i >= 0; i--) {
        if (!fruits[i].active) {
            fruits.erase(fruits.begin() + i);
        }
    }
}

void DrawFruits(const std::vector<Fruit>& fruits) {
    for (const auto& fruit : fruits) {
        if (fruit.active) {
            DrawRectangleRec(fruit.rect, fruit.color);

            if (fruit.type >= 6) {
                DrawRectangle(fruit.rect.x + 15, fruit.rect.y + 5, 10, 30, WHITE);
                DrawRectangle(fruit.rect.x + 5, fruit.rect.y + 15, 30, 10, WHITE);
            }
        }
    }
}

void ResetGame() {
    currentScore = 0;
    lives = 3;
    missedFruits = 0;
    gameOver = false;
    gameTime = 120.0f;
    fruitSpeed = 3.0f;
    spawnRate = 1.0f;
    twoPlayerGameOver = false;

    slowMotionActive = false;
    doublePointsActive = false;
    slowMotionTimer = 0.0f;
    doublePointsTimer = 0.0f;

    player1 = { {100, screenHeight - 100, 80, 60}, 0, 3, BLUE, "Player 1" };
    player2 = { {screenWidth - 180, screenHeight - 100, 80, 60}, 0, 3, RED, "Player 2" };
}

void DrawTwoPlayersGame(const std::vector<Fruit>& fruits) {
    DrawRectangleRec(player1.basket, player1.color);
    DrawRectangleRec(player2.basket, player2.color);

    DrawText("P1", player1.basket.x + 30, player1.basket.y - 25, 20, WHITE);
    DrawText("P2", player2.basket.x + 30, player2.basket.y - 25, 20, WHITE);

    DrawFruits(fruits);

    DrawText("Player 1", 10, 10, 25, BLUE);
    DrawText(TextFormat("Score: %d", player1.score), 10, 40, 20, WHITE);
    DrawText(TextFormat("Lives: %d", player1.lives), 10, 65, 20, WHITE);

    DrawText("Player 2", screenWidth - 120, 10, 25, RED);
    DrawText(TextFormat("Score: %d", player2.score), screenWidth - 120, 40, 20, WHITE);
    DrawText(TextFormat("Lives: %d", player2.lives), screenWidth - 120, 65, 20, WHITE);

    int totalScore = player1.score + player2.score;
    DrawText(TextFormat("Total Score: %d", totalScore), screenWidth / 2 - 80, 10, 25, GREEN);

    DrawText("Controls:", 10, screenHeight - 120, 20, WHITE);
    DrawText("Player 1: A/D", 10, screenHeight - 95, 18, BLUE);
    DrawText("Player 2: Arrow Keys", 10, screenHeight - 70, 18, RED);

    int bonusY = 100;
    if (slowMotionActive) {
        DrawText("Slow Motion!", screenWidth / 2 - 60, bonusY, 20, BLUE);
    }
    if (doublePointsActive) {
        DrawText("Double Points!", screenWidth / 2 - 70, bonusY + 25, 20, GOLD);
    }
}

void DrawMenu() {
    ClearBackground(SKYBLUE);

    DrawText("FruityDrop", screenWidth / 2 - 100, 80, 40, DARKBLUE);

    Rectangle classicBtn = { screenWidth / 2 - 100, 180, 200, 50 };
    Rectangle survivalBtn = { screenWidth / 2 - 100, 250, 200, 50 };
    Rectangle timeBtn = { screenWidth / 2 - 100, 320, 200, 50 };
    Rectangle twoPlayerBtn = { screenWidth / 2 - 100, 390, 200, 50 };
    Rectangle ratingBtn = { screenWidth / 2 - 100, 460, 200, 50 };
    Rectangle exitBtn = { screenWidth / 2 - 100, 530, 200, 50 };

    DrawRectangleRec(classicBtn, GREEN);
    DrawRectangleRec(survivalBtn, BLUE);
    DrawRectangleRec(timeBtn, ORANGE);
    DrawRectangleRec(twoPlayerBtn, MAGENTA);
    DrawRectangleRec(ratingBtn, PURPLE);
    DrawRectangleRec(exitBtn, RED);

    DrawText("Classic", classicBtn.x + 60, classicBtn.y + 15, 20, WHITE);
    DrawText("Survival", survivalBtn.x + 50, survivalBtn.y + 15, 20, WHITE);
    DrawText("Time Attack", timeBtn.x + 35, timeBtn.y + 15, 20, WHITE);
    DrawText("Two Players", twoPlayerBtn.x + 45, twoPlayerBtn.y + 15, 20, WHITE);
    DrawText("My Scores", ratingBtn.x + 50, ratingBtn.y + 15, 20, WHITE);
    DrawText("Exit", exitBtn.x + 75, exitBtn.y + 15, 20, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePoint = GetMousePosition();
        if (CheckCollisionPointRec(mousePoint, classicBtn)) {
            inMenu = false;
            gameMode = 1;
            ResetGame();
        }
        if (CheckCollisionPointRec(mousePoint, survivalBtn)) {
            inMenu = false;
            gameMode = 2;
            ResetGame();
        }
        if (CheckCollisionPointRec(mousePoint, timeBtn)) {
            inMenu = false;
            gameMode = 3;
            ResetGame();
        }
        if (CheckCollisionPointRec(mousePoint, twoPlayerBtn)) {
            inMenu = false;
            gameMode = 4;
            ResetGame();
        }
        if (CheckCollisionPointRec(mousePoint, ratingBtn)) {
            showTopRating = true;
        }
        if (CheckCollisionPointRec(mousePoint, exitBtn)) {
            CloseWindow();
        }
    }
}

int main(void) {
    InitWindow(screenWidth, screenHeight, "FruityDrop");
    SetTargetFPS(60);

    LoadPlayerScores();

    Rectangle basket = { screenWidth / 2 - 50, screenHeight - 100, 100, 60 };
    std::vector<Fruit> fruits;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (showTopRating) {
            DrawTopRating();
            if (IsKeyPressed(KEY_ESCAPE)) {
                showTopRating = false;
            }
        }
        else if (inMenu) {
            DrawMenu();
        }
        else {
            if (gameMode == 4) {
                if (!twoPlayerGameOver) {
                    UpdateBonusEffects();

                    MovePlayer(player1, false);
                    MovePlayer(player2, true);

                    timeSinceLastSpawn += GetFrameTime();
                    if (timeSinceLastSpawn >= spawnRate) {
                        fruits.push_back(CreateFruit());
                        timeSinceLastSpawn = 0.0f;
                    }

                    UpdateFruitsTwoPlayers(fruits);

                    if (player1.lives <= 0 && player2.lives <= 0) {
                        twoPlayerGameOver = true;
                    }

                    DrawTwoPlayersGame(fruits);

                }
                else {
                    int totalScore = player1.score + player2.score;
                    SavePlayerScore(totalScore, "Two Players");

                    DrawText("GAME OVER!", screenWidth / 2 - 100, 150, 40, RED);

                    DrawText(TextFormat("Player 1: %d points", player1.score), screenWidth / 2 - 100, 220, 30, BLUE);
                    DrawText(TextFormat("Player 2: %d points", player2.score), screenWidth / 2 - 100, 260, 30, RED);
                    DrawText(TextFormat("Total Score: %d", totalScore), screenWidth / 2 - 100, 300, 30, GREEN);

                    if (player1.score > player2.score) {
                        DrawText("Player 1 Wins!", screenWidth / 2 - 80, 350, 35, BLUE);
                    }
                    else if (player2.score > player1.score) {
                        DrawText("Player 2 Wins!", screenWidth / 2 - 80, 350, 35, RED);
                    }
                    else {
                        DrawText("It's a Tie!", screenWidth / 2 - 70, 350, 35, YELLOW);
                    }

                    DrawText("Press SPACE for menu", screenWidth / 2 - 120, 420, 20, WHITE);
                    DrawText("Press R for My Scores", screenWidth / 2 - 120, 450, 20, WHITE);

                    if (IsKeyPressed(KEY_SPACE)) {
                        inMenu = true;
                        fruits.clear();
                    }
                    if (IsKeyPressed(KEY_R)) {
                        showTopRating = true;
                    }
                }
            }
            else {
                if (!gameOver) {
                    UpdateBonusEffects();

                    MoveRectangle(basket, false);

                    if (gameMode == 3) {
                        gameTime -= GetFrameTime();
                        if (gameTime <= 0) gameOver = true;
                    }

                    timeSinceLastSpawn += GetFrameTime();
                    if (timeSinceLastSpawn >= spawnRate) {
                        fruits.push_back(CreateFruit());
                        timeSinceLastSpawn = 0.0f;

                        if (gameMode == 2) {
                            spawnRate = 0.5f;
                            fruitSpeed = 5.0f;
                        }
                        else if (gameMode == 3) {
                            spawnRate = 0.7f;
                            fruitSpeed = 4.0f;
                        }
                    }

                    UpdateFruits(fruits, basket);

                    DrawRectangleRec(basket, ORANGE);
                    DrawText("Basket", basket.x + 20, basket.y - 25, 20, DARKBROWN);
                    DrawFruits(fruits);

                    DrawText("Score:", 10, 10, 30, DARKBLUE);
                    DrawText(TextFormat("%d", currentScore), 120, 10, 30, DARKBLUE);

                    DrawText("Lives:", 10, 50, 30, DARKBLUE);
                    DrawText(TextFormat("%d", lives), 120, 50, 30, RED);

                    DrawText("Missed:", 10, 90, 20, DARKBLUE);
                    DrawText(TextFormat("%d/%d", missedFruits, maxMissedFruits), 120, 90, 20, DARKBLUE);

                    const char* modeName = "";
                    if (gameMode == 1) modeName = "Classic Mode";
                    else if (gameMode == 2) modeName = "Survival Mode";
                    else if (gameMode == 3) modeName = "Time Attack";
                    DrawText(modeName, screenWidth / 2 - 80, 10, 25, DARKBLUE);

                    if (gameMode == 3) {
                        DrawText("Time:", screenWidth - 180, 10, 30, DARKBLUE);
                        DrawText(TextFormat("%d", (int)gameTime), screenWidth - 80, 10, 30, DARKBLUE);
                    }

                    int bonusY = 130;
                    if (slowMotionActive) {
                        DrawText("Slow Motion!", 10, bonusY, 20, BLUE);
                        bonusY += 25;
                    }
                    if (doublePointsActive) {
                        DrawText("Double Points!", 10, bonusY, 20, GOLD);
                    }

                }
                else {
                    const char* modeName = "";
                    std::string modeStr = "";

                    if (gameMode == 1) {
                        modeName = "Classic Mode";
                        modeStr = "Classic";
                    }
                    else if (gameMode == 2) {
                        modeName = "Survival Mode";
                        modeStr = "Survival";
                    }
                    else if (gameMode == 3) {
                        modeName = "Time Attack";
                        modeStr = "Time Attack";
                    }

                    SavePlayerScore(currentScore, modeStr);

                    bool newRecord = false;
                    if (gameMode == 1 && currentScore > playerScores.classicBest) newRecord = true;
                    else if (gameMode == 2 && currentScore > playerScores.survivalBest) newRecord = true;
                    else if (gameMode == 3 && currentScore > playerScores.timeAttackBest) newRecord = true;

                    DrawText("Game Over!", screenWidth / 2 - 80, 170, 40, RED);

                    if (newRecord) {
                        DrawText("NEW RECORD!", screenWidth / 2 - 80, 210, 30, GOLD);
                    }

                    DrawText(TextFormat("Final Score: %d", currentScore), screenWidth / 2 - 70, 250, 30, DARKBLUE);
                    DrawText("Press SPACE for menu", screenWidth / 2 - 100, 290, 20, DARKBLUE);
                    DrawText("Press R for My Scores", screenWidth / 2 - 100, 320, 20, DARKBLUE);

                    if (IsKeyPressed(KEY_SPACE)) {
                        inMenu = true;
                        fruits.clear();
                    }
                    if (IsKeyPressed(KEY_R)) {
                        showTopRating = true;
                    }
                }
            }
        }

        if (IsKeyPressed(KEY_ESCAPE) && !inMenu && !showTopRating) {
            inMenu = true;
            fruits.clear();
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}