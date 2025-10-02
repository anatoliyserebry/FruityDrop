#include "raylib.h"
#include <string>
#include <cstring>
#include <iostream>

const int screenWidth = 800;
const int screenHeight = 450;

void MoveRectangle(Rectangle& rec, bool controls) {
    if ((controls && IsKeyDown(KEY_LEFT)) || (!controls && IsKeyDown(KEY_A))) {
        rec.x += -5;
    }
    if ((controls && IsKeyDown(KEY_RIGHT)) || (!controls && IsKeyDown(KEY_D))) {
        rec.x += 5;
    }
    if (rec.x + rec.width > screenWidth) {
        rec.x = screenWidth - rec.width - 2;
    }
    if (rec.x < 0) {
        rec.x = 2;
    }
  
}

int main(void)
{
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);

    Image image = LoadImage("cat.png");
    ImageResize(&image, 132, 114);
    Texture2D texture = LoadTextureFromImage(image);

    int score = 0;

    Rectangle player = { screenWidth / 2 - 50,  screenHeight -100,90,90 }; 


    Rectangle rec2 = { screenWidth / 2 - 50, 0,50,50 };
    Rectangle rec3 = { screenWidth / 2 - 400, -500,50,50 };
    Rectangle rec4 = { screenWidth / 2 - 300, -1000,50,50 };
    Rectangle rec5 = { screenWidth / 2 - 300, -1200,50,50 };
    Rectangle rec6 = { screenWidth / 2 - 300, -1300,50,50 };
    
    while (!WindowShouldClose())
    {
        MoveRectangle(player, false);
        MoveRectangle(rec2, true);
        MoveRectangle(rec3, true); 
        MoveRectangle(rec4, true);
        MoveRectangle(rec5, true);
        MoveRectangle(rec6, true);

        rec2.y += 5; 
        if (rec2.y > screenHeight)
            rec2.y = GetRandomValue(-300, 0);
        rec3.y += 5;
        if (rec3.y > screenHeight)
            rec3.y = GetRandomValue(-300,0);
        rec4.y += 5;
        if (rec4.y > screenHeight)
            rec4.y = GetRandomValue(-300, 0);
        rec5.y += 5;
        if (rec5.y > screenHeight)
            rec5.y = GetRandomValue(-300, 0);
        rec6.y += 5; 
        if (rec6.y > screenHeight)
            rec6.y = GetRandomValue(-300, 0);

        //check collision
        if (CheckCollisionRecs(player, rec2)) {
            score += 10;
            rec2.y = GetRandomValue(-300, 0);
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);


        DrawRectangleRec(player, BLUE);
        DrawRectangleRec(rec2, RED);
        DrawRectangleRec(rec3, BLUE);
        DrawText("Bob", player.x, player.y, 20, BLACK);
        //DrawTexture(texture, rec.x, rec.y, WHITE);
        DrawText("John", rec2.x, rec2.y, 20, BLACK);
        //DrawTexture(texture, rec2.x, rec2.y, WHITE);
        DrawText("John", rec3.x, rec3.y, 20, BLACK);
        //DrawTexture(texture, rec3.x, rec3.y, WHITE);
        DrawText("John", rec4.x, rec4.y, 20, BLACK);
        //DrawTexture(texture, rec4.x, rec4.y, WHITE);
        DrawText("John", rec5.x, rec5.y, 20, BLACK);
        //DrawTexture(texture, rec5.x, rec5.y, WHITE);
        DrawText("John", rec6.x, rec6.y, 20, BLACK);
        //DrawTexture(texture, rec6.x, rec6.y, WHITE);
        DrawText((std::to_string(score)).c_str(), 10, 10, 20, BLACK);
        EndDrawing();

    }

    CloseWindow();

    return 0;
}
