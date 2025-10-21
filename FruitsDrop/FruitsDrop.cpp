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
// Структура для fruits (хорошие, плохие) 
struct Fruit {
    Rectangle rect; 
    int type; 
    float speed; 
    bool active; 
    Color color; 
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
int lives = 1;
int missedFruits = 0;
int maxMissedFruits = 1;
bool gameOver = false;
bool inMenu = true;

float fruitSpeed = 3.0f;
float spawnRate = 1.0f;
float timeSinceLastSpawn = 0.0f;

void MoveRectangle(Rectangle& rec, bool useArrowKeys) {
    if ((useArrowKeys && IsKeyDown(KEY_LEFT)) || (!useArrowKeys && IsKeyDown(KEY_A))) {
        rec.x -= 5;
    }
    if ((useArrowKeys && IsKeyDown(KEY_RIGHT)) || (!useArrowKeys && IsKeyDown(KEY_D))) {
        rec.x += 5;
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
    fruit.type = GetRandomValue(0, 5);
    fruit.active = true;
    fruit.speed = fruitSpeed + GetRandomValue(0, 2);
    return fruit;
}

void UpdateFruits(std::vector<Fruit>& fruits, Rectangle& basket) {
    for (auto& fruit : fruits) {
        if (fruit.active) {
            fruit.rect.y += fruit.speed;

            if (CheckCollisionRecs(fruit.rect, basket)) {
                if (fruit.type <= 3) {
                    currentScore += 100;
                }
                else {
                    lives--;
                    if (lives <= 0) gameOver = true;
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
            if (fruit.type <= 3) {
                // Good fruits - bright colors
                DrawRectangleRec(fruit.rect, fruit.type == 0 ? RED :
                    fruit.type == 1 ? YELLOW :
                    fruit.type == 2 ? GREEN : PURPLE);
            }
            else {
                // Bad items - dark colors
                DrawRectangleRec(fruit.rect, fruit.type == 4 ? BROWN : GRAY);
            }
        }
    }
}

void ResetGame() {
    currentScore = 0;
    lives = 1;
    missedFruits = 0;
    gameOver = false;
    gameTime = 120.0f;
    fruitSpeed = 3.0f;
    spawnRate = 1.0f;
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

                // Fruit legend 
                DrawText("Good fruits:", 10, screenHeight - 120, 20, DARKGREEN);
                DrawRectangle(130, screenHeight - 120, 20, 20, RED);
                DrawRectangle(160, screenHeight - 120, 20, 20, YELLOW);
                DrawRectangle(190, screenHeight - 120, 20, 20, GREEN);
                DrawRectangle(220, screenHeight - 120, 20, 20, PURPLE);

                DrawText("Bad items:", 10, screenHeight - 90, 20, DARKGREEN);
                DrawRectangle(130, screenHeight - 90, 20, 20, BROWN);
                DrawRectangle(160, screenHeight - 90, 20, 20, GRAY);

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