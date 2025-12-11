#include "raylib.h"
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

int baseWidth = 1920;
int baseHeight = 1080;
int screenWidth;
int screenHeight;
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
Texture2D Fruit11Texture;
Texture2D Fruit12Texture;
Texture2D BasketTexture;

// Текстуры для меню
Texture2D playBigTexture;
Texture2D recTexture;
Texture2D howToPlayTexture;
Texture2D exitTexture;
Texture2D playSmallTexture;
Texture2D backgroundTexture;

// Переменные для масштабирования
float scaleX = 1.0f;
float scaleY = 1.0f;

// Вспомогательные функции для масштабирования
int ScaleInt(int value) {
    return static_cast<int>(value * scaleX);
}

int ScaleIntY(int value) {
    return static_cast<int>(value * scaleY);
}

float ScaleFloat(float value) {
    return value * scaleX;
}

float ScaleFloatY(float value) {
    return value * scaleY;
}

void UpdateScale() {
    scaleX = static_cast<float>(screenWidth) / baseWidth;
    scaleY = static_cast<float>(screenHeight) / baseHeight;
}

// Переменные для меню
int currentMenu = 0;
int selectedGameMode = 0;

// Структура для кнопок
struct MenuButton {
    Rectangle rect;
    int id;
    bool hovered;
};

// Структура для fruits
struct Fruit {
    Rectangle rect;
    int type;
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
    int twoPlayerBestP1 = 0;
    int twoPlayerBestP2 = 0;
};

// Структура для бонусных эффектов
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
    bool useLeftStick;
};

// Global
int gameMode = 0;
float gameTime = 120.0f;
bool gameOver = false;
bool showTopRating = false;
bool gameStarted = false;
bool fullscreen = true;

float fruitSpeed = 2.0f;
float spawnRate = 1.0f;
float timeSinceLastSpawn = 0.0f;

// Global bonus effects
float slowMotionTimer = 0.0f;
float doublePointsTimer = 0.0f;
float freezeTimer = 0.0f;
float speedBoostTimer = 0.0f;
bool slowMotionActive = false;
bool doublePointsActive = false;
bool freezeActive = false;
bool speedBoostActive = false;

// For Scores system 
PlayerBestScores playerScores;

// For Two Players system 
Player player1, player2;

// For Gamepads 
GamepadController gamepad1 = { 0, false, true };
GamepadController gamepad2 = { 1, false, true };

// Массивы кнопок
std::vector<MenuButton> mainMenuButtons;
std::vector<MenuButton> chooseModeButtons;

// Для навигации по меню с геймпада
int selectedButton = 0;
float gamepadMenuCooldown = 0.0f;

// Загрузка текстур меню
void LoadMenuTextures() {
    // Загрузка фона
    Image img = LoadImage("background.png");
    if (img.data != NULL) {
        ImageResize(&img, screenWidth, screenHeight);
        backgroundTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    else {
        // Если фон не загружен, создаем однотонный фон
        img = GenImageColor(screenWidth, screenHeight, DARKBLUE);
        backgroundTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    // Большая кнопка Play
    int playWidth = ScaleInt(300);
    int playHeight = ScaleInt(70);
    img = LoadImage("play.png");
    if (img.data != NULL) {
        ImageCrop(&img, { 37, 32, 416, 72 });
        ImageResize(&img, playWidth, playHeight);
        playBigTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    else {
        // Создаем кнопку Play с однотонным цветом
        img = GenImageColor(playWidth, playHeight, GREEN);
        playBigTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    // Маленькие кнопки
    int smallBtnSize = ScaleInt(80);
    img = LoadImage("rec.png");
    if (img.data != NULL) {
        ImageResize(&img, smallBtnSize, smallBtnSize);
        recTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    else {
        img = GenImageColor(smallBtnSize, smallBtnSize, BLUE);
        recTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    img = LoadImage("how_to_play.png");
    if (img.data != NULL) {
        ImageResize(&img, smallBtnSize, smallBtnSize);
        howToPlayTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    else {
        img = GenImageColor(smallBtnSize, smallBtnSize, ORANGE);
        howToPlayTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    img = LoadImage("exit.png");
    if (img.data != NULL) {
        ImageResize(&img, smallBtnSize, smallBtnSize);
        exitTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    else {
        img = GenImageColor(smallBtnSize, smallBtnSize, RED);
        exitTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    // Маленькая кнопка Play для меню выбора
    int smallPlayWidth = ScaleInt(300);
    int smallPlayHeight = ScaleInt(70);
    img = LoadImage("play.png");
    if (img.data != NULL) {
        ImageCrop(&img, { 37, 32, 416, 72 });
        ImageResize(&img, smallPlayWidth, smallPlayHeight);
        playSmallTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    else {
        img = GenImageColor(smallPlayWidth, smallPlayHeight, GREEN);
        playSmallTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }
}

// Инициализация главного меню
void InitMainMenu() {
    mainMenuButtons.clear();

    // Большая кнопка Play в центре
    MenuButton playBigBtn;
    playBigBtn.rect = {
        screenWidth / 2.0f - ScaleFloat(150),
        screenHeight / 2.0f - ScaleFloat(75),
        ScaleFloat(300),
        ScaleFloat(70)
    };
    playBigBtn.id = 0;
    playBigBtn.hovered = false;
    mainMenuButtons.push_back(playBigBtn);

    // Маленькая кнопка Records слева
    MenuButton recBtn;
    recBtn.rect = {
        screenWidth / 2.0f - ScaleFloat(200),
        screenHeight / 2.0f + ScaleFloat(100),
        ScaleFloat(80),
        ScaleFloat(80)
    };
    recBtn.id = 1;
    recBtn.hovered = false;
    mainMenuButtons.push_back(recBtn);

    // Маленькая кнопка How to Play в центре
    MenuButton howBtn;
    howBtn.rect = {
        screenWidth / 2.0f - ScaleFloat(40),
        screenHeight / 2.0f + ScaleFloat(100),
        ScaleFloat(80),
        ScaleFloat(80)
    };
    howBtn.id = 2;
    howBtn.hovered = false;
    mainMenuButtons.push_back(howBtn);

    // Маленькая кнопка Exit справа
    MenuButton exitBtn;
    exitBtn.rect = {
        screenWidth / 2.0f + ScaleFloat(120),
        screenHeight / 2.0f + ScaleFloat(100),
        ScaleFloat(80),
        ScaleFloat(80)
    };
    exitBtn.id = 3;
    exitBtn.hovered = false;
    mainMenuButtons.push_back(exitBtn);

    selectedButton = 0;
}

// Инициализация меню выбора режима
void InitChooseModeMenu() {
    chooseModeButtons.clear();

    int buttonWidth = ScaleInt(180);
    int buttonHeight = ScaleInt(100);
    int startX = screenWidth / 2 - (2 * buttonWidth + ScaleInt(40)) / 2;
    int startY = ScaleInt(200);

    // Classic Mode (левая верхняя)
    MenuButton classicBtn;
    classicBtn.rect = { (float)startX, (float)startY, (float)buttonWidth, (float)buttonHeight };
    classicBtn.id = 4;
    classicBtn.hovered = false;
    chooseModeButtons.push_back(classicBtn);

    // Survival Mode (правая верхняя)
    MenuButton survivalBtn;
    survivalBtn.rect = { (float)(startX + buttonWidth + ScaleInt(40)), (float)startY, (float)buttonWidth, (float)buttonHeight };
    survivalBtn.id = 5;
    survivalBtn.hovered = false;
    chooseModeButtons.push_back(survivalBtn);

    // Time Attack (левая нижняя)
    MenuButton timeBtn;
    timeBtn.rect = { (float)startX, (float)(startY + buttonHeight + ScaleInt(40)), (float)buttonWidth, (float)buttonHeight };
    timeBtn.id = 6;
    timeBtn.hovered = false;
    chooseModeButtons.push_back(timeBtn);

    // Two Players (правая нижняя)
    MenuButton twoPlayerBtn;
    twoPlayerBtn.rect = { (float)(startX + buttonWidth + ScaleInt(40)), (float)(startY + buttonHeight + ScaleInt(40)), (float)buttonWidth, (float)buttonHeight };
    twoPlayerBtn.id = 7;
    twoPlayerBtn.hovered = false;
    chooseModeButtons.push_back(twoPlayerBtn);

    // Большая кнопка Play внизу
    MenuButton playSmallBtn;
    playSmallBtn.rect = {
        screenWidth / 2.0f - ScaleFloat(150),
        screenHeight - ScaleFloat(250.0f),
        ScaleFloat(300),
        ScaleFloat(70)
    };
    playSmallBtn.id = 8;
    playSmallBtn.hovered = false;
    chooseModeButtons.push_back(playSmallBtn);

    // Кнопка Records слева
    MenuButton recBtn2;
    recBtn2.rect = {
        screenWidth / 2.0f - ScaleFloat(200),
        screenHeight - ScaleFloat(150.0f),
        ScaleFloat(80),
        ScaleFloat(80)
    };
    recBtn2.id = 9;
    recBtn2.hovered = false;
    chooseModeButtons.push_back(recBtn2);

    // Кнопка Exit справа
    MenuButton exitBtn2;
    exitBtn2.rect = {
        screenWidth / 2.0f + ScaleFloat(120),
        screenHeight - ScaleFloat(150.0f),
        ScaleFloat(80),
        ScaleFloat(80)
    };
    exitBtn2.id = 10;
    exitBtn2.hovered = false;
    chooseModeButtons.push_back(exitBtn2);

    selectedButton = 0;
    selectedGameMode = 0;
}

// Отрисовка главного меню
void DrawMainMenu() {
    // Сначала очищаем экран
    ClearBackground(BLACK);

    DrawTexture(backgroundTexture, 0, 0, WHITE);
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.3f));

    int titleSize = ScaleInt(50);
    DrawText("FruityDrop", screenWidth / 2 - MeasureText("FruityDrop", titleSize) / 2, ScaleInt(80), titleSize, YELLOW);

    for (size_t i = 0; i < mainMenuButtons.size(); i++) {
        MenuButton btn = mainMenuButtons[i];
        Texture2D textureToDraw;

        switch (btn.id) {
        case 0: textureToDraw = playBigTexture; break;
        case 1: textureToDraw = recTexture; break;
        case 2: textureToDraw = howToPlayTexture; break;
        case 3: textureToDraw = exitTexture; break;
        }

        Color tint = btn.hovered || (selectedButton == static_cast<int>(i) && currentMenu == 0) ?
            Color{ 255, 255, 200, 255 } : WHITE;

        DrawTexture(textureToDraw, btn.rect.x, btn.rect.y, tint);

        if (btn.hovered || (selectedButton == static_cast<int>(i) && currentMenu == 0)) {
            DrawRectangleLinesEx(btn.rect, 3, GOLD);
        }
    }

    if (gamepad1.connected) {
        int hintSize = ScaleInt(20);
        DrawText("Use D-Pad to navigate, A to select",
            screenWidth / 2 - MeasureText("Use D-Pad to navigate, A to select", hintSize) / 2,
            screenHeight - ScaleInt(40), hintSize, WHITE);
    }
}

// Отрисовка меню выбора режима
void DrawChooseModeMenu() {
    // Сначала очищаем экран
    ClearBackground(BLACK);

    DrawTexture(backgroundTexture, 0, 0, WHITE);
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.3f));

    int titleSize = ScaleInt(40);
    DrawText("Choose Game Mode", screenWidth / 2 - MeasureText("Choose Game Mode", titleSize) / 2, ScaleInt(80), titleSize, YELLOW);

    for (size_t i = 0; i < 4; i++) {
        MenuButton btn = chooseModeButtons[i];
        Color btnColor;
        const char* btnText;

        switch (btn.id) {
        case 4: btnColor = Fade(GREEN, 0.8f); btnText = "Classic"; break;
        case 5: btnColor = Fade(BLUE, 0.8f); btnText = "Survival"; break;
        case 6: btnColor = Fade(ORANGE, 0.8f); btnText = "Time Attack"; break;
        case 7: btnColor = Fade(PURPLE, 0.8f); btnText = "2 Players"; break;
        default: btnColor = Fade(GRAY, 0.8f); btnText = ""; break;
        }

        if (selectedGameMode == btn.id - 3) {
            btnColor = GOLD;
        }

        DrawRectangleRec(btn.rect, btnColor);

        int textSize = ScaleInt(20);
        DrawText(btnText, btn.rect.x + btn.rect.width / 2 - MeasureText(btnText, textSize) / 2,
            btn.rect.y + btn.rect.height / 2 - 10, textSize, WHITE);

        if (btn.hovered || (selectedButton == static_cast<int>(i) && currentMenu == 1)) {
            DrawRectangleLinesEx(btn.rect, 3, YELLOW);
        }
    }

    for (size_t i = 4; i < chooseModeButtons.size(); i++) {
        MenuButton btn = chooseModeButtons[i];
        Texture2D textureToDraw;

        switch (btn.id) {
        case 8: textureToDraw = playSmallTexture; break;
        case 9: textureToDraw = recTexture; break;
        case 10: textureToDraw = exitTexture; break;
        }

        Color tint = btn.hovered || (selectedButton == static_cast<int>(i) && currentMenu == 1) ?
            Color{ 255, 255, 200, 255 } : WHITE;

        DrawTexture(textureToDraw, btn.rect.x, btn.rect.y, tint);

        if (btn.hovered || (selectedButton == static_cast<int>(i) && currentMenu == 1)) {
            DrawRectangleLinesEx(btn.rect, 3, GOLD);
        }
    }

    if (selectedGameMode >= 1) {
        const char* modeName = "";
        switch (selectedGameMode) {
        case 1: modeName = "Classic Mode"; break;
        case 2: modeName = "Survival Mode"; break;
        case 3: modeName = "Time Attack Mode"; break;
        case 4: modeName = "Two Players Mode"; break;
        }

        int infoSize = ScaleInt(25);
        DrawText(TextFormat("Selected: %s", modeName),
            screenWidth / 2 - MeasureText(TextFormat("Selected: %s", modeName), infoSize) / 2,
            ScaleInt(480), infoSize, YELLOW);

        int hintSize = ScaleInt(20);
        DrawText("Click PLAY button to start",
            screenWidth / 2 - MeasureText("Click PLAY button to start", hintSize) / 2,
            ScaleInt(510), hintSize, GREEN);
    }

    if (gamepad1.connected) {
        int hintSize = ScaleInt(20);
        DrawText("Use D-Pad to navigate, A to select, B to back",
            screenWidth / 2 - MeasureText("Use D-Pad to navigate, A to select, B to back", hintSize) / 2,
            screenHeight - ScaleInt(40), hintSize, WHITE);
    }
}

// Отрисовка таблицы рекордов
void DrawTopRating() {
    // Сначала очищаем экран
    ClearBackground(BLACK);

    DrawTexture(backgroundTexture, 0, 0, WHITE);
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));

    int titleSize = ScaleInt(40);
    DrawText("MY BEST SCORES", screenWidth / 2 - ScaleInt(150), ScaleInt(50), titleSize, YELLOW);

    int startY = ScaleInt(150);
    int modeSize = ScaleInt(30);
    int scoreSize = ScaleInt(25);
    int smallSize = ScaleInt(20);

    // Classic Mode
    DrawText("CLASSIC MODE", screenWidth / 2 - ScaleInt(100), startY, modeSize, GREEN);
    DrawText("Best Score:", screenWidth / 2 - ScaleInt(150), startY + ScaleInt(40), scoreSize, WHITE);
    DrawText(TextFormat("%d", playerScores.classicBest), screenWidth / 2 + ScaleInt(50), startY + ScaleInt(40), scoreSize, GOLD);

    // Survival Mode
    DrawText("SURVIVAL MODE", screenWidth / 2 - ScaleInt(100), startY + ScaleInt(100), modeSize, BLUE);
    DrawText("Best Score:", screenWidth / 2 - ScaleInt(150), startY + ScaleInt(140), scoreSize, WHITE);
    DrawText(TextFormat("%d", playerScores.survivalBest), screenWidth / 2 + ScaleInt(50), startY + ScaleInt(140), scoreSize, GOLD);

    // Time Attack Mode
    DrawText("TIME ATTACK MODE", screenWidth / 2 - ScaleInt(120), startY + ScaleInt(200), modeSize, ORANGE);
    DrawText("Best Score:", screenWidth / 2 - ScaleInt(150), startY + ScaleInt(240), scoreSize, WHITE);
    DrawText(TextFormat("%d", playerScores.timeAttackBest), screenWidth / 2 + ScaleInt(50), startY + ScaleInt(240), scoreSize, GOLD);

    // Two Players Mode
    DrawText("TWO PLAYERS MODE", screenWidth / 2 - ScaleInt(120), startY + ScaleInt(280), modeSize, RED);
    DrawText("Player 1 Best:", screenWidth / 2 - ScaleInt(150), startY + ScaleInt(320), scoreSize, BLUE);
    DrawText(TextFormat("%d", playerScores.twoPlayerBestP1), screenWidth / 2 + ScaleInt(50), startY + ScaleInt(320), scoreSize, GOLD);
    DrawText("Player 2 Best:", screenWidth / 2 - ScaleInt(150), startY + ScaleInt(360), scoreSize, RED);
    DrawText(TextFormat("%d", playerScores.twoPlayerBestP2), screenWidth / 2 + ScaleInt(50), startY + ScaleInt(360), scoreSize, GOLD);

    // Statistics
    DrawText("STATISTICS", screenWidth / 2 - ScaleInt(70), startY + ScaleInt(420), scoreSize, PURPLE);

    int totalBest = playerScores.classicBest + playerScores.survivalBest + playerScores.timeAttackBest +
        (playerScores.twoPlayerBestP1 > playerScores.twoPlayerBestP2 ? playerScores.twoPlayerBestP1 : playerScores.twoPlayerBestP2);

    DrawText(TextFormat("Total Best: %d", totalBest), screenWidth / 2 - ScaleInt(80), startY + ScaleInt(460), smallSize, WHITE);

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

    DrawText(TextFormat("Favorite Mode: %s", favoriteMode.c_str()), screenWidth / 2 - ScaleInt(100), startY + ScaleInt(490), smallSize, WHITE);

    DrawText("Press Q to return", screenWidth / 2 - ScaleInt(100), screenHeight - ScaleInt(50), smallSize, WHITE);
}

// Обработка навигации по меню с геймпада
void UpdateMenuNavigation() {
    if (gamepadMenuCooldown > 0) {
        gamepadMenuCooldown -= GetFrameTime();
        return;
    }

    bool moved = false;

    if (gamepad1.connected) {
        // Навигация вверх/вниз
        if (IsGamepadButtonPressed(gamepad1.gamepadNumber, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
            if (currentMenu == 0) {
                if (selectedButton == 0) selectedButton = 1;
                else if (selectedButton >= 1 && selectedButton <= 3) selectedButton = 0;
            }
            else {
                if (selectedButton >= 0 && selectedButton <= 3) {
                    if (selectedButton < 2) selectedButton = 4;
                    else selectedButton = 6;
                }
                else if (selectedButton >= 4 && selectedButton <= 6) {
                    if (selectedButton == 4) selectedButton = 0;
                    else if (selectedButton == 5) selectedButton = 1;
                    else if (selectedButton == 6) selectedButton = 2;
                }
            }
            moved = true;
        }

        if (IsGamepadButtonPressed(gamepad1.gamepadNumber, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
            if (currentMenu == 0) {
                if (selectedButton == 0) selectedButton = 1;
                else if (selectedButton >= 1 && selectedButton <= 3) selectedButton = 0;
            }
            else {
                if (selectedButton >= 0 && selectedButton <= 3) {
                    if (selectedButton < 2) selectedButton = 4;
                    else selectedButton = 6;
                }
                else if (selectedButton >= 4 && selectedButton <= 6) {
                    if (selectedButton == 4) selectedButton = 0;
                    else if (selectedButton == 5) selectedButton = 1;
                    else if (selectedButton == 6) selectedButton = 2;
                }
            }
            moved = true;
        }

        // Навигация влево/вправо
        if (IsGamepadButtonPressed(gamepad1.gamepadNumber, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
            if (currentMenu == 0) {
                if (selectedButton >= 1 && selectedButton <= 3) {
                    selectedButton--;
                    if (selectedButton < 1) selectedButton = 3;
                }
            }
            else {
                if (selectedButton >= 0 && selectedButton <= 3) {
                    if (selectedButton == 0 || selectedButton == 2) selectedButton++;
                    else if (selectedButton == 1 || selectedButton == 3) selectedButton--;
                }
                else if (selectedButton >= 4 && selectedButton <= 6) {
                    if (selectedButton == 4) selectedButton = 6;
                    else if (selectedButton == 5) selectedButton = 4;
                    else if (selectedButton == 6) selectedButton = 5;
                }
            }
            moved = true;
        }

        if (IsGamepadButtonPressed(gamepad1.gamepadNumber, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
            if (currentMenu == 0) {
                if (selectedButton >= 1 && selectedButton <= 3) {
                    selectedButton++;
                    if (selectedButton > 3) selectedButton = 1;
                }
            }
            else {
                if (selectedButton >= 0 && selectedButton <= 3) {
                    if (selectedButton == 0 || selectedButton == 2) selectedButton++;
                    else if (selectedButton == 1 || selectedButton == 3) selectedButton--;
                }
                else if (selectedButton >= 4 && selectedButton <= 6) {
                    if (selectedButton == 4) selectedButton = 5;
                    else if (selectedButton == 5) selectedButton = 6;
                    else if (selectedButton == 6) selectedButton = 4;
                }
            }
            moved = true;
        }

        // Кнопка A - выбор
        if (IsGamepadButtonPressed(gamepad1.gamepadNumber, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            if (currentMenu == 0) {
                if (selectedButton >= 0 && selectedButton < static_cast<int>(mainMenuButtons.size())) {
                    MenuButton btn = mainMenuButtons[selectedButton];

                    if (btn.id == 0) {
                        currentMenu = 1;
                        InitChooseModeMenu();
                        selectedButton = 0;
                    }
                    else if (btn.id == 1) {
                        showTopRating = true;
                    }
                    else if (btn.id == 2) {
                        // How to Play
                    }
                    else if (btn.id == 3) {
                        gameMode = 5;
                    }
                }
            }
            else {
                if (selectedButton >= 0 && selectedButton < static_cast<int>(chooseModeButtons.size())) {
                    MenuButton btn = chooseModeButtons[selectedButton];

                    if (btn.id >= 4 && btn.id <= 7) {
                        selectedGameMode = btn.id - 3;
                    }
                    else if (btn.id == 8) {
                        if (selectedGameMode >= 1) {
                            gameMode = selectedGameMode;
                        }
                    }
                    else if (btn.id == 9) {
                        showTopRating = true;
                    }
                    else if (btn.id == 10) {
                        currentMenu = 0;
                        InitMainMenu();
                        selectedButton = 0;
                    }
                }
            }
            moved = true;
        }

        // Кнопка B - назад
        if (IsGamepadButtonPressed(gamepad1.gamepadNumber, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) && currentMenu == 1) {
            currentMenu = 0;
            InitMainMenu();
            selectedButton = 0;
        }
    }

    if (moved) {
        gamepadMenuCooldown = 0.2f;
    }
}

// Обработка мыши для меню
void UpdateMenuMouse() {
    Vector2 mousePoint = GetMousePosition();

    if (currentMenu == 0) {
        for (size_t i = 0; i < mainMenuButtons.size(); i++) {
            mainMenuButtons[i].hovered = CheckCollisionPointRec(mousePoint, mainMenuButtons[i].rect);
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            for (size_t i = 0; i < mainMenuButtons.size(); i++) {
                if (CheckCollisionPointRec(mousePoint, mainMenuButtons[i].rect)) {
                    MenuButton btn = mainMenuButtons[i];

                    if (btn.id == 0) {
                        currentMenu = 1;
                        InitChooseModeMenu();
                    }
                    else if (btn.id == 1) {
                        showTopRating = true;
                    }
                    else if (btn.id == 2) {
                        // How to Play
                    }
                    else if (btn.id == 3) {
                        gameMode = 5;
                    }
                    break;
                }
            }
        }
    }
    else {
        for (size_t i = 0; i < chooseModeButtons.size(); i++) {
            chooseModeButtons[i].hovered = CheckCollisionPointRec(mousePoint, chooseModeButtons[i].rect);
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            for (size_t i = 0; i < chooseModeButtons.size(); i++) {
                if (CheckCollisionPointRec(mousePoint, chooseModeButtons[i].rect)) {
                    MenuButton btn = chooseModeButtons[i];

                    if (btn.id >= 4 && btn.id <= 7) {
                        selectedGameMode = btn.id - 3;
                    }
                    else if (btn.id == 8) {
                        if (selectedGameMode >= 1) {
                            gameMode = selectedGameMode;
                        }
                    }
                    else if (btn.id == 9) {
                        showTopRating = true;
                    }
                    else if (btn.id == 10) {
                        currentMenu = 0;
                        InitMainMenu();
                    }
                    break;
                }
            }
        }
    }
}

// Для геймпадов 
void CheckGamepads() {
    gamepad1.connected = IsGamepadAvailable(gamepad1.gamepadNumber);
    gamepad2.connected = IsGamepadAvailable(gamepad2.gamepadNumber);

    if (gamepad1.connected && IsGamepadButtonPressed(gamepad1.gamepadNumber, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
        gamepad1.useLeftStick = !gamepad1.useLeftStick;
    }

    if (gamepad2.connected && IsGamepadButtonPressed(gamepad2.gamepadNumber, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
        gamepad2.useLeftStick = !gamepad2.useLeftStick;
    }
}

void MoveRectangleWithGamepad(Rectangle& rec, GamepadController& gamepad, int playerSide = 0) {
    if (!gamepad.connected) return;

    float moveSpeed = 20.0f * scaleX;
    Vector2 stickAxis = { 0, 0 };

    if (gamepad.useLeftStick) {
        stickAxis.x = GetGamepadAxisMovement(gamepad.gamepadNumber, GAMEPAD_AXIS_LEFT_X);
    }
    else {
        stickAxis.x = GetGamepadAxisMovement(gamepad.gamepadNumber, GAMEPAD_AXIS_RIGHT_X);
    }

    if (fabs(stickAxis.x) > 0.2f) {
        rec.x += stickAxis.x * moveSpeed;
    }

    if (IsGamepadButtonDown(gamepad.gamepadNumber, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
        rec.x -= moveSpeed;
    }
    if (IsGamepadButtonDown(gamepad.gamepadNumber, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
        rec.x += moveSpeed;
    }

    if (gameMode == 4) {
        if (playerSide == 1) {
            if (rec.x < 0) rec.x = 0;
            if (rec.x + rec.width > screenWidth / 2) rec.x = screenWidth / 2 - rec.width;
        }
        else if (playerSide == 2) {
            if (rec.x < screenWidth / 2) rec.x = screenWidth / 2;
            if (rec.x + rec.width > screenWidth) rec.x = screenWidth - rec.width;
        }
    }
    else {
        if (rec.x + rec.width > screenWidth) rec.x = screenWidth - rec.width;
        if (rec.x < 0) rec.x = 0;
    }
}

void DrawGamepadInfo() {
    if (gameMode == 4) {
        int infoFontSize = ScaleInt(18);

        if (gamepad1.connected) {
            DrawText("Gamepad 1: Connected", ScaleInt(20), screenHeight - ScaleInt(60), infoFontSize, GREEN);
            DrawText(TextFormat("Stick: %s", gamepad1.useLeftStick ? "LEFT" : "RIGHT"), ScaleInt(20), screenHeight - ScaleInt(40), infoFontSize - 2, WHITE);
            DrawText("Press X to switch", ScaleInt(20), screenHeight - ScaleInt(20), infoFontSize - 2, YELLOW);
        }
        else {
            DrawText("Gamepad 1: Not Connected", ScaleInt(40), screenHeight - ScaleInt(60), infoFontSize, RED);
            DrawText("Use A/D keys", ScaleInt(20), screenHeight - ScaleInt(20), infoFontSize - 2, WHITE);
        }

        if (gamepad2.connected) {
            DrawText("Gamepad 2: Connected", screenWidth - ScaleInt(200), screenHeight - ScaleInt(60), infoFontSize, GREEN);
            DrawText(TextFormat("Stick: %s", gamepad2.useLeftStick ? "LEFT" : "RIGHT"), screenWidth - ScaleInt(200), screenHeight - ScaleInt(40), infoFontSize - 2, WHITE);
            DrawText("Press X to switch", screenWidth - ScaleInt(200), screenHeight - ScaleInt(20), infoFontSize - 2, YELLOW);
        }
        else {
            DrawText("Gamepad 2: Not Connected", screenWidth - ScaleInt(260), screenHeight - ScaleInt(60), infoFontSize, RED);
            DrawText("Use Arrow keys", screenWidth - ScaleInt(200), screenHeight - ScaleInt(20), infoFontSize - 2, WHITE);
        }
    }
}

// Score system 
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

void MoveRectangle(Rectangle& rec, bool useArrowKeys, int playerSide = 0) {
    float moveSpeed = 20.0f * scaleX;

    if (gameMode == 4) {
        if (playerSide == 1 && player1.bonuses.speedBoostActive) {
            moveSpeed = 25.0f * scaleX;
        }
        else if (playerSide == 2 && player2.bonuses.speedBoostActive) {
            moveSpeed = 25.0f * scaleX;
        }
    }
    else {
        if (speedBoostActive) {
            moveSpeed = 25.0f * scaleX;
        }
    }

    if (gameMode == 4) {
        if (playerSide == 1 && player1.bonuses.slowMotionActive) {
            moveSpeed = 20.0f * scaleX;
        }
        else if (playerSide == 2 && player2.bonuses.slowMotionActive) {
            moveSpeed = 20.0f * scaleX;
        }
    }
    else {
        if (slowMotionActive) {
            moveSpeed = 20.0f * scaleX;
        }
    }

    if ((useArrowKeys && IsKeyDown(KEY_LEFT)) || (!useArrowKeys && IsKeyDown(KEY_A))) {
        rec.x -= moveSpeed;
    }
    if ((useArrowKeys && IsKeyDown(KEY_RIGHT)) || (!useArrowKeys && IsKeyDown(KEY_D))) {
        rec.x += moveSpeed;
    }

    if (gameMode == 4) {
        if (playerSide == 1) {
            if (rec.x < 0) rec.x = 0;
            if (rec.x + rec.width > screenWidth / 2) rec.x = screenWidth / 2 - rec.width;
        }
        else if (playerSide == 2) {
            if (rec.x < screenWidth / 2) rec.x = screenWidth / 2;
            if (rec.x + rec.width > screenWidth) rec.x = screenWidth - rec.width;
        }
    }
    else {
        if (rec.x + rec.width > screenWidth) rec.x = screenWidth - rec.width;
        if (rec.x < 0) rec.x = 0;
    }
}

Fruit CreateFruit(int playerSide = 0) {
    Fruit fruit;
    fruitSize = ScaleInt(60);

    if (playerSide == 1) {
        fruit.rect = { static_cast<float>(GetRandomValue(ScaleInt(50), screenWidth / 2 - ScaleInt(100))), -ScaleFloat(50), static_cast<float>(fruitSize), static_cast<float>(fruitSize) };
    }
    else if (playerSide == 2) {
        fruit.rect = { static_cast<float>(GetRandomValue(screenWidth / 2 + ScaleInt(50), screenWidth - ScaleInt(100))), -ScaleFloat(50), static_cast<float>(fruitSize), static_cast<float>(fruitSize) };
    }
    else {
        fruit.rect = { static_cast<float>(GetRandomValue(ScaleInt(50), screenWidth - ScaleInt(100))), -ScaleFloat(50), static_cast<float>(fruitSize), static_cast<float>(fruitSize) };
    }

    int randomChance = GetRandomValue(0, 100);

    if (randomChance < 8) {
        if (gameMode == 2) {
            fruit.type = GetRandomValue(6, 8);
        }
        else if (gameMode == 3) {
            fruit.type = GetRandomValue(9, 10);
        }
        else {
            fruit.type = GetRandomValue(6, 10);
        }
    }
    else if (randomChance < 35) {
        fruit.type = GetRandomValue(4, 5);
    }
    else {
        fruit.type = GetRandomValue(0, 3);
    }

    fruit.active = true;
    float baseSpeed = fruitSpeed + GetRandomValue(0, 2);

    if (gameMode == 4) {
        if (playerSide == 1 && player1.bonuses.freezeActive) baseSpeed = 0.5f;
        else if (playerSide == 2 && player2.bonuses.freezeActive) baseSpeed = 0.5f;
        else {
            if (player1.bonuses.slowMotionActive || player2.bonuses.slowMotionActive) baseSpeed *= 0.4f;
            if (player1.bonuses.speedBoostActive || player2.bonuses.speedBoostActive) baseSpeed *= 1.3f;
        }
    }
    else {
        if (slowMotionActive) baseSpeed *= 0.4f;
        if (speedBoostActive) baseSpeed *= 1.3f;
        if (freezeActive) baseSpeed = 0.5f;
    }

    fruit.speed = baseSpeed * scaleY;

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
    case 11: fruit.color = GOLD; break;
    case 12: fruit.color = ORANGE; break;
    }

    fruit.effectDuration = 5.0f;
    return fruit;
}

void UpdateBonusEffects() {
    if (slowMotionActive) {
        slowMotionTimer -= GetFrameTime();
        if (slowMotionTimer <= 0) slowMotionActive = false;
    }
    if (doublePointsActive) {
        doublePointsTimer -= GetFrameTime();
        if (doublePointsTimer <= 0) doublePointsActive = false;
    }
    if (freezeActive) {
        freezeTimer -= GetFrameTime();
        if (freezeTimer <= 0) freezeActive = false;
    }
    if (speedBoostActive) {
        speedBoostTimer -= GetFrameTime();
        if (speedBoostTimer <= 0) speedBoostActive = false;
    }

    if (gameMode == 4) {
        if (player1.isAlive) {
            if (player1.bonuses.slowMotionActive) {
                player1.bonuses.slowMotionTimer -= GetFrameTime();
                if (player1.bonuses.slowMotionTimer <= 0) player1.bonuses.slowMotionActive = false;
            }
            if (player1.bonuses.doublePointsActive) {
                player1.bonuses.doublePointsTimer -= GetFrameTime();
                if (player1.bonuses.doublePointsTimer <= 0) player1.bonuses.doublePointsActive = false;
            }
            if (player1.bonuses.freezeActive) {
                player1.bonuses.freezeTimer -= GetFrameTime();
                if (player1.bonuses.freezeTimer <= 0) player1.bonuses.freezeActive = false;
            }
            if (player1.bonuses.speedBoostActive) {
                player1.bonuses.speedBoostTimer -= GetFrameTime();
                if (player1.bonuses.speedBoostTimer <= 0) player1.bonuses.speedBoostActive = false;
            }
        }

        if (player2.isAlive) {
            if (player2.bonuses.slowMotionActive) {
                player2.bonuses.slowMotionTimer -= GetFrameTime();
                if (player2.bonuses.slowMotionTimer <= 0) player2.bonuses.slowMotionActive = false;
            }
            if (player2.bonuses.doublePointsActive) {
                player2.bonuses.doublePointsTimer -= GetFrameTime();
                if (player2.bonuses.doublePointsTimer <= 0) player2.bonuses.doublePointsActive = false;
            }
            if (player2.bonuses.freezeActive) {
                player2.bonuses.freezeTimer -= GetFrameTime();
                if (player2.bonuses.freezeTimer <= 0) player2.bonuses.freezeActive = false;
            }
            if (player2.bonuses.speedBoostActive) {
                player2.bonuses.speedBoostTimer -= GetFrameTime();
                if (player2.bonuses.speedBoostTimer <= 0) player2.bonuses.speedBoostActive = false;
            }
        }
    }
}

void ApplyBonusEffect(int bonusType, int playerNumber = 0) {
    if (gameMode == 4) {
        if (playerNumber == 1 && player1.isAlive) {
            switch (bonusType) {
            case 6:
                player1.bonuses.slowMotionActive = true;
                player1.bonuses.slowMotionTimer = 7.0f;
                break;
            case 7:
                player1.bonuses.doublePointsActive = true;
                player1.bonuses.doublePointsTimer = 10.0f;
                break;
            case 8:
                if (player1.lives < 5) player1.lives++;
                break;
            case 9:
                gameTime += 10.0f;
                break;
            case 10:
                player1.bonuses.speedBoostActive = true;
                player1.bonuses.speedBoostTimer = 6.0f;
                break;
            }
        }
        else if (playerNumber == 2 && player2.isAlive) {
            switch (bonusType) {
            case 6:
                player2.bonuses.slowMotionActive = true;
                player2.bonuses.slowMotionTimer = 7.0f;
                break;
            case 7:
                player2.bonuses.doublePointsActive = true;
                player2.bonuses.doublePointsTimer = 10.0f;
                break;
            case 8:
                if (player2.lives < 5) player2.lives++;
                break;
            case 9:
                gameTime += 10.0f;
                break;
            case 10:
                player2.bonuses.speedBoostActive = true;
                player2.bonuses.speedBoostTimer = 6.0f;
                break;
            }
        }
    }
    else {
        switch (bonusType) {
        case 6:
            slowMotionActive = true;
            slowMotionTimer = 7.0f;
            break;
        case 7:
            doublePointsActive = true;
            doublePointsTimer = 10.0f;
            break;
        case 8:
            if (player1.lives < 5) player1.lives++;
            break;
        case 9:
            gameTime += 10.0f;
            break;
        case 10:
            speedBoostActive = true;
            speedBoostTimer = 6.0f;
            break;
        }
    }
}

void UpdateFruits(std::vector<Fruit>& fruits) {
    for (auto& fruit : fruits) {
        if (fruit.active) {
            float currentSpeed = fruit.speed;

            if (gameMode == 4) {
                if (fruit.rect.x < screenWidth / 2 && player1.bonuses.freezeActive) {
                    currentSpeed = 0.5f;
                }
                else if (fruit.rect.x >= screenWidth / 2 && player2.bonuses.freezeActive) {
                    currentSpeed = 0.5f;
                }
            }
            else {
                if (freezeActive) {
                    currentSpeed = 0.5f;
                }
            }

            fruit.rect.y += currentSpeed;

            bool collision = false;

            if (gameMode == 4) {
                if (player1.isAlive && CheckCollisionRecs(fruit.rect, player1.basket)) {
                    collision = true;
                    if (fruit.type <= 3) {
                        int points = 100;
                        if (player1.bonuses.doublePointsActive) points *= 2;
                        player1.score += points;
                    }
                    else if (fruit.type <= 5) {
                        player1.lives--;
                        if (player1.lives <= 0) player1.isAlive = false;
                    }
                    else {
                        ApplyBonusEffect(fruit.type, 1);
                        player1.score += 200;
                    }
                }
                else if (player2.isAlive && CheckCollisionRecs(fruit.rect, player2.basket)) {
                    collision = true;
                    if (fruit.type <= 3) {
                        int points = 100;
                        if (player2.bonuses.doublePointsActive) points *= 2;
                        player2.score += points;
                    }
                    else if (fruit.type <= 5) {
                        player2.lives--;
                        if (player2.lives <= 0) player2.isAlive = false;
                    }
                    else {
                        ApplyBonusEffect(fruit.type, 2);
                        player2.score += 200;
                    }
                }
            }
            else {
                if (CheckCollisionRecs(fruit.rect, player1.basket)) {
                    collision = true;
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
                }
            }

            if (collision) {
                fruit.active = false;
            }

            if (fruit.rect.y > screenHeight) {
                if (fruit.type <= 3) {
                    if (gameMode == 4) {
                        if (fruit.rect.x < screenWidth / 2 && player1.isAlive) {
                            player1.missedFruits++;
                            if (player1.missedFruits >= 10) player1.isAlive = false;
                        }
                        else if (fruit.rect.x >= screenWidth / 2 && player2.isAlive) {
                            player2.missedFruits++;
                            if (player2.missedFruits >= 10) player2.isAlive = false;
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

    for (int i = static_cast<int>(fruits.size()) - 1; i >= 0; i--) {
        if (!fruits[i].active) {
            fruits.erase(fruits.begin() + i);
        }
    }
}

void DrawFruits(const std::vector<Fruit>& fruits) {
    for (const auto& fruit : fruits) {
        if (fruit.active) {
            Texture2D textureToDraw = Fruit0Texture;
            switch (fruit.type) {
            case 0: textureToDraw = Fruit0Texture; break;
            case 1: textureToDraw = Fruit1Texture; break;
            case 2: textureToDraw = Fruit2Texture; break;
            case 3: textureToDraw = Fruit3Texture; break;
            case 4: textureToDraw = Fruit4Texture; break;
            case 5: textureToDraw = Fruit5Texture; break;
            case 6: textureToDraw = Fruit6Texture; break;
            case 7: textureToDraw = Fruit7Texture; break;
            case 8: textureToDraw = Fruit8Texture; break;
            case 9: textureToDraw = Fruit9Texture; break;
            case 10: textureToDraw = Fruit10Texture; break;
            case 11: textureToDraw = Fruit11Texture; break;
            case 12: textureToDraw = Fruit12Texture; break;
            }
            DrawTexture(textureToDraw, fruit.rect.x, fruit.rect.y, RAYWHITE);
        }
    }
}

void ResetGame() {
    gameOver = false;
    gameTime = 120.0f;
    fruitSpeed = 2.0f;
    spawnRate = 1.0f;
    gameStarted = false;

    UpdateScale();

    int basketWidth = ScaleInt(100);
    int basketHeight = ScaleInt(60);
    // Корзины должны быть в самом низу экрана
    int basketYPos = screenHeight - basketHeight - ScaleIntY(50);

    player1.basket = {
        screenWidth / 4.0f - basketWidth / 2,
        static_cast<float>(basketYPos),
        static_cast<float>(basketWidth),
        static_cast<float>(basketHeight)
    };
    player1.score = 0;
    player1.lives = 3;
    player1.missedFruits = 0;
    player1.name = "Player 1";
    player1.color = BLUE;
    player1.isAlive = true;
    player1.bonuses = PlayerBonusEffects();

    player2.basket = {
        3 * screenWidth / 4.0f - basketWidth / 2,
        static_cast<float>(basketYPos),
        static_cast<float>(basketWidth),
        static_cast<float>(basketHeight)
    };
    player2.score = 0;
    player2.lives = 3;
    player2.missedFruits = 0;
    player2.name = "Player 2";
    player2.color = RED;
    player2.isAlive = true;
    player2.bonuses = PlayerBonusEffects();

    slowMotionActive = false;
    doublePointsActive = false;
    freezeActive = false;
    speedBoostActive = false;
    slowMotionTimer = 0.0f;
    doublePointsTimer = 0.0f;
    freezeTimer = 0.0f;
    speedBoostTimer = 0.0f;
}

void ToggleFullscreen() {
    if (fullscreen) {
        // Переключаемся в оконный режим
        SetWindowSize(baseWidth, baseHeight);
        SetWindowPosition(GetMonitorWidth(0) / 2 - baseWidth / 2, GetMonitorHeight(0) / 2 - baseHeight / 2);
        screenWidth = baseWidth;
        screenHeight = baseHeight;
        fullscreen = false;
    }
    else {
        // Переключаемся в полноэкранный режим
        int monitor = GetCurrentMonitor();
        int monitorWidth = GetMonitorWidth(monitor);
        int monitorHeight = GetMonitorHeight(monitor);
        SetWindowSize(monitorWidth, monitorHeight);
        SetWindowPosition(0, 0);
        screenWidth = monitorWidth;
        screenHeight = monitorHeight;
        fullscreen = true;
    }

    UpdateScale();
    LoadMenuTextures();
    if (currentMenu == 0) {
        InitMainMenu();
    }
    else {
        InitChooseModeMenu();
    }

    // Перезагружаем текстуры с новыми размерами
    fruitSize = ScaleInt(60);

    // Перезагрузка текстур фруктов
    Image image = LoadImage("apple.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit0Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("orange.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit1Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("watermelon.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit2Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("pear.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit3Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("bad_fruit1.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit4Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("bad_fruit2.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit5Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("banana.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit6Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("cherry.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit7Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("golden_apple.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit8Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("golden_watermelon.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit9Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("bad_fruit3.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit10Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("golden_cherry.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit11Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("kiwi.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit12Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    // Перезагрузка текстуры корзины
    image = LoadImage("basket.png");
    if (image.data != NULL) {
        ImageResize(&image, ScaleInt(100), ScaleInt(60));
        BasketTexture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    // Сброс игры с новыми размерами
    ResetGame();
}

int main(void) {
    // Сначала создаем окно в оконном режиме с базовым разрешением
    screenWidth = baseWidth;
    screenHeight = baseHeight;

    // Создаем окно без флага полноэкранного режима
    InitWindow(screenWidth, screenHeight, "FruityDrop");

    // Проверяем, успешно ли создано окно
    if (!IsWindowReady()) {
        // Если окно не создано, выходим
        return -1;
    }

    // Изначально устанавливаем оконный режим
    fullscreen = false;

    SetTargetFPS(60);

    UpdateScale();

    LoadPlayerScores();
    ResetGame();
    std::vector<Fruit> fruits;

    // Сначала загружаем текстуры меню
    LoadMenuTextures();
    InitMainMenu();

    // Загружаем текстуры фруктов и корзины
    fruitSize = ScaleInt(60);

    // Загрузка текстур фруктов с масштабированием
    Image image = LoadImage("apple.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit0Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("orange.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit1Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("watermelon.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit2Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("pear.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit3Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("bad_fruit1.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit4Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("bad_fruit2.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit5Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("banana.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit6Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("cherry.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit7Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("golden_apple.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit8Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("golden_watermelon.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit9Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("bad_fruit3.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit10Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("golden_cherry.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit11Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    image = LoadImage("kiwi.png");
    if (image.data != NULL) {
        ImageResize(&image, fruitSize, fruitSize);
        Fruit12Texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    // Загрузка текстуры корзины
    image = LoadImage("basket.png");
    if (image.data != NULL) {
        ImageResize(&image, ScaleInt(100), ScaleInt(60));
        BasketTexture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    while (!WindowShouldClose()) {
        if (gameMode == 5) break;

        CheckGamepads();

        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        BeginDrawing();

        if (showTopRating) {
            DrawTopRating();
            if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ESCAPE)) {
                showTopRating = false;
            }
            EndDrawing();
            continue;
        }

        if (gameMode == 0) {
            UpdateMenuNavigation();
            UpdateMenuMouse();

            if (currentMenu == 0) {
                DrawMainMenu();
            }
            else {
                DrawChooseModeMenu();
            }

            EndDrawing();
            continue;
        }

        if (!gameOver) {
            if (!gameStarted) {
                // Очищаем экран перед отрисовкой
                ClearBackground(BLACK);

                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.3f));
                if (gameMode == 4) {
                    DrawRectangle(0, 0, screenWidth / 2, screenHeight, Fade(SKYBLUE, 0.1f));
                    DrawRectangle(screenWidth / 2, 0, screenWidth / 2, screenHeight, Fade(LIGHTGRAY, 0.1f));
                    DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, DARKGRAY);
                }

                DrawTexture(BasketTexture, player1.basket.x, player1.basket.y, player1.color);
                if (gameMode == 4) {
                    DrawTexture(BasketTexture, player2.basket.x, player2.basket.y, player2.color);
                }

                int instructionFontSize = ScaleInt(25);
                const char* modeName = "";

                switch (gameMode) {
                case 1: modeName = "CLASSIC MODE"; break;
                case 2: modeName = "SURVIVAL MODE"; break;
                case 3: modeName = "TIME ATTACK MODE"; break;
                case 4: modeName = "TWO PLAYERS MODE"; break;
                }

                DrawText(modeName, screenWidth / 2 - MeasureText(modeName, instructionFontSize + 10) / 2,
                    screenHeight / 2 - 100, instructionFontSize + 10, YELLOW);

                DrawText("Catch good fruits, avoid bad ones!",
                    screenWidth / 2 - MeasureText("Catch good fruits, avoid bad ones!", instructionFontSize) / 2,
                    screenHeight / 2 - 40, instructionFontSize, WHITE);
                DrawText("Collect bonus fruits for special effects!",
                    screenWidth / 2 - MeasureText("Collect bonus fruits for special effects!", instructionFontSize) / 2,
                    screenHeight / 2, instructionFontSize, WHITE);
                DrawText("Press SPACE to start the game!",
                    screenWidth / 2 - MeasureText("Press SPACE to start the game!", instructionFontSize + 5) / 2,
                    screenHeight / 2 + 60, instructionFontSize + 5, GREEN);

                if (gameMode == 4) {
                    DrawText("PLAYER 1", ScaleInt(20), ScaleInt(20), instructionFontSize, BLUE);
                    DrawText("A/D keys to move", ScaleInt(20), ScaleInt(50), instructionFontSize - 5, DARKBLUE);
                    if (gamepad1.connected) {
                        DrawText("Gamepad: LEFT/RIGHT stick", ScaleInt(30), ScaleInt(75), instructionFontSize - 5, GREEN);
                        DrawText("X button: switch stick", ScaleInt(30), ScaleInt(100), instructionFontSize - 5, YELLOW);
                    }

                    DrawText("PLAYER 2", screenWidth - ScaleInt(200), ScaleInt(20), instructionFontSize, RED);
                    DrawText("Arrow keys to move", screenWidth - ScaleInt(200), ScaleInt(50), instructionFontSize - 5, DARKBLUE);
                    if (gamepad2.connected) {
                        DrawText("Gamepad: LEFT/RIGHT stick", screenWidth - ScaleInt(190), ScaleInt(75), instructionFontSize - 5, GREEN);
                        DrawText("X button: switch stick", screenWidth - ScaleInt(190), ScaleInt(100), instructionFontSize - 5, YELLOW);
                    }
                }
                else {
                    DrawText("Controls: A/D or Arrow keys to move",
                        screenWidth / 2 - MeasureText("Controls: A/D or Arrow keys to move", instructionFontSize) / 2,
                        screenHeight / 2 + 120, instructionFontSize, WHITE);
                    if (gamepad1.connected) {
                        DrawText("Gamepad also supported",
                            screenWidth / 2 - MeasureText("Gamepad also supported", instructionFontSize) / 2,
                            screenHeight / 2 + 160, instructionFontSize, GREEN);
                    }
                }

                bool startPressed = IsKeyPressed(KEY_SPACE);
                if (gamepad1.connected && IsGamepadButtonPressed(gamepad1.gamepadNumber, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                    startPressed = true;
                }
                if (gameMode == 4 && gamepad2.connected && IsGamepadButtonPressed(gamepad2.gamepadNumber, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                    startPressed = true;
                }

                if (startPressed) {
                    gameStarted = true;
                }

                EndDrawing();
                continue;
            }

            UpdateBonusEffects();

            if (gameMode == 4) {
                if (player1.isAlive) {
                    if (gamepad1.connected) {
                        MoveRectangleWithGamepad(player1.basket, gamepad1, 1);
                    }
                    else {
                        MoveRectangle(player1.basket, false, 1);
                    }
                }

                if (player2.isAlive) {
                    if (gamepad2.connected) {
                        MoveRectangleWithGamepad(player2.basket, gamepad2, 2);
                    }
                    else {
                        MoveRectangle(player2.basket, true, 2);
                    }
                }
            }
            else {
                if (gamepad1.connected) {
                    MoveRectangleWithGamepad(player1.basket, gamepad1);
                }
                else {
                    MoveRectangle(player1.basket, false);
                }
            }

            if (gameMode == 3 || gameMode == 4) {
                gameTime -= GetFrameTime();
                if (gameTime <= 0) {
                    gameOver = true;
                }
            }

            if (gameMode == 4) {
                if (gameTime <= 0 || (!player1.isAlive && !player2.isAlive)) {
                    gameOver = true;
                }
            }
            else if (gameMode == 1 || gameMode == 2 || gameMode == 3) {
                if (player1.lives <= 0 || player1.missedFruits >= 10) {
                    gameOver = true;
                }
            }

            timeSinceLastSpawn += GetFrameTime();
            if (timeSinceLastSpawn >= spawnRate) {
                if (gameMode == 4) {
                    if (player1.isAlive) fruits.push_back(CreateFruit(1));
                    if (player2.isAlive) fruits.push_back(CreateFruit(2));
                }
                else {
                    fruits.push_back(CreateFruit());
                }
                timeSinceLastSpawn = 0.0f;

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

            // Очищаем экран перед отрисовкой игрового поля
            ClearBackground(BLACK);
            DrawTexture(backgroundTexture, 0, 0, WHITE);

            if (gameMode == 4) {
                DrawRectangle(0, 0, screenWidth / 2, screenHeight, Fade(SKYBLUE, 0.1f));
                DrawRectangle(screenWidth / 2, 0, screenWidth / 2, screenHeight, Fade(LIGHTGRAY, 0.1f));
                DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, DARKGRAY);

                if (player1.isAlive) DrawTexture(BasketTexture, player1.basket.x, player1.basket.y, player1.color);
                if (player2.isAlive) DrawTexture(BasketTexture, player2.basket.x, player2.basket.y, player2.color);
            }
            else {
                DrawTexture(BasketTexture, player1.basket.x, player1.basket.y, ORANGE);
            }

            DrawFruits(fruits);

            int uiFontSize = ScaleInt(25);
            int smallFontSize = ScaleInt(20);

            // Фон для интерфейса - теперь вверху экрана
            DrawRectangle(0, 0, screenWidth, ScaleIntY(200), Fade(BLACK, 0.5f));

            if (gameMode == 4) {
                int playerInfoY = ScaleIntY(20);
                int lineHeight = ScaleIntY(35);

                // Игрок 1 (слева)
                DrawText("PLAYER 1", ScaleInt(20), playerInfoY, uiFontSize, BLUE);
                DrawText(TextFormat("Score: %d", player1.score), ScaleInt(20), playerInfoY + lineHeight, smallFontSize, WHITE);
                DrawText(TextFormat("Lives: %d", player1.lives), ScaleInt(20), playerInfoY + 2 * lineHeight, smallFontSize, player1.isAlive ? RED : GRAY);
                DrawText(TextFormat("Missed: %d/10", player1.missedFruits), ScaleInt(20), playerInfoY + 3 * lineHeight, smallFontSize, WHITE);
                if (!player1.isAlive) DrawText("ELIMINATED!", ScaleInt(20), playerInfoY + 4 * lineHeight, smallFontSize - 5, RED);

                // Игрок 2 (справа)
                DrawText("PLAYER 2", screenWidth - ScaleInt(150), playerInfoY, uiFontSize, RED);
                DrawText(TextFormat("Score: %d", player2.score), screenWidth - ScaleInt(150), playerInfoY + lineHeight, smallFontSize, WHITE);
                DrawText(TextFormat("Lives: %d", player2.lives), screenWidth - ScaleInt(150), playerInfoY + 2 * lineHeight, smallFontSize, player2.isAlive ? RED : GRAY);
                DrawText(TextFormat("Missed: %d/10", player2.missedFruits), screenWidth - ScaleInt(150), playerInfoY + 3 * lineHeight, smallFontSize, WHITE);
                if (!player2.isAlive) DrawText("ELIMINATED!", screenWidth - ScaleInt(150), playerInfoY + 4 * lineHeight, smallFontSize - 5, RED);

                // Время по центру
                DrawText(TextFormat("Time: %d", static_cast<int>(gameTime)), screenWidth / 2 - ScaleInt(40), playerInfoY, uiFontSize, YELLOW);

                // Бонусы игроков
                int bonusY1 = playerInfoY + 5 * lineHeight;
                int bonusY2 = playerInfoY + 5 * lineHeight;

                if (player1.isAlive) {
                    if (player1.bonuses.slowMotionActive) {
                        DrawText("Slow Motion!", ScaleInt(20), bonusY1, smallFontSize - 5, BLUE);
                        bonusY1 += ScaleIntY(20);
                    }
                    if (player1.bonuses.doublePointsActive) {
                        DrawText("Double Points!", ScaleInt(20), bonusY1, smallFontSize - 5, GOLD);
                        bonusY1 += ScaleIntY(20);
                    }
                    if (player1.bonuses.speedBoostActive) {
                        DrawText("Speed Boost!", ScaleInt(20), bonusY1, smallFontSize - 5, ORANGE);
                        bonusY1 += ScaleIntY(20);
                    }
                    if (player1.bonuses.freezeActive) {
                        DrawText("Freeze!", ScaleInt(20), bonusY1, smallFontSize - 5, SKYBLUE);
                        bonusY1 += ScaleIntY(20);
                    }
                }

                if (player2.isAlive) {
                    if (player2.bonuses.slowMotionActive) {
                        DrawText("Slow Motion!", screenWidth - ScaleInt(150), bonusY2, smallFontSize - 5, BLUE);
                        bonusY2 += ScaleIntY(20);
                    }
                    if (player2.bonuses.doublePointsActive) {
                        DrawText("Double Points!", screenWidth - ScaleInt(150), bonusY2, smallFontSize - 5, GOLD);
                        bonusY2 += ScaleIntY(20);
                    }
                    if (player2.bonuses.speedBoostActive) {
                        DrawText("Speed Boost!", screenWidth - ScaleInt(150), bonusY2, smallFontSize - 5, ORANGE);
                        bonusY2 += ScaleIntY(20);
                    }
                    if (player2.bonuses.freezeActive) {
                        DrawText("Freeze!", screenWidth - ScaleInt(150), bonusY2, smallFontSize - 5, SKYBLUE);
                        bonusY2 += ScaleIntY(20);
                    }
                }

                DrawGamepadInfo();

                // Нижний текст - теперь выше и не выходит за пределы экрана
                int bottomTextY = screenHeight - ScaleIntY(80);
                if (player1.isAlive && player2.isAlive) {
                    const char* text = "";
                    Color textColor = WHITE;
                    if (player1.score > player2.score) {
                        text = "PLAYER 1 LEADING!";
                        textColor = BLUE;
                    }
                    else if (player2.score > player1.score) {
                        text = "PLAYER 2 LEADING!";
                        textColor = RED;
                    }
                    else {
                        text = "TIE!";
                        textColor = PURPLE;
                    }
                    int textWidth = MeasureText(text, smallFontSize);
                    DrawText(text, screenWidth / 2 - textWidth / 2, bottomTextY, smallFontSize, textColor);
                }
                else if (player1.isAlive && !player2.isAlive) {
                    const char* text = "PLAYER 1 WINS BY ELIMINATION!";
                    int textWidth = MeasureText(text, smallFontSize);
                    DrawText(text, screenWidth / 2 - textWidth / 2, bottomTextY, smallFontSize, BLUE);
                }
                else if (!player1.isAlive && player2.isAlive) {
                    const char* text = "PLAYER 2 WINS BY ELIMINATION!";
                    int textWidth = MeasureText(text, smallFontSize);
                    DrawText(text, screenWidth / 2 - textWidth / 2, bottomTextY, smallFontSize, RED);
                }
            }
            else {
                int uiY = ScaleIntY(20);
                int lineHeight = ScaleIntY(40);

                DrawText("Score:", ScaleInt(10), uiY, uiFontSize, WHITE);
                DrawText(TextFormat("%d", player1.score), ScaleInt(120), uiY, uiFontSize, WHITE);

                DrawText("Lives:", ScaleInt(10), uiY + lineHeight, uiFontSize, WHITE);
                DrawText(TextFormat("%d", player1.lives), ScaleInt(120), uiY + lineHeight, uiFontSize, RED);

                DrawText("Missed:", ScaleInt(10), uiY + 2 * lineHeight, smallFontSize, WHITE);
                DrawText(TextFormat("%d/10", player1.missedFruits), ScaleInt(120), uiY + 2 * lineHeight, smallFontSize, WHITE);

                if (gameMode == 3) {
                    DrawText(TextFormat("Time: %d", static_cast<int>(gameTime)), screenWidth - ScaleInt(150), ScaleIntY(40), uiFontSize, YELLOW);
                }

                int bonusY = uiY + 3 * lineHeight;
                if (slowMotionActive) {
                    DrawText("Slow Motion!", ScaleInt(10), bonusY, smallFontSize, BLUE);
                    bonusY += ScaleIntY(25);
                }
                if (doublePointsActive) {
                    DrawText("Double Points!", ScaleInt(10), bonusY, smallFontSize, GOLD);
                    bonusY += ScaleIntY(25);
                }
                if (speedBoostActive) {
                    DrawText("Speed Boost!", ScaleInt(10), bonusY, smallFontSize, ORANGE);
                    bonusY += ScaleIntY(25);
                }
                if (freezeActive) {
                    DrawText("Freeze!", ScaleInt(10), bonusY, smallFontSize, SKYBLUE);
                    bonusY += ScaleIntY(25);
                }

                if (gamepad1.connected) {
                    DrawText("Gamepad Connected", screenWidth - ScaleInt(200), screenHeight - ScaleIntY(40), smallFontSize - 2, GREEN);
                }
            }
        }
        else {
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

            // Очищаем экран
            ClearBackground(BLACK);

            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));

            int gameOverFontSize = ScaleInt(40);
            int resultFontSize = ScaleInt(30);
            int smallResultFontSize = ScaleInt(25);

            DrawText("Game Over!", screenWidth / 2 - ScaleInt(80), ScaleIntY(150), gameOverFontSize, RED);

            if (newRecord1) {
                DrawText("PLAYER 1 NEW RECORD!", screenWidth / 2 - ScaleInt(100), ScaleIntY(190), smallResultFontSize, GOLD);
            }
            if (newRecord2) {
                DrawText("PLAYER 2 NEW RECORD!", screenWidth / 2 - ScaleInt(100), ScaleIntY(220), smallResultFontSize, GOLD);
            }

            if (gameMode == 4) {
                DrawText(TextFormat("Player 1 Score: %d", player1.score), screenWidth / 2 - ScaleInt(100), ScaleIntY(260), smallResultFontSize, BLUE);
                DrawText(TextFormat("Player 2 Score: %d", player2.score), screenWidth / 2 - ScaleInt(100), ScaleIntY(290), smallResultFontSize, RED);

                if (player1.score > player2.score) {
                    DrawText("PLAYER 1 WINS!", screenWidth / 2 - ScaleInt(80), ScaleIntY(330), resultFontSize, BLUE);
                }
                else if (player2.score > player1.score) {
                    DrawText("PLAYER 2 WINS!", screenWidth / 2 - ScaleInt(80), ScaleIntY(330), resultFontSize, RED);
                }
                else {
                    DrawText("DRAW!", screenWidth / 2 - ScaleInt(40), ScaleIntY(330), resultFontSize, PURPLE);
                }
            }
            else {
                DrawText(TextFormat("Final Score: %d", player1.score), screenWidth / 2 - ScaleInt(70), ScaleIntY(260), resultFontSize, WHITE);
            }

            DrawText("Press SPACE for menu", screenWidth / 2 - ScaleInt(100), ScaleIntY(390), smallResultFontSize - 5, WHITE);
            DrawText("Press R for Records", screenWidth / 2 - ScaleInt(100), ScaleIntY(420), smallResultFontSize - 5, WHITE);

            bool menuPressed = IsKeyPressed(KEY_SPACE);
            if (gamepad1.connected && IsGamepadButtonPressed(gamepad1.gamepadNumber, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                menuPressed = true;
            }
            if (gamepad2.connected && IsGamepadButtonPressed(gamepad2.gamepadNumber, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                menuPressed = true;
            }

            if (menuPressed) {
                gameMode = 0;
                currentMenu = 0;
                selectedButton = 0;
                fruits.clear();
                ResetGame();
            }
            if (IsKeyPressed(KEY_R)) {
                showTopRating = true;
            }
        }

        if (IsKeyPressed(KEY_Q)) {
            if (gameMode != 0) {
                gameMode = 0;
                currentMenu = 0;
                selectedButton = 0;
                fruits.clear();
                ResetGame();
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
    UnloadTexture(Fruit11Texture);
    UnloadTexture(Fruit12Texture);
    UnloadTexture(BasketTexture);

    UnloadTexture(playBigTexture);
    UnloadTexture(recTexture);
    UnloadTexture(howToPlayTexture);
    UnloadTexture(exitTexture);
    UnloadTexture(playSmallTexture);
    UnloadTexture(backgroundTexture);

    CloseWindow();
    return 0;
}