#include "raylib.h"
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

const int screenWidth = 800;
const int screenHeight = 600;
int fruitSize;
Texture2D Fruit0Texture;
Texture2D Fruit1Texture;
Texture2D Fruit2Texture;
Texture2D Fruit3Texture;
Texture2D Fruit4Texture;
Texture2D Fruit5Texture;
Texture2D Fruit6Texture;
Texture2D Fruit7Texture;
Texture2D Fruit8Texture;
Texture2D Fruit9Texture;
Texture2D Fruit10Texture;

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

// Структура для рекордов
struct PlayerBestScores {
    int classicBest = 0;
    int survivalBest = 0;
    int timeAttackBest = 0;
    int twoPlayerBestP1 = 0; // Рекорд Player 1 в Two Players
    int twoPlayerBestP2 = 0; // Рекорд Player 2 в Two Players
};

// Структура для бонусных эффектов игрока
struct PlayerBonusEffects {
    float slowMotionTimer = 0.0f;
    float doublePointsTimer = 0.0f;
    float freezeTimer = 0.0f;
    float speedBoostTimer = 0.0f;
    bool slowMotionActive = false;
    bool doublePointsActive = false;
    bool freezeActive = false;
    bool speedBoostActive = false;
};

// Структура для игрока
struct Player {
    Rectangle basket;
    int score;
    int lives;
    int missedFruits;
    std::string name;
    Color color;
    PlayerBonusEffects bonuses;
    bool isAlive;
};

// Структура для геймпада
struct GamepadController {
    int gamepadNumber;
    bool connected;
    bool useLeftStick; // true - левый стик, false - правый стик
};

// Global
int gameMode = 4; // 0: menu, 1: classic, 2: survival, 3: time attack, 4: two players, 5: top rating, 6: exit
float gameTime = 120.0f;
bool gameOver = false;
bool inMenu = false; // Начинаем сразу с игры
bool showTopRating = false;
bool gameStarted = false; // Флаг начала игры
bool fullscreen = true; // Флаг полноэкранного режима

float fruitSpeed = 2.0f;
float spawnRate = 1.0f;
float timeSinceLastSpawn = 0.0f;

// Глобальные бонусные эффекты (только для одиночных режимов)
float slowMotionTimer = 0.0f;
float doublePointsTimer = 0.0f;
float freezeTimer = 0.0f;
float speedBoostTimer = 0.0f;
bool slowMotionActive = false;
bool doublePointsActive = false;
bool freezeActive = false;
bool speedBoostActive = false;

// Для рекордов
PlayerBestScores playerScores;

// Для двух игроков
Player player1, player2;

// Для геймпадов
GamepadController gamepad1 = { 0, false, true }; // Геймпад 1 (игрок 1)
GamepadController gamepad2 = { 1, false, true }; // Геймпад 2 (игрок 2)

// Функции для работы с геймпадами
void CheckGamepads() {
    // Проверяем подключение геймпадов
    gamepad1.connected = IsGamepadAvailable(gamepad1.gamepadNumber);
    gamepad2.connected = IsGamepadAvailable(gamepad2.gamepadNumber);

    if (gamepad1.connected) {
        // Проверяем кнопки для переключения стиков
        if (IsGamepadButtonPressed(gamepad1.gamepadNumber, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
            gamepad1.useLeftStick = !gamepad1.useLeftStick;
        }
    }

    if (gamepad2.connected) {
        if (IsGamepadButtonPressed(gamepad2.gamepadNumber, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
            gamepad2.useLeftStick = !gamepad2.useLeftStick;
        }
    }
}

void MoveRectangleWithGamepad(Rectangle& rec, GamepadController& gamepad, int playerSide = 0) {
    if (!gamepad.connected) return;

    float moveSpeed = 20.0f;
    Vector2 stickAxis = { 0, 0 };

    // Получаем данные со стика в зависимости от настройки
    if (gamepad.useLeftStick) {
        stickAxis.x = GetGamepadAxisMovement(gamepad.gamepadNumber, GAMEPAD_AXIS_LEFT_X);
    }
    else {
        stickAxis.x = GetGamepadAxisMovement(gamepad.gamepadNumber, GAMEPAD_AXIS_RIGHT_X);
    }

    // Движение с учетом мертвой зоны
    if (fabs(stickAxis.x) > 0.2f) {
        rec.x += stickAxis.x * moveSpeed;
    }

    // Также поддерживаем кнопки D-pad для точного управления
    if (IsGamepadButtonDown(gamepad.gamepadNumber, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
        rec.x -= moveSpeed;
    }
    if (IsGamepadButtonDown(gamepad.gamepadNumber, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
        rec.x += moveSpeed;
    }

    // Ограничения для разделенного экрана в режиме двух игроков
    if (gameMode == 4) {
        if (playerSide == 1) { // Player 1 - левая сторона
            if (rec.x < 0) rec.x = 0;
            if (rec.x + rec.width > GetScreenWidth() / 2) rec.x = GetScreenWidth() / 2 - rec.width;
        }
        else if (playerSide == 2) { // Player 2 - правая сторона
            if (rec.x < GetScreenWidth() / 2) rec.x = GetScreenWidth() / 2;
            if (rec.x + rec.width > GetScreenWidth()) rec.x = GetScreenWidth() - rec.width;
        }
    }
    else {
        // Обычные ограничения для одиночных режимов
        if (rec.x + rec.width > GetScreenWidth()) rec.x = GetScreenWidth() - rec.width;
        if (rec.x < 0) rec.x = 0;
    }
}

void DrawGamepadInfo() {
    if (gameMode == 4) {
        int infoFontSize = 18 * GetScreenWidth() / 1920;

        // Информация о геймпаде игрока 1
        if (gamepad1.connected) {
            DrawText("Gamepad 1: Connected", 20, GetScreenHeight() - 60, infoFontSize, GREEN);
            DrawText(TextFormat("Stick: %s", gamepad1.useLeftStick ? "LEFT" : "RIGHT"), 20, GetScreenHeight() - 40, infoFontSize - 2, WHITE);
            DrawText("Press X to switch", 20, GetScreenHeight() - 20, infoFontSize - 2, YELLOW);
        }
        else {
            DrawText("Gamepad 1: Not Connected", 40, GetScreenHeight() - 60, infoFontSize, RED);
            DrawText("Use A/D keys", 20, GetScreenHeight() - 20, infoFontSize - 2, WHITE);
        }

        // Информация о геймпаде игрока 2
        if (gamepad2.connected) {
            DrawText("Gamepad 2: Connected", GetScreenWidth() - 200, GetScreenHeight() - 60, infoFontSize, GREEN);
            DrawText(TextFormat("Stick: %s", gamepad2.useLeftStick ? "LEFT" : "RIGHT"), GetScreenWidth() - 200, GetScreenHeight() - 40, infoFontSize - 2, WHITE);
            DrawText("Press X to switch", GetScreenWidth() - 200, GetScreenHeight() - 20, infoFontSize - 2, YELLOW);
        }
        else {
            DrawText("Gamepad 2: Not Connected", GetScreenWidth() - 260, GetScreenHeight() - 60, infoFontSize, RED);
            DrawText("Use Arrow keys", GetScreenWidth() - 200, GetScreenHeight() - 20, infoFontSize - 2, WHITE);
        }
    }
}

// Для работы с рекордами
void LoadPlayerScores() {
    playerScores.classicBest = 0;
    playerScores.survivalBest = 0;
    playerScores.timeAttackBest = 0;
    playerScores.twoPlayerBestP1 = 0;
    playerScores.twoPlayerBestP2 = 0;
}

void SavePlayerScore(int score, const std::string& mode, int playerNumber = 0) {
    if (mode == "Classic" && score > playerScores.classicBest) {
        playerScores.classicBest = score;
    }
    else if (mode == "Survival" && score > playerScores.survivalBest) {
        playerScores.survivalBest = score;
    }
    else if (mode == "Time Attack" && score > playerScores.timeAttackBest) {
        playerScores.timeAttackBest = score;
    }
    else if (mode == "Two Players") {
        if (playerNumber == 1 && score > playerScores.twoPlayerBestP1) {
            playerScores.twoPlayerBestP1 = score;
        }
        else if (playerNumber == 2 && score > playerScores.twoPlayerBestP2) {
            playerScores.twoPlayerBestP2 = score;
        }
    }
}

void DrawTopRating() {
    ClearBackground(SKYBLUE);

    DrawText("MY BEST SCORES", GetScreenWidth() / 2 - 150, 50, 40, DARKBLUE);

    // Лучшие результаты по режимам
    int startY = 150;

    // Classic Mode
    DrawText("CLASSIC MODE", GetScreenWidth() / 2 - 100, startY, 30, GREEN);
    DrawText("Best Score:", GetScreenWidth() / 2 - 150, startY + 40, 25, WHITE);
    DrawText(TextFormat("%d", playerScores.classicBest), GetScreenWidth() / 2 + 50, startY + 40, 25, GOLD);

    // Survival Mode
    DrawText("SURVIVAL MODE", GetScreenWidth() / 2 - 100, startY + 100, 30, BLUE);
    DrawText("Best Score:", GetScreenWidth() / 2 - 150, startY + 140, 25, WHITE);
    DrawText(TextFormat("%d", playerScores.survivalBest), GetScreenWidth() / 2 + 50, startY + 140, 25, GOLD);

    // Time Attack Mode
    DrawText("TIME ATTACK MODE", GetScreenWidth() / 2 - 120, startY + 200, 30, ORANGE);
    DrawText("Best Score:", GetScreenWidth() / 2 - 150, startY + 240, 25, WHITE);
    DrawText(TextFormat("%d", playerScores.timeAttackBest), GetScreenWidth() / 2 + 50, startY + 240, 25, GOLD);

    // Two Players Mode - разделенные рекорды
    DrawText("TWO PLAYERS MODE", GetScreenWidth() / 2 - 120, startY + 280, 30, RED);

    // Player 1 record
    DrawText("Player 1 Best:", GetScreenWidth() / 2 - 150, startY + 320, 25, BLUE);
    DrawText(TextFormat("%d", playerScores.twoPlayerBestP1), GetScreenWidth() / 2 + 50, startY + 320, 25, GOLD);

    // Player 2 record
    DrawText("Player 2 Best:", GetScreenWidth() / 2 - 150, startY + 360, 25, RED);
    DrawText(TextFormat("%d", playerScores.twoPlayerBestP2), GetScreenWidth() / 2 + 50, startY + 360, 25, GOLD);

    // Статистика
    DrawText("STATISTICS", GetScreenWidth() / 2 - 70, startY + 420, 25, PURPLE);

    int totalBest = playerScores.classicBest + playerScores.survivalBest + playerScores.timeAttackBest +
        (playerScores.twoPlayerBestP1 > playerScores.twoPlayerBestP2 ? playerScores.twoPlayerBestP1 : playerScores.twoPlayerBestP2);

    DrawText(TextFormat("Total Best: %d", totalBest), GetScreenWidth() / 2 - 80, startY + 460, 20, WHITE);

    // Определяем любимый режим
    std::string favoriteMode = "Classic";
    int maxScore = playerScores.classicBest;

    if (playerScores.survivalBest > maxScore) {
        maxScore = playerScores.survivalBest;
        favoriteMode = "Survival";
    }
    if (playerScores.timeAttackBest > maxScore) {
        maxScore = playerScores.timeAttackBest;
        favoriteMode = "Time Attack";
    }
    int twoPlayerMax = playerScores.twoPlayerBestP1 > playerScores.twoPlayerBestP2 ? playerScores.twoPlayerBestP1 : playerScores.twoPlayerBestP2;
    if (twoPlayerMax > maxScore) {
        maxScore = twoPlayerMax;
        favoriteMode = "Two Players";
    }

    DrawText(TextFormat("Favorite Mode: %s", favoriteMode.c_str()), GetScreenWidth() / 2 - 100, startY + 490, 20, WHITE);

    DrawText("Press ESC to return", GetScreenWidth() / 2 - 100, GetScreenHeight() - 50, 20, WHITE);
}

void MoveRectangle(Rectangle& rec, bool useArrowKeys, int playerSide = 0) {
    float moveSpeed = 20.0f;

    // Увеличенная скорость при бонусе скорости
    if (gameMode == 4) {
        // В режиме двух игроков используем индивидуальные бонусы
        if (playerSide == 1 && player1.bonuses.speedBoostActive) {
            moveSpeed = 25.0f;
        }
        else if (playerSide == 2 && player2.bonuses.speedBoostActive) {
            moveSpeed = 25.0f;
        }
    }
    else {
        // В одиночных режимах используем глобальные бонусы
        if (speedBoostActive) {
            moveSpeed = 25.0f;
        }
    }

    // Замедление при замедлении времени
    if (gameMode == 4) {
        if (playerSide == 1 && player1.bonuses.slowMotionActive) {
            moveSpeed = 20.0f;
        }
        else if (playerSide == 2 && player2.bonuses.slowMotionActive) {
            moveSpeed = 20.0f;
        }
    }
    else {
        if (slowMotionActive) {
            moveSpeed = 20.0f;
        }
    }

    // Управление с клавиатуры (как резерв)
    if ((useArrowKeys && IsKeyDown(KEY_LEFT)) || (!useArrowKeys && IsKeyDown(KEY_A))) {
        rec.x -= moveSpeed;
    }
    if ((useArrowKeys && IsKeyDown(KEY_RIGHT)) || (!useArrowKeys && IsKeyDown(KEY_D))) {
        rec.x += moveSpeed;
    }

    // Ограничения для разделенного экрана в режиме двух игроков
    if (gameMode == 4) {
        if (playerSide == 1) { // Player 1 - левая сторона
            if (rec.x < 0) rec.x = 0;
            if (rec.x + rec.width > GetScreenWidth() / 2) rec.x = GetScreenWidth() / 2 - rec.width;
        }
        else if (playerSide == 2) { // Player 2 - правая сторона
            if (rec.x < GetScreenWidth() / 2) rec.x = GetScreenWidth() / 2;
            if (rec.x + rec.width > GetScreenWidth()) rec.x = GetScreenWidth() - rec.width;
        }
    }
    else {
        // Обычные ограничения для одиночных режимов
        if (rec.x + rec.width > GetScreenWidth()) rec.x = GetScreenWidth() - rec.width;
        if (rec.x < 0) rec.x = 0;
    }
}

Fruit CreateFruit(int playerSide = 0) {
    Fruit fruit;

    if (playerSide == 1) { // Для игрока 1 (левая сторона)
        fruit.rect = { (float)GetRandomValue(50, GetScreenWidth() / 2 - 100), -50, (float)fruitSize, (float)fruitSize };
    }
    else if (playerSide == 2) { // Для игрока 2 (правая сторона)
        fruit.rect = { (float)GetRandomValue(GetScreenWidth() / 2 + 50, GetScreenWidth() - 100), -50, (float)fruitSize, (float)fruitSize };
    }
    else { // Для одиночных режимов
        fruit.rect = { (float)GetRandomValue(50, GetScreenWidth() - 100), -50, (float)fruitSize, (float)fruitSize };
    }

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
        else { // Classic и Two Players - случайные бонусы
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

    // В режиме двух игроков учитываем индивидуальные эффекты заморозки
    if (gameMode == 4) {
        if (playerSide == 1 && player1.bonuses.freezeActive) baseSpeed = 0.5f;
        else if (playerSide == 2 && player2.bonuses.freezeActive) baseSpeed = 0.5f;
        else {
            if (player1.bonuses.slowMotionActive || player2.bonuses.slowMotionActive) baseSpeed *= 0.4f;
            if (player1.bonuses.speedBoostActive || player2.bonuses.speedBoostActive) baseSpeed *= 1.3f;
        }
    }
    else {
        // В одиночных режимах используем глобальные эффекты
        if (slowMotionActive) baseSpeed *= 0.4f;
        if (speedBoostActive) baseSpeed *= 1.3f;
        if (freezeActive) baseSpeed = 0.5f;
    }

    fruit.speed = baseSpeed * GetScreenHeight() / 1080.0f; // Масштабируем скорость под разрешение

    // Цвета и длительность эффектов
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
    case 9: fruit.color = MAGENTA; break;
    case 10: fruit.color = ORANGE; break;
    }

    fruit.effectDuration = 5.0f; // 5 секунд для бонусных эффектов

    return fruit;
}

void UpdateBonusEffects() {
    // Обновляем таймеры бонусов для одиночных режимов
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

    // Обновляем таймеры бонусов для игроков в режиме двух игроков
    if (gameMode == 4) {
        // Player 1
        if (player1.isAlive) {
            if (player1.bonuses.slowMotionActive) {
                player1.bonuses.slowMotionTimer -= GetFrameTime();
                if (player1.bonuses.slowMotionTimer <= 0) {
                    player1.bonuses.slowMotionActive = false;
                }
            }
            if (player1.bonuses.doublePointsActive) {
                player1.bonuses.doublePointsTimer -= GetFrameTime();
                if (player1.bonuses.doublePointsTimer <= 0) {
                    player1.bonuses.doublePointsActive = false;
                }
            }
            if (player1.bonuses.freezeActive) {
                player1.bonuses.freezeTimer -= GetFrameTime();
                if (player1.bonuses.freezeTimer <= 0) {
                    player1.bonuses.freezeActive = false;
                }
            }
            if (player1.bonuses.speedBoostActive) {
                player1.bonuses.speedBoostTimer -= GetFrameTime();
                if (player1.bonuses.speedBoostTimer <= 0) {
                    player1.bonuses.speedBoostActive = false;
                }
            }
        }

        // Player 2
        if (player2.isAlive) {
            if (player2.bonuses.slowMotionActive) {
                player2.bonuses.slowMotionTimer -= GetFrameTime();
                if (player2.bonuses.slowMotionTimer <= 0) {
                    player2.bonuses.slowMotionActive = false;
                }
            }
            if (player2.bonuses.doublePointsActive) {
                player2.bonuses.doublePointsTimer -= GetFrameTime();
                if (player2.bonuses.doublePointsTimer <= 0) {
                    player2.bonuses.doublePointsActive = false;
                }
            }
            if (player2.bonuses.freezeActive) {
                player2.bonuses.freezeTimer -= GetFrameTime();
                if (player2.bonuses.freezeTimer <= 0) {
                    player2.bonuses.freezeActive = false;
                }
            }
            if (player2.bonuses.speedBoostActive) {
                player2.bonuses.speedBoostTimer -= GetFrameTime();
                if (player2.bonuses.speedBoostTimer <= 0) {
                    player2.bonuses.speedBoostActive = false;
                }
            }
        }
    }
}

void ApplyBonusEffect(int bonusType, int playerNumber = 0) {
    if (gameMode == 4) {
        // В режиме двух игроков применяем бонусы индивидуально
        if (playerNumber == 1 && player1.isAlive) {
            switch (bonusType) {
            case 6: // Замедление времени
                player1.bonuses.slowMotionActive = true;
                player1.bonuses.slowMotionTimer = 7.0f;
                break;
            case 7: // Двойные очки
                player1.bonuses.doublePointsActive = true;
                player1.bonuses.doublePointsTimer = 10.0f;
                break;
            case 8: // Дополнительная жизнь
                if (player1.lives < 5) player1.lives++;
                break;
            case 9: // Бонус времени
                gameTime += 10.0f;
                break;
            case 10: // Ускорение
                player1.bonuses.speedBoostActive = true;
                player1.bonuses.speedBoostTimer = 6.0f;
                break;
            }
        }
        else if (playerNumber == 2 && player2.isAlive) {
            switch (bonusType) {
            case 6: // Замедление времени
                player2.bonuses.slowMotionActive = true;
                player2.bonuses.slowMotionTimer = 7.0f;
                break;
            case 7: // Двойные очки
                player2.bonuses.doublePointsActive = true;
                player2.bonuses.doublePointsTimer = 10.0f;
                break;
            case 8: // Дополнительная жизнь
                if (player2.lives < 5) player2.lives++;
                break;
            case 9: // Бонус времени
                gameTime += 10.0f;
                break;
            case 10: // Ускорение
                player2.bonuses.speedBoostActive = true;
                player2.bonuses.speedBoostTimer = 6.0f;
                break;
            }
        }
    }
    else {
        // В одиночных режимах используем глобальные бонусы
        switch (bonusType) {
        case 6: // Замедление времени
            slowMotionActive = true;
            slowMotionTimer = 7.0f;
            break;
        case 7: // Двойные очки
            doublePointsActive = true;
            doublePointsTimer = 10.0f;
            break;
        case 8: // Дополнительная жизнь
            if (player1.lives < 5) player1.lives++;
            break;
        case 9: // Бонус времени
            gameTime += 10.0f;
            break;
        case 10: // Ускорение
            speedBoostActive = true;
            speedBoostTimer = 6.0f;
            break;
        }
    }
}

void UpdateFruits(std::vector<Fruit>& fruits) {
    for (auto& fruit : fruits) {
        if (fruit.active) {
            // Применяем активные эффекты к скорости падения
            float currentSpeed = fruit.speed;

            if (gameMode == 4) {
                // В режиме двух игроков учитываем индивидуальные эффекты заморозки
                if (fruit.rect.x < GetScreenWidth() / 2 && player1.bonuses.freezeActive) {
                    currentSpeed = 0.5f;
                }
                else if (fruit.rect.x >= GetScreenWidth() / 2 && player2.bonuses.freezeActive) {
                    currentSpeed = 0.5f;
                }
            }
            else {
                // В одиночных режимах используем глобальные эффекты
                if (freezeActive) {
                    currentSpeed = 0.5f;
                }
            }

            fruit.rect.y += currentSpeed;

            // Проверяем столкновение с корзинами игроков
            bool collision = false;

            if (gameMode == 4) { // Режим двух игроков
                if (player1.isAlive && CheckCollisionRecs(fruit.rect, player1.basket)) {
                    collision = true;
                    if (fruit.type <= 3) { // Хорошие фрукты
                        int points = 100;
                        if (player1.bonuses.doublePointsActive) points *= 2;
                        player1.score += points;
                    }
                    else if (fruit.type <= 5) { // Плохие предметы
                        player1.lives--;
                        if (player1.lives <= 0) {
                            player1.isAlive = false;
                        }
                    }
                    else { // Бонусные фрукты
                        ApplyBonusEffect(fruit.type, 1);
                        player1.score += 200;
                    }
                }
                else if (player2.isAlive && CheckCollisionRecs(fruit.rect, player2.basket)) {
                    collision = true;
                    if (fruit.type <= 3) { // Хорошие фрукты
                        int points = 100;
                        if (player2.bonuses.doublePointsActive) points *= 2;
                        player2.score += points;
                    }
                    else if (fruit.type <= 5) { // Плохие предметы
                        player2.lives--;
                        if (player2.lives <= 0) {
                            player2.isAlive = false;
                        }
                    }
                    else { // Бонусные фрукты
                        ApplyBonusEffect(fruit.type, 2);
                        player2.score += 200;
                    }
                }
            }
            else { // Одиночные режимы
                if (CheckCollisionRecs(fruit.rect, player1.basket)) {
                    collision = true;
                    if (fruit.type <= 3) { // Хорошие фрукты
                        int points = 100;
                        if (doublePointsActive) points *= 2;
                        player1.score += points;
                    }
                    else if (fruit.type <= 5) { // Плохие предметы
                        player1.lives--;
                    }
                    else { // Бонусные фрукты
                        ApplyBonusEffect(fruit.type);
                        player1.score += 200;
                    }
                }
            }

            if (collision) {
                fruit.active = false;
            }

            // Проверка пропущенных фруктов
            if (fruit.rect.y > GetScreenHeight()) {
                if (fruit.type <= 3) { // Только хорошие фрукты считаются как пропущенные
                    if (gameMode == 4) {
                        // Определяем, какой игрок пропустил фрукт
                        if (fruit.rect.x < GetScreenWidth() / 2 && player1.isAlive) {
                            player1.missedFruits++;
                            if (player1.missedFruits >= 10) {
                                player1.isAlive = false;
                            }
                        }
                        else if (fruit.rect.x >= GetScreenWidth() / 2 && player2.isAlive) {
                            player2.missedFruits++;
                            if (player2.missedFruits >= 10) {
                                player2.isAlive = false;
                            }
                        }
                    }
                    else {
                        player1.missedFruits++;
                    }
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
            switch (fruit.type)
            {
            case 0:
                DrawTexture(Fruit0Texture, fruit.rect.x, fruit.rect.y, RAYWHITE);
                break;
            case 1:
                DrawTexture(Fruit1Texture, fruit.rect.x, fruit.rect.y, RAYWHITE);
                break;
            case 2:
                DrawTexture(Fruit2Texture, fruit.rect.x, fruit.rect.y, RAYWHITE);
                break;
            case 3:
                DrawTexture(Fruit3Texture, fruit.rect.x, fruit.rect.y, RAYWHITE);
                break;
            case 4:
                DrawTexture(Fruit4Texture, fruit.rect.x, fruit.rect.y, RAYWHITE);
                break;
            case 5:
                DrawTexture(Fruit5Texture, fruit.rect.x, fruit.rect.y, RAYWHITE);
                break;
            case 6:
                DrawTexture(Fruit6Texture, fruit.rect.x, fruit.rect.y, RAYWHITE);
                break;
            case 7:
                DrawTexture(Fruit7Texture, fruit.rect.x, fruit.rect.y, RAYWHITE);
                break;
            case 8:
                DrawTexture(Fruit8Texture, fruit.rect.x, fruit.rect.y, RAYWHITE);
                break;
            case 9:
                DrawTexture(Fruit9Texture, fruit.rect.x, fruit.rect.y, RAYWHITE);
                break;
            case 10:
                DrawTexture(Fruit10Texture, fruit.rect.x, fruit.rect.y, RAYWHITE);
                break;
            default:
                break;
            }

            if (fruit.type >= 6) {

            }
        }
    }
}

void ResetGame() {
    gameOver = false;
    gameTime = 120.0f;
    fruitSpeed = 2.0f;
    spawnRate = 1.0f;
    gameStarted = false; // Игра не начата до нажатия SPACE

    // Инициализация игроков с масштабированием под разрешение
    int basketWidth = 100 * GetScreenWidth() / 1920;
    int basketHeight = 60 * GetScreenHeight() / 1080;

    player1.basket = { GetScreenWidth() / 4.0f - basketWidth / 2, GetScreenHeight() - 100.0f, (float)basketWidth, (float)basketHeight };
    player1.score = 0;
    player1.lives = 3;
    player1.missedFruits = 0;
    player1.name = "Player 1";
    player1.color = BLUE;
    player1.isAlive = true;
    // Сброс бонусов игрока 1
    player1.bonuses = PlayerBonusEffects();

    player2.basket = { 3 * GetScreenWidth() / 4.0f - basketWidth / 2, GetScreenHeight() - 100.0f, (float)basketWidth, (float)basketHeight };
    player2.score = 0;
    player2.lives = 3;
    player2.missedFruits = 0;
    player2.name = "Player 2";
    player2.color = RED;
    player2.isAlive = true;
    // Сброс бонусов игрока 2
    player2.bonuses = PlayerBonusEffects();

    // Сбрасываем все глобальные бонусные эффекты
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

    DrawText("FruityDrop", GetScreenWidth() / 2 - 100, 80, 40, DARKBLUE);

    // Menu buttons
    int buttonWidth = 200 * GetScreenWidth() / 1920;
    int buttonHeight = 50 * GetScreenHeight() / 1080;
    int buttonSpacing = 70 * GetScreenHeight() / 1080;

    Rectangle classicBtn = { GetScreenWidth() / 2.0f - buttonWidth / 2, 180, (float)buttonWidth, (float)buttonHeight };
    Rectangle survivalBtn = { GetScreenWidth() / 2.0f - buttonWidth / 2, 180 + buttonSpacing, (float)buttonWidth, (float)buttonHeight };
    Rectangle timeBtn = { GetScreenWidth() / 2.0f - buttonWidth / 2, 180 + 2 * buttonSpacing, (float)buttonWidth, (float)buttonHeight };
    Rectangle twoPlayerBtn = { GetScreenWidth() / 2.0f - buttonWidth / 2, 180 + 3 * buttonSpacing, (float)buttonWidth, (float)buttonHeight };
    Rectangle ratingBtn = { GetScreenWidth() / 2.0f - buttonWidth / 2, 180 + 4 * buttonSpacing, (float)buttonWidth, (float)buttonHeight };
    Rectangle exitBtn = { GetScreenWidth() / 2.0f - buttonWidth / 2, 180 + 5 * buttonSpacing, (float)buttonWidth, (float)buttonHeight };

    DrawRectangleRec(classicBtn, GREEN);
    DrawRectangleRec(survivalBtn, BLUE);
    DrawRectangleRec(timeBtn, ORANGE);
    DrawRectangleRec(twoPlayerBtn, PURPLE);
    DrawRectangleRec(ratingBtn, DARKPURPLE);
    DrawRectangleRec(exitBtn, RED);

    int fontSize = 20 * GetScreenWidth() / 1920;

    DrawText("Classic", classicBtn.x + 60, classicBtn.y + 15, fontSize, WHITE);
    DrawText("Survival", survivalBtn.x + 50, survivalBtn.y + 15, fontSize, WHITE);
    DrawText("Time Attack", timeBtn.x + 35, timeBtn.y + 15, fontSize, WHITE);
    DrawText("2 Players", twoPlayerBtn.x + 50, twoPlayerBtn.y + 15, fontSize, WHITE);
    DrawText("My Scores", ratingBtn.x + 50, ratingBtn.y + 15, fontSize, WHITE);
    DrawText("Exit", exitBtn.x + 75, exitBtn.y + 15, fontSize, WHITE); 

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
        if (CheckCollisionPointRec(mousePoint, twoPlayerBtn)) {
            inMenu = false;
            gameMode = 4;
            ResetGame();
        }
        if (CheckCollisionPointRec(mousePoint, ratingBtn)) {
            showTopRating = true;
        }
        if (CheckCollisionPointRec(mousePoint, exitBtn)) {
            gameMode = 6; // Выход из игры
        }
    }
}

void ToggleFullscreen() {
    if (fullscreen) {
        SetWindowSize(screenWidth, screenHeight);
        ToggleFullscreen();
        fullscreen = false;
    }
    else {
        int monitor = GetCurrentMonitor();
        int monitorWidth = GetMonitorWidth(monitor);
        int monitorHeight = GetMonitorHeight(monitor);
        SetWindowSize(monitorWidth, monitorHeight);
        ToggleFullscreen();
        fullscreen = true;
    }
}

int main(void) {
    // Получаем размеры монитора для полноэкранного режима
    int monitor = GetCurrentMonitor();
    int monitorWidth = GetMonitorWidth(monitor);
    int monitorHeight = GetMonitorHeight(monitor);

    InitWindow(0, 0, "FruityDrop");
    SetTargetFPS(60);

    LoadPlayerScores();
    ResetGame(); // Автоматически запускаем режим 2 Players
    std::vector<Fruit> fruits;

    fruitSize = 60 * GetScreenWidth() / 1920;

    Image image = LoadImage("apple.png");
    ImageResize(&image, fruitSize, fruitSize);
    Fruit0Texture = LoadTextureFromImage(image);

    image = LoadImage("orange.png");
    ImageResize(&image, fruitSize, fruitSize);
    Fruit1Texture = LoadTextureFromImage(image);

    image = LoadImage("watermelon.png");
    ImageResize(&image, fruitSize, fruitSize);
    Fruit2Texture = LoadTextureFromImage(image);

    image = LoadImage("pear.png");
    ImageResize(&image, fruitSize, fruitSize);
    Fruit3Texture = LoadTextureFromImage(image);

    image = LoadImage("bad_fruit1.png");
    ImageResize(&image, fruitSize, fruitSize);
    Fruit4Texture = LoadTextureFromImage(image);

    image = LoadImage("bad_fruit2.png");
    ImageResize(&image, fruitSize, fruitSize);
    Fruit5Texture = LoadTextureFromImage(image);

    image = LoadImage("banana.png");
    ImageResize(&image, fruitSize, fruitSize);
    Fruit6Texture = LoadTextureFromImage(image);

    image = LoadImage("cherry.png");
    ImageResize(&image, fruitSize, fruitSize);
    Fruit7Texture = LoadTextureFromImage(image);

    image = LoadImage("gold_apple.png");
    ImageResize(&image, fruitSize, fruitSize);
    Fruit8Texture = LoadTextureFromImage(image);

    image = LoadImage("gold_watermelon.png");
    ImageResize(&image, fruitSize, fruitSize);
    Fruit9Texture = LoadTextureFromImage(image);

    image = LoadImage("kiwi.png");
    ImageResize(&image, fruitSize, fruitSize);
    Fruit10Texture = LoadTextureFromImage(image);

    UnloadImage(image);

    while (!WindowShouldClose()) {
        // Проверка на выход из игры
        if (gameMode == 6) {
            break;
        }

        // Проверяем состояние геймпадов
        CheckGamepads();

        // Переключение полноэкранного режима по F11
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (showTopRating) {
            DrawTopRating();
            if (IsKeyPressed(KEY_ESCAPE)) {
                showTopRating = false;
            }
            EndDrawing();
            continue;
        }

        if (inMenu) {
            DrawMenu();
            EndDrawing();
            continue;
        }

        // Игровой процесс
        if (!gameOver) {
            // Экран начала игры
            if (!gameStarted) {
                ClearBackground(SKYBLUE);

                // Рисуем разделенный экран
                DrawRectangle(0, 0, GetScreenWidth() / 2, GetScreenHeight(), Fade(SKYBLUE, 0.1f));
                DrawRectangle(GetScreenWidth() / 2, 0, GetScreenWidth() / 2, GetScreenHeight(), Fade(LIGHTGRAY, 0.1f));
                DrawLine(GetScreenWidth() / 2, 0, GetScreenWidth() / 2, GetScreenHeight(), DARKGRAY);

                // Рисуем корзины
                DrawRectangleRec(player1.basket, player1.color);
                DrawRectangleRec(player2.basket, player2.color);

                // Информация об игроках
                int instructionFontSize = 25 * GetScreenWidth() / 1920;
                DrawText("PLAYER 1", 20, 10, instructionFontSize, BLUE);
                DrawText("A/D keys to move", 20, 40, instructionFontSize - 5, DARKBLUE);
                if (gamepad1.connected) {
                    DrawText("Gamepad: LEFT/RIGHT stick", 20, 65, instructionFontSize - 5, GREEN);
                    DrawText("X button: switch stick", 20, 90, instructionFontSize - 5, YELLOW);
                }

                DrawText("PLAYER 2", GetScreenWidth() - 150, 10, instructionFontSize, RED);
                DrawText("Arrow keys to move", GetScreenWidth() - 180, 40, instructionFontSize - 5, DARKBLUE);
                if (gamepad2.connected) {
                    DrawText("Gamepad: LEFT/RIGHT stick", GetScreenWidth() - 220, 65, instructionFontSize - 5, GREEN);
                    DrawText("X button: switch stick", GetScreenWidth() - 180, 90, instructionFontSize - 5, YELLOW);
                }

                // Инструкция
                DrawText("TWO PLAYERS MODE", GetScreenWidth() / 2 - 120, GetScreenHeight() / 2 - 60, instructionFontSize + 5, PURPLE);
                DrawText("Catch good fruits, avoid bad ones!", GetScreenWidth() / 2 - 180, GetScreenHeight() / 2 - 20, instructionFontSize, DARKBLUE);
                DrawText("Collect bonus fruits for special effects!", GetScreenWidth() / 2 - 200, GetScreenHeight() / 2 + 10, instructionFontSize, DARKBLUE);
                DrawText("Press SPACE to start the game!", GetScreenWidth() / 2 - 160, GetScreenHeight() / 2 + 50, instructionFontSize + 5, GREEN);

                //Можно начать игру кнопкой START на геймпаде
                bool startPressed = IsKeyPressed(KEY_SPACE);
                if (gamepad1.connected && IsGamepadButtonPressed(gamepad1.gamepadNumber, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                    startPressed = true;
                }
                if (gamepad2.connected && IsGamepadButtonPressed(gamepad2.gamepadNumber, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                    startPressed = true;
                }

                if (startPressed) {
                    gameStarted = true;
                }

                EndDrawing();
                continue;
            }

            // Основной игровой процесс
            UpdateBonusEffects();

            // Controls - только для живых игроков
            if (gameMode == 4) {
                // Player 1 управление: геймпад или клавиатура
                if (player1.isAlive) {
                    if (gamepad1.connected) {
                        MoveRectangleWithGamepad(player1.basket, gamepad1, 1);
                    }
                    else {
                        MoveRectangle(player1.basket, false, 1); // Клавиши A/D
                    }
                }

                // Player 2 управление: геймпад или клавиатура  
                if (player2.isAlive) {
                    if (gamepad2.connected) {
                        MoveRectangleWithGamepad(player2.basket, gamepad2, 2);
                    }
                    else {
                        MoveRectangle(player2.basket, true, 2); // Стрелки
                    }
                }
            }
            else {
                // Одиночные режимы - можно использовать геймпад 1
                if (gamepad1.connected) {
                    MoveRectangleWithGamepad(player1.basket, gamepad1);
                }
                else {
                    MoveRectangle(player1.basket, false);
                }
            }

            // Timer only for Time Attack and Two Players modes
            if (gameMode == 3 || gameMode == 4) {
                gameTime -= GetFrameTime();
                if (gameTime <= 0) {
                    gameOver = true;
                }
            }

            // Проверка условий завершения игры для всех режимов
            if (gameMode == 4) {
                // В режиме двух игроков игра заканчивается по таймеру или если оба игрока проиграли
                if (gameTime <= 0 || (!player1.isAlive && !player2.isAlive)) {
                    gameOver = true;
                }
            }
            else if (gameMode == 1 || gameMode == 2) {
                // В одиночных режимах
                if (player1.lives <= 0 || player1.missedFruits >= 10) {
                    gameOver = true;
                }
            }

            // Spawn fruits только для живых игроков
            timeSinceLastSpawn += GetFrameTime();
            if (timeSinceLastSpawn >= spawnRate) {
                if (gameMode == 4) {
                    // Для двух игроков создаем фрукты только для живых игроков
                    if (player1.isAlive) fruits.push_back(CreateFruit(1)); // Для игрока 1
                    if (player2.isAlive) fruits.push_back(CreateFruit(2)); // Для игрока 2
                }
                else {
                    fruits.push_back(CreateFruit());
                }
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
                else if (gameMode == 4) {
                    spawnRate = 0.6f;
                    fruitSpeed = 3.0f;
                }
            }

            UpdateFruits(fruits);

            // Drawing
            if (gameMode == 4) {
                // Разделенный экран для двух игроков
                DrawRectangle(0, 0, GetScreenWidth() / 2, GetScreenHeight(), Fade(SKYBLUE, 0.1f));
                DrawRectangle(GetScreenWidth() / 2, 0, GetScreenWidth() / 2, GetScreenHeight(), Fade(LIGHTGRAY, 0.1f));

                // Разделительная линия
                DrawLine(GetScreenWidth() / 2, 0, GetScreenWidth() / 2, GetScreenHeight(), DARKGRAY);

                // Рисуем корзины только живых игроков
                if (player1.isAlive) DrawRectangleRec(player1.basket, player1.color);
                if (player2.isAlive) DrawRectangleRec(player2.basket, player2.color);
            }
            else {
                DrawRectangleRec(player1.basket, ORANGE);
            }

            DrawFruits(fruits);

            // UI
            int uiFontSize = 25 * GetScreenWidth() / 1920;
            int smallFontSize = 20 * GetScreenWidth() / 1920;

            if (gameMode == 4) {
                // Player 1 UI
                int playerInfoY = 100; 

                DrawText("PLAYER 1", 20, playerInfoY, uiFontSize, BLUE);
                DrawText(TextFormat("Score: %d", player1.score), 20, playerInfoY + 30, smallFontSize, DARKBLUE);
                DrawText(TextFormat("Lives: %d", player1.lives), 20, playerInfoY + 55, smallFontSize, player1.isAlive ? RED : GRAY);
                DrawText(TextFormat("Missed: %d/10", player1.missedFruits), 20, playerInfoY + 80, smallFontSize, DARKBLUE);
                if (!player1.isAlive) DrawText("ELIMINATED!", 20, playerInfoY + 105, smallFontSize - 5, RED);

                // Player 2 UI
                DrawText("PLAYER 2", GetScreenWidth() - 150, playerInfoY, uiFontSize, RED);
                DrawText(TextFormat("Score: %d", player2.score), GetScreenWidth() - 150, playerInfoY + 30, smallFontSize, DARKBLUE);
                DrawText(TextFormat("Lives: %d", player2.lives), GetScreenWidth() - 150, playerInfoY + 55, smallFontSize, player2.isAlive ? RED : GRAY);
                DrawText(TextFormat("Missed: %d/10", player2.missedFruits), GetScreenWidth() - 150, playerInfoY + 80, smallFontSize, DARKBLUE);
                if (!player2.isAlive) DrawText("ELIMINATED!", GetScreenWidth() - 150, playerInfoY + 105, smallFontSize - 5, RED);

                // Таймер по центру
                DrawText(TextFormat("Time: %d", (int)gameTime), GetScreenWidth() / 2 - 40, 30, uiFontSize, DARKBLUE);

                // Отображение активных бонусов для каждого живого игрока 
                int bonusY1 = playerInfoY + 130; // Начинаем ниже информации об игроке
                int bonusY2 = playerInfoY + 130;

                // Бонусы Player 1 (слева)
                if (player1.isAlive) {
                    if (player1.bonuses.slowMotionActive) {
                        DrawText("Slow Motion!", 20, bonusY1, smallFontSize - 5, BLUE);
                        bonusY1 += 20;
                    }
                    if (player1.bonuses.doublePointsActive) {
                        DrawText("Double Points!", 20, bonusY1, smallFontSize - 5, GOLD);
                        bonusY1 += 20;
                    }
                    if (player1.bonuses.speedBoostActive) {
                        DrawText("Speed Boost!", 20, bonusY1, smallFontSize - 5, ORANGE);
                        bonusY1 += 20;
                    }
                    if (player1.bonuses.freezeActive) {
                        DrawText("Freeze!", 20, bonusY1, smallFontSize - 5, SKYBLUE);
                        bonusY1 += 20;
                    }
                }

                // Бонусы Player 2 (справа)
                if (player2.isAlive) {
                    if (player2.bonuses.slowMotionActive) {
                        DrawText("Slow Motion!", GetScreenWidth() - 150, bonusY2, smallFontSize - 5, BLUE);
                        bonusY2 += 20;
                    }
                    if (player2.bonuses.doublePointsActive) {
                        DrawText("Double Points!", GetScreenWidth() - 150, bonusY2, smallFontSize - 5, GOLD);
                        bonusY2 += 20;
                    }
                    if (player2.bonuses.speedBoostActive) {
                        DrawText("Speed Boost!", GetScreenWidth() - 150, bonusY2, smallFontSize - 5, ORANGE);
                        bonusY2 += 20;
                    }
                    if (player2.bonuses.freezeActive) {
                        DrawText("Freeze!", GetScreenWidth() - 150, bonusY2, smallFontSize - 5, SKYBLUE);
                        bonusY2 += 20;
                    }
                }

                // Отображение информации о геймпадах
                DrawGamepadInfo();

                // Отображение лидера
                if (player1.isAlive && player2.isAlive) {
                    if (player1.score > player2.score) {
                        DrawText("PLAYER 1 LEADING!", GetScreenWidth() / 2 - 80, GetScreenHeight() - 60, smallFontSize, BLUE);
                    }
                    else if (player2.score > player1.score) {
                        DrawText("PLAYER 2 LEADING!", GetScreenWidth() / 2 - 80, GetScreenHeight() - 60, smallFontSize, RED);
                    }
                    else {
                        DrawText("TIE!", GetScreenWidth() / 2 - 20, GetScreenHeight() - 60, smallFontSize, PURPLE);
                    }
                }
                else if (player1.isAlive && !player2.isAlive) {
                    DrawText("PLAYER 1 WINS BY ELIMINATION!", GetScreenWidth() / 2 - 140, GetScreenHeight() - 60, smallFontSize, BLUE);
                }
                else if (!player1.isAlive && player2.isAlive) {
                    DrawText("PLAYER 2 WINS BY ELIMINATION!", GetScreenWidth() / 2 - 140, GetScreenHeight() - 60, smallFontSize, RED);
                }
            }
            else {
                // Одиночные режимы
                DrawText("Score:", 10, 10, uiFontSize, DARKBLUE);
                DrawText(TextFormat("%d", player1.score), 120, 10, uiFontSize, DARKBLUE);

                DrawText("Lives:", 10, 50, uiFontSize, DARKBLUE);
                DrawText(TextFormat("%d", player1.lives), 120, 50, uiFontSize, RED);

                DrawText("Missed:", 10, 90, smallFontSize, DARKBLUE);
                DrawText(TextFormat("%d/10", player1.missedFruits), 120, 90, smallFontSize, DARKBLUE);

                if (gameMode == 3) {
                    DrawText(TextFormat("Time: %d", (int)gameTime), GetScreenWidth() - 150, 10, uiFontSize, DARKBLUE);
                }

                // Отображение активных бонусов для одиночных режимов
                int bonusY = 130;
                if (slowMotionActive) {
                    DrawText("Slow Motion!", 10, bonusY, smallFontSize, BLUE);
                    bonusY += 25;
                }
                if (doublePointsActive) {
                    DrawText("Double Points!", 10, bonusY, smallFontSize, GOLD);
                    bonusY += 25;
                }
                if (speedBoostActive) {
                    DrawText("Speed Boost!", 10, bonusY, smallFontSize, ORANGE);
                    bonusY += 25;
                }
                if (freezeActive) {
                    DrawText("Freeze!", 10, bonusY, smallFontSize, SKYBLUE);
                    bonusY += 25;
                }

                // Информация о геймпаде для одиночного режима
                if (gamepad1.connected) {
                    DrawText("Gamepad Connected", GetScreenWidth() - 200, GetScreenHeight() - 40, smallFontSize - 2, GREEN);
                }
            }

        }
        else {
            // Game over screen - сохраняем рекорды
            const char* modeName = "";
            std::string modeStr = "";

            if (gameMode == 1) {
                modeName = "Classic Mode";
                modeStr = "Classic";
                SavePlayerScore(player1.score, modeStr);
            }
            else if (gameMode == 2) {
                modeName = "Survival Mode";
                modeStr = "Survival";
                SavePlayerScore(player1.score, modeStr);
            }
            else if (gameMode == 3) {
                modeName = "Time Attack";
                modeStr = "Time Attack";
                SavePlayerScore(player1.score, modeStr);
            }
            else if (gameMode == 4) {
                modeName = "Two Players Mode";
                modeStr = "Two Players";
                SavePlayerScore(player1.score, modeStr, 1);
                SavePlayerScore(player2.score, modeStr, 2);
            }

            // Проверяем, побит ли рекорд
            bool newRecord1 = false;
            bool newRecord2 = false;

            if (gameMode == 4) {
                if (player1.score > playerScores.twoPlayerBestP1) newRecord1 = true;
                if (player2.score > playerScores.twoPlayerBestP2) newRecord2 = true;
            }
            else {
                if (gameMode == 1 && player1.score > playerScores.classicBest) newRecord1 = true;
                else if (gameMode == 2 && player1.score > playerScores.survivalBest) newRecord1 = true;
                else if (gameMode == 3 && player1.score > playerScores.timeAttackBest) newRecord1 = true;
            }

            int gameOverFontSize = 40 * GetScreenWidth() / 1920;
            int resultFontSize = 30 * GetScreenWidth() / 1920;
            int smallResultFontSize = 25 * GetScreenWidth() / 1920;

            DrawText("Game Over!", GetScreenWidth() / 2 - 80, 150, gameOverFontSize, RED);

            if (newRecord1) {
                DrawText("PLAYER 1 NEW RECORD!", GetScreenWidth() / 2 - 100, 190, smallResultFontSize, GOLD);
            }
            if (newRecord2) {
                DrawText("PLAYER 2 NEW RECORD!", GetScreenWidth() / 2 - 100, 220, smallResultFontSize, GOLD);
            }

            if (gameMode == 4) {
                DrawText(TextFormat("Player 1 Score: %d", player1.score), GetScreenWidth() / 2 - 100, 260, smallResultFontSize, BLUE);
                DrawText(TextFormat("Player 2 Score: %d", player2.score), GetScreenWidth() / 2 - 100, 290, smallResultFontSize, RED);

                if (player1.score > player2.score) {
                    DrawText("PLAYER 1 WINS!", GetScreenWidth() / 2 - 80, 330, resultFontSize, BLUE);
                }
                else if (player2.score > player1.score) {
                    DrawText("PLAYER 2 WINS!", GetScreenWidth() / 2 - 80, 330, resultFontSize, RED);
                }
                else {
                    DrawText("DRAW!", GetScreenWidth() / 2 - 40, 330, resultFontSize, PURPLE);
                }
            }
            else {
                DrawText(TextFormat("Final Score: %d", player1.score), GetScreenWidth() / 2 - 70, 260, resultFontSize, DARKBLUE);
            }

            DrawText("Press SPACE for menu", GetScreenWidth() / 2 - 100, 390, smallResultFontSize - 5, DARKBLUE);
            DrawText("Press R for Records", GetScreenWidth() / 2 - 100, 420, smallResultFontSize - 5, DARKBLUE);

            // Также можно использовать кнопку START на геймпаде для возврата в меню
            bool menuPressed = IsKeyPressed(KEY_SPACE);
            if (gamepad1.connected && IsGamepadButtonPressed(gamepad1.gamepadNumber, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                menuPressed = true;
            }
            if (gamepad2.connected && IsGamepadButtonPressed(gamepad2.gamepadNumber, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                menuPressed = true;
            }

            if (menuPressed) {
                inMenu = true;
                gameMode = 0; // Возврат в меню
                fruits.clear();
            }
            if (IsKeyPressed(KEY_R)) {
                showTopRating = true;
            }
        }

        // Возврат в меню по ESC
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (!inMenu && !showTopRating) {
                inMenu = true;
                gameMode = 0; // Возврат в меню
                fruits.clear();
            }
            else if (showTopRating) {
                showTopRating = false;
            }
        }

        EndDrawing();
    }
    UnloadTexture(Fruit0Texture);
    UnloadTexture(Fruit1Texture);
    UnloadTexture(Fruit2Texture);
    UnloadTexture(Fruit3Texture);
    UnloadTexture(Fruit4Texture);
    UnloadTexture(Fruit5Texture);
    UnloadTexture(Fruit6Texture);
    UnloadTexture(Fruit7Texture);
    UnloadTexture(Fruit8Texture);
    UnloadTexture(Fruit9Texture);
    UnloadTexture(Fruit10Texture);
    CloseWindow();
    return 0;
}