#include "berzerk.h"

//------------------------------------------------------------------------------------
// Ponto de entrada do programa
//------------------------------------------------------------------------------------
int main(){
    Game game;
    int letter_count = 0;

    FILE *hiscores;

    game.screenWidth = SCREEN_SIZE_X;
    game.screenHeight = SCREEN_SIZE_Y;

    InitWindow(game.screenWidth, game.screenHeight, "BERZRK");

    SetTargetFPS(60);
    while(!IsKeyPressed(KEY_ENTER)){ // Desenha a tela de título antes do jogo começar
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("BERZRK", GetScreenWidth()/2 - MeasureText("BERZRK", 20)/2, GetScreenHeight()/2 - 50, 20, BLACK);
        DrawText("Press Enter to continue", GetScreenWidth()/2 - MeasureText("Press Enter to continue", 20)/2, GetScreenHeight()/2 + 25, 20, GRAY);
        EndDrawing();
        if(WindowShouldClose()) exit(0); // Detecta o pedido de fechamento do jogo
    }
    InitGame(&game); // Inicializa o jogo

    while(!IsKeyPressed(KEY_ENTER) || letter_count < 3){ // Tela de entrada do nome do jogador
        int key = GetCharPressed();
        letter_count = strlen(game.hero.name);
        while(key != 0){
            // NOTE: carateres válidos estão no intervalo [32, 125]
            if((key >= 32) && (key <= 125) && (letter_count < CHARACTER_NAME_SIZE)){
                game.hero.name[letter_count] = (char)key;
                game.hero.name[letter_count+1] = 0; // Adicionar o '\0' ao final da string
                letter_count++;
            }
            key = GetCharPressed(); // Ler o próximo caractere na fila
        }

        if(IsKeyPressed(KEY_BACKSPACE)){
            letter_count--;
            if(letter_count < 0) letter_count = 0;
            game.hero.name[letter_count] = '\0';
        }
            
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Input your name:", GetScreenWidth()/2 - MeasureText("Input your name:", 20)/2, GetScreenHeight()/2 - 50, 20, BLACK);
        DrawText(game.hero.name, GetScreenWidth()/2 - MeasureText(game.hero.name, 20)/2, GetScreenHeight()/2, 20, BLACK);
        if(letter_count >= 3)
            DrawText("Press Enter to continue", GetScreenWidth()/2 - MeasureText("Press Enter to continue", 20)/2, GetScreenHeight()/2 + 50, 20, GRAY);
        EndDrawing();
        if(WindowShouldClose()) exit(0); // Detecta o pedido de fechamento do jogo
    }

    while(1) { // Loop principal do jogo
        UpdateDrawFrame(&game);
        if(game.gameover) break;
        if(WindowShouldClose()) exit(0);
    }

    game.timer = clock() - game.timer;
    hiscores = fopen("highscores", "r");

    char str[24];
    char names[3][7];
    int scores[3] = {0, 0, 0};
    double score = (double)game.timer/100000;
    for(int i=0;fgets(str, sizeof(str), hiscores);i++){
        if(i < 3) sscanf(str, "%d %s\n", &scores[i], names[i]); 
    }
    for(int i=0;i<3;i++){
        if(!scores[i]) strcpy(names[i], "");
        if(score > scores[i]){
            strcpy(names[i], game.hero.name);
            scores[i] = (int)score;
            break;
        }
    }

    hiscores = fopen("highscores", "w");
    for(int i=0;i<3;i++){
        fprintf(hiscores, "%d %s\n", scores[i], names[i]);
    }

    //fprintf(hiscores, "%.0lf %s\n", (double)game.timer/100000, game.hero.name);
    fclose(hiscores);
    while(!IsKeyPressed(KEY_ENTER)){ // Desenha a tela de gameover
        draw_highscores(names, scores);
        if(WindowShouldClose()) exit(0);
    }
    exit(0);
}