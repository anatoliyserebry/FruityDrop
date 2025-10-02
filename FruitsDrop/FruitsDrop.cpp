#include "raylib.h"
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
   
    if (rec.y < 0) {
        rec.y = 2;
    }
}

int main(void)
{
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);

    Image image = LoadImage("cat.png");
    ImageResize(&image, 132, 114);
    Texture2D texture = LoadTextureFromImage(image);



    Rectangle rec = { screenWidth / 2 - 50,  screenHeight -100,100,100 }; 


    Rectangle rec2 = { screenWidth / 2 - 50, 0,100,100 };

    
    Rectangle rec3 = { screenWidth / 2 - 50, 0,200,200 };
    
    while (!WindowShouldClose())
    {
        MoveRectangle(rec, false);
        MoveRectangle(rec2, true);

        rec2.y += 5; 
        if (rec2.y > screenHeight)
            rec2.y = -50;

        BeginDrawing();
        ClearBackground(RAYWHITE);


        DrawRectangleRec(rec, BLUE);
        DrawRectangleRec(rec2, RED);
        DrawRectangleRec(rec3, BLUE);
        DrawText("Bob", rec.x, rec.y, 20, BLACK);
        DrawTexture(texture, rec.x, rec.y, WHITE);
        DrawText("John", rec2.x, rec2.y, 20, BLACK);
        DrawTexture(texture, rec2.x, rec2.y, WHITE);
        EndDrawing();

    }

    CloseWindow();

    return 0;
}
