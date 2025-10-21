#include "raylib.h"
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>

const int screenWidth = 800;
const int screenHeight = 600;

// Структура для меню
struct Button {
    Rectangle rect; 
    std::string text; 
};
// Структура для fruits (хорошие, плохие, бонусные) 
struct Fruit {
    Rectangle rect;
    int type; // 0-3: хорошие, 4-5: плохие, 6-9: бонусные
    float speed;
    bool active;
    Color color;
    float effectDuration;
};

// Структура для score 
struct HighScore {
    std::string name;
    int score;
    std::string mode;
};

// Global
int currentScore = 0;
int gameMode = 0; // 0: menu, 1: classic, 2: survival, 3: time attack
float gameTime = 120.0f;
int lives = 3;
int missedFruits = 0;
int maxMissedFruits = 10;
bool gameOver = false;
bool inMenu = true;

float fruitSpeed = 3.0f;
float spawnRate = 1.0f;
float timeSinceLastSpawn = 0.0f;

// Бонусные эффекты
float slowMotionTimer = 0.0f;
float doublePointsTimer = 0.0f;
float freezeTimer = 0.0f;
float speedBoostTimer = 0.0f;
bool slowMotionActive = false;
bool doublePointsActive = false;
bool freezeActive = false;
bool speedBoostActive = false;

void MoveRectangle(Rectangle& rec, bool useArrowKeys) {
    float moveSpeed = 5.0f;

    // Увеличенная скорость при бонусе скорости
    if (speedBoostActive) {
        moveSpeed = 8.0f;
    }
    // Замедление при замедлении времени
    if (slowMotionActive) {
        moveSpeed = 3.0f;
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

Fruit CreateFruit() {
    Fruit fruit;
    fruit.rect = { (float)GetRandomValue(50, screenWidth - 100), -50, 40, 40 };

    // Определяем тип фрукта с учетом режима игры
    int randomChance = GetRandomValue(0, 100);

    // Редкий шанс появления бонусного фрукта (8%)
    if (randomChance < 8) {
        // Разные бонусы для разных режимов
        if (gameMode == 2) { // Survival - более полезные бонусы
            fruit.type = GetRandomValue(6, 8); // Slow motion, Double points, Extra life
        }
        else if (gameMode == 3) { // Time Attack - бонусы времени
            fruit.type = GetRandomValue(9, 10); // Time bonus, Speed boost
        }
        else { // Classic - случайные бонусы
            fruit.type = GetRandomValue(6, 10);
        }
    }
    else if (randomChance < 35) { // 27% шанс для плохих фруктов
        fruit.type = GetRandomValue(4, 5);
    }
    else { // 65% шанс для хороших фруктов
        fruit.type = GetRandomValue(0, 3);
    }

    fruit.active = true;

    // Базовая скорость с учетом активных эффектов
    float baseSpeed = fruitSpeed + GetRandomValue(0, 2);
    if (slowMotionActive) baseSpeed *= 0.4f;
    if (speedBoostActive) baseSpeed *= 1.3f;
    if (freezeActive) baseSpeed = 0.5f;

    fruit.speed = baseSpeed;

    // Устанавливаем цвета и длительность эффектов
    switch (fruit.type) {
    case 0: fruit.color = RED; break;      // Яблоко
    case 1: fruit.color = YELLOW; break;   // Банан
    case 2: fruit.color = GREEN; break;    // Арбуз
    case 3: fruit.color = PURPLE; break;   // Виноград
    case 4: fruit.color = BROWN; break;    // Гнилой фрукт
    case 5: fruit.color = GRAY; break;     // Камень
    case 6: fruit.color = BLUE; break;     // Замедление времени
    case 7: fruit.color = GOLD; break;     // Двойные очки
    case 8: fruit.color = PINK; break;     // Дополнительная жизнь
    case 9: fruit.color = MAGENTA; break;  // Бонус времени
    case 10: fruit.color = ORANGE; break;  // Ускорение
    }

    fruit.effectDuration = 5.0f; // 5 секунд для бонусных эффектов

    return fruit;
}

void UpdateBonusEffects() {
    // Обновляем таймеры бонусов
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

    if (freezeActive) {
        freezeTimer -= GetFrameTime();
        if (freezeTimer <= 0) {
            freezeActive = false;
        }
    }

    if (speedBoostActive) {
        speedBoostTimer -= GetFrameTime();
        if (speedBoostTimer <= 0) {
            speedBoostActive = false;
        }
    }
}

void ApplyBonusEffect(int bonusType) {
    switch (bonusType) {
    case 6: // Замедление времени (особенно полезно в Survival)
        slowMotionActive = true;
        slowMotionTimer = 7.0f; // 7 секунд
        break;

    case 7: // Двойные очки
        doublePointsActive = true;
        doublePointsTimer = 10.0f; // 10 секунд
        break;

    case 8: // Дополнительная жизнь
        if (lives < 5) { // Максимум 5 жизней
            lives++;
        }
        break;

    case 9: // Бонус времени (особенно полезно в Time Attack)
        gameTime += 10.0f; // +10 секунд
        break;

    case 10: // Ускорение (полезно во всех режимах)
        speedBoostActive = true;
        speedBoostTimer = 6.0f; // 6 секунд
        break;
    }
}

void UpdateFruits(std::vector<Fruit>& fruits, Rectangle& basket) {
    for (auto& fruit : fruits) {
        if (fruit.active) {
            // Применяем активные эффекты к скорости падения
            float currentSpeed = fruit.speed;
            if (freezeActive) {
                currentSpeed = 0.5f; // Почти остановка
            }

            fruit.rect.y += currentSpeed;

            if (CheckCollisionRecs(fruit.rect, basket)) {
                if (fruit.type <= 3) { // Хорошие фрукты
                    int points = 100;
                    if (doublePointsActive) points *= 2;
                    currentScore += points;
                }
                else if (fruit.type <= 5) { // Плохие предметы
                    lives--;
                    if (lives <= 0) gameOver = true;
                }
                else { // Бонусные фрукты
                    ApplyBonusEffect(fruit.type);
                    currentScore += 200; // Дополнительные очки за бонус
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

    // Remove inactive fruits
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

            // Специальные узоры для бонусных фруктов
            if (fruit.type >= 6) {
                // Рисуем звездочку для бонусных фруктов
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

    // Сбрасываем все бонусные эффекты
    slowMotionActive = false;
    doublePointsActive = false;
    freezeActive = false;
    speedBoostActive = false;
    slowMotionTimer = 0.0f;
    doublePointsTimer = 0.0f;
    freezeTimer = 0.0f;
    speedBoostTimer = 0.0f;
}

void DrawMenu() {
    ClearBackground(SKYBLUE);

    DrawText("FruityDrop", screenWidth / 2 - 100, 100, 40, DARKBLUE);

    // Menu buttons
    Rectangle classicBtn = { screenWidth / 2 - 100, 200, 200, 50 };
    Rectangle survivalBtn = { screenWidth / 2 - 100, 270, 200, 50 };
    Rectangle timeBtn = { screenWidth / 2 - 100, 340, 200, 50 };
    Rectangle exitBtn = { screenWidth / 2 - 100, 410, 200, 50 };

    DrawRectangleRec(classicBtn, GREEN);
    DrawRectangleRec(survivalBtn, BLUE);
    DrawRectangleRec(timeBtn, ORANGE);
    DrawRectangleRec(exitBtn, RED);

    // English text 
    DrawText("Classic", classicBtn.x + 60, classicBtn.y + 15, 20, WHITE);
    DrawText("Survival", survivalBtn.x + 50, survivalBtn.y + 15, 20, WHITE);
    DrawText("Time Attack", timeBtn.x + 35, timeBtn.y + 15, 20, WHITE);
    DrawText("Exit", exitBtn.x + 75, exitBtn.y + 15, 20, WHITE);

    // Handle clicks 
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
        if (CheckCollisionPointRec(mousePoint, exitBtn)) {
            gameMode = 4;
        }
    }
}

int main(void) {
    InitWindow(screenWidth, screenHeight, "FruityDrop");
    SetTargetFPS(60);

    Rectangle basket = { screenWidth / 2 - 50, screenHeight - 100, 100, 60 };
    std::vector<Fruit> fruits;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (gameMode == 4) break;
        if (inMenu) {
            DrawMenu();
        }
        else {
            if (!gameOver) {
                // Обновляем бонусные эффекты
                UpdateBonusEffects();

                // Controls
                MoveRectangle(basket, false);

                // Timer for time attack mode
                if (gameMode == 3) {
                    gameTime -= GetFrameTime();
                    if (gameTime <= 0) gameOver = true;
                }

                // Spawn fruits
                timeSinceLastSpawn += GetFrameTime();
                if (timeSinceLastSpawn >= spawnRate) {
                    fruits.push_back(CreateFruit());
                    timeSinceLastSpawn = 0.0f;

                    // Difficulty settings
                    if (gameMode == 2) {
                        spawnRate = 0.5f;
                        fruitSpeed = 5.0f;
                    }
                    else if (gameMode == 3) {
                        spawnRate = 0.7f;
                        fruitSpeed = 4.0f;
                    }
                }

                // Update fruits
                UpdateFruits(fruits, basket);

                // Drawing
                DrawRectangleRec(basket, ORANGE);
                DrawText("Basket", basket.x + 20, basket.y - 25, 20, DARKBROWN);
                DrawFruits(fruits);

                // UI 
                DrawText("Score:", 10, 10, 30, DARKBLUE);
                DrawText(TextFormat("%d", currentScore), 120, 10, 30, DARKBLUE);

                DrawText("Lives:", 10, 50, 30, DARKBLUE);
                DrawText(TextFormat("%d", lives), 120, 50, 30, RED);

                DrawText("Missed:", 10, 90, 20, DARKBLUE);
                DrawText(TextFormat("%d/%d", missedFruits, maxMissedFruits), 120, 90, 20, DARKBLUE);

                // Game mode name
                const char* modeName = "";
                if (gameMode == 1) modeName = "Classic Mode";
                else if (gameMode == 2) modeName = "Survival Mode";
                else if (gameMode == 3) modeName = "Time Attack";
                DrawText(modeName, screenWidth / 2 - 80, 10, 25, DARKBLUE);

                if (gameMode == 3) {
                    DrawText("Time:", screenWidth - 180, 10, 30, DARKBLUE);
                    DrawText(TextFormat("%d", (int)gameTime), screenWidth - 80, 10, 30, DARKBLUE);
                }

                // Отображение активных бонусов
                int bonusY = 130;
                if (slowMotionActive) {
                    DrawText("Slow Motion!", 10, bonusY, 20, BLUE);
                    bonusY += 25;
                }
                if (doublePointsActive) {
                    DrawText("Double Points!", 10, bonusY, 20, GOLD);
                    bonusY += 25;
                }
                if (speedBoostActive) {
                    DrawText("Speed Boost!", 10, bonusY, 20, ORANGE);
                    bonusY += 25;
                }
                if (freezeActive) {
                    DrawText("Freeze!", 10, bonusY, 20, SKYBLUE);
                    bonusY += 25;
                }

                // Fruit legend с бонусными фруктами
                DrawText("Good fruits:", 10, screenHeight - 150, 20, DARKGREEN);
                DrawRectangle(130, screenHeight - 150, 20, 20, RED);
                DrawRectangle(160, screenHeight - 150, 20, 20, YELLOW);
                DrawRectangle(190, screenHeight - 150, 20, 20, GREEN);
                DrawRectangle(220, screenHeight - 150, 20, 20, PURPLE);

                DrawText("Bad items:", 10, screenHeight - 120, 20, DARKGREEN);
                DrawRectangle(130, screenHeight - 120, 20, 20, BROWN);
                DrawRectangle(160, screenHeight - 120, 20, 20, GRAY);

                DrawText("Bonus fruits:", 10, screenHeight - 90, 20, DARKGREEN);
                DrawRectangle(130, screenHeight - 90, 20, 20, BLUE);
                DrawRectangle(160, screenHeight - 90, 20, 20, GOLD);
                DrawRectangle(190, screenHeight - 90, 20, 20, PINK);
                DrawRectangle(220, screenHeight - 90, 20, 20, MAGENTA);
                DrawRectangle(250, screenHeight - 90, 20, 20, ORANGE);

            }
            else {
                // Game over screen 
                DrawText("Game Over!", screenWidth / 2 - 80, 200, 40, RED);
                DrawText(TextFormat("Final Score: %d", currentScore), screenWidth / 2 - 70, 250, 30, DARKBLUE);
                DrawText("Press SPACE for menu", screenWidth / 2 - 100, 300, 20, DARKBLUE);

                if (IsKeyPressed(KEY_SPACE)) {
                    inMenu = true;
                    fruits.clear();
                }
            }
        }

        // Return to menu
        if (IsKeyPressed(KEY_ESCAPE) && !inMenu) {
            inMenu = true;
            fruits.clear();
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}