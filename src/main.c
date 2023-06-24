#include "berzerk.h"

//------------------------------------------------------------------------------------
// Ponto de entrada do programa
//------------------------------------------------------------------------------------
int main(){
    Game game;

    game.screenWidth = SCREEN_SIZE_X;
    game.screenHeight = SCREEN_SIZE_Y;

    InitWindow(game.screenWidth, game.screenHeight, "Berzerk");
    SetTargetFPS(60);

    game.font = LoadFont("res/fnt/alpha_beta.png");
    Texture2D logo = LoadTexture("res/gfx/title.png");
    Texture2D box = LoadTexture("res/gfx/box.png");

    while(!IsKeyPressed(KEY_ENTER)){ // Desenha a tela de título antes do jogo começar
        BeginDrawing();
        DrawTexture(logo, (GetScreenWidth() - logo.width)/2, 20, WHITE);
        ClearBackground((Color){28, 16, 28, 255});
        draw_st_text(game.font, "Press Enter to continue", (float)GetScreenHeight()/2 + 75, GRAY);
        EndDrawing();
        if(WindowShouldClose()) exit(0); // Detecta o pedido de fechamento do jogo
    }
    InitGame(&game); // Inicializa o jogo
    UnloadTexture(logo);

    int letter_count = 0;
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
        ClearBackground((Color){28, 16, 28, 255});
        DrawTexture(box, (GetScreenWidth() - box.width)/2, 150, WHITE);
        draw_st_text(game.font, "Input your name", (float)GetScreenHeight()/2 - 50, WHITE);
        draw_st_text(game.font, game.hero.name, (float)GetScreenHeight()/2, WHITE);
        if(letter_count >= 3)
            draw_st_text(game.font, "Press Enter to continue", (float)GetScreenHeight()/2 + 50, LIGHTGRAY);
        EndDrawing();
        if(WindowShouldClose()) exit(0);
    }

    while(!IsKeyPressed(KEY_ONE) && !IsKeyPressed(KEY_TWO)){
        ClearBackground((Color){28, 16, 28, 255});
        DrawTexture(box, (GetScreenWidth() - box.width)/2, 150, WHITE);
        draw_st_text(game.font, "Select your level", (float)GetScreenHeight()/2 - 50, WHITE);
        draw_st_text(game.font, "1: Easy", (float)GetScreenHeight()/2 + 25, LIGHTGRAY);
        draw_st_text(game.font, "2: Hard", (float)GetScreenHeight()/2 + 50, LIGHTGRAY);
        EndDrawing();
        if(WindowShouldClose()) exit(0); 
    }
    if(IsKeyPressed(KEY_ONE)) game.difficulty = 1;
    if(IsKeyPressed(KEY_TWO)) game.difficulty = 2;
    game.timer = clock(); // Inicializa o timer do jogo
    UnloadTexture(box);

    while(1) { // Loop principal do jogo
        UpdateDrawFrame(&game);
        if(game.gameover) break;
        if(WindowShouldClose()) exit(0);
    }

    // Leitura e tratamento dos recordes
    FILE *scores_file;
    game.timer = clock() - game.timer;
    scores_file = fopen("highscores", "a+");

    char str[24];
    char names[3][7];
    int highscores[3] = {0, 0, 0};
    double current_score = (double)game.timer/100000;
    for(int i=0;fgets(str, sizeof(str), scores_file);i++){
        if(i < 3) sscanf(str, "%d %s\n", &highscores[i], names[i]); 
    }
    
    fclose(scores_file);
    for(int i=0;i<3;i++){
        if(!highscores[i]) strcpy(names[i], "");
        if(current_score > highscores[i]){
            strcpy(names[i], game.hero.name);
            highscores[i] = (int)current_score;
            break;
        }
    }

    scores_file = fopen("highscores", "w");
    for(int i=0;i<3;i++){
        fprintf(scores_file, "%d %s\n", highscores[i], names[i]);
    }
    fclose(scores_file);

    box = LoadTexture("res/gfx/box.png");
    while(!IsKeyPressed(KEY_ENTER) && !WindowShouldClose()){ // Desenha a tela de gameover
        draw_highscores(box, game.font, names, highscores);
    }
    exit(0);
}