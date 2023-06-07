#include "berzerk.h"

//------------------------------------------------------------------------------------
// Ponto de entrada do programa
//------------------------------------------------------------------------------------
int main(void)
{
    Game game;
    game.screenWidth = SCREEN_SIZE_X;
    game.screenHeight = SCREEN_SIZE_Y;

    InitWindow(game.screenWidth, game.screenHeight, "BERZERK");
    SetTargetFPS(60);
    while(!IsKeyDown(KEY_ENTER)){ // Desenha a tela de título antes do jogo começar
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Berzerk", GetScreenWidth()/2 - MeasureText("Berzerk", 20)/2, GetScreenHeight()/2 - 50, 20, BLACK);
        DrawText("Press Enter to continue", GetScreenWidth()/2 - MeasureText("Press Enter to continue", 20)/2, GetScreenHeight()/2 + 25, 20, GRAY);
        EndDrawing();
    }
    InitGame(&game);
    while(!WindowShouldClose()) { // Detecta o pedido de fechamento do jogo
        UpdateDrawFrame(&game);
        if(game.gameover) break;
    }
    while(!IsKeyDown(KEY_ENTER)){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Game Over", GetScreenWidth()/2 - MeasureText("GAME OVER", 20)/2, GetScreenHeight()/2 - 50, 20, BLACK);
        EndDrawing();
    }
    return 0;
}