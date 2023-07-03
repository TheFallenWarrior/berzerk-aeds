#include "berzerk.h"

//------------------------------------------------------------------------------------
// Ponto de entrada do programa
//------------------------------------------------------------------------------------
int main(){
    Game game;

    InitWindow(SCREEN_SIZE_X, SCREEN_SIZE_Y, "Berzerk");
    InitAudioDevice();
    SetTargetFPS(60);
    Sound ting = LoadSound("res/snd/ting1.mp3");
    game.font = LoadFont("res/fnt/alpha_beta.png");

    while(1){ 
        Texture2D logo = LoadTexture("res/gfx/title.png");
        Texture2D box = LoadTexture("res/gfx/box.png");
        Music music = LoadMusicStream("res/bgm/theprelude.mp3");
        PlayMusicStream(music);

        while(!IsKeyPressed(KEY_ENTER)){ // Desenha a tela de título antes do jogo começar
            UpdateMusicStream(music);
            BeginDrawing();
            DrawTexture(logo, (GetScreenWidth() - logo.width)/2, 20, WHITE);
            ClearBackground((Color){28, 16, 28, 255});
            draw_st_text(game.font, "Press Enter to continue", (float)GetScreenHeight()/2 + 75, GRAY);
            EndDrawing();
            if(WindowShouldClose()) exit(0); // Detecta o pedido de fechamento do jogo
        }
        PlaySound(ting);
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
            
            UpdateMusicStream(music);
            BeginDrawing();
            ClearBackground((Color){28, 16, 28, 255});
            DrawTexture(box, (GetScreenWidth() - box.width)/2, 150, WHITE);
            draw_st_text(game.font, "Type your name", (float)GetScreenHeight()/2 - 50, WHITE);
            draw_st_text(game.font, game.hero.name, (float)GetScreenHeight()/2, WHITE);
            if(letter_count >= 3)
                draw_st_text(game.font, "Press Enter to continue", (float)GetScreenHeight()/2 + 50, LIGHTGRAY);
            EndDrawing();
            if(WindowShouldClose()) exit(0);
        }
        PlaySound(ting);

        while(!IsKeyPressed(KEY_ONE) && !IsKeyPressed(KEY_TWO)){ // Tela de seleção de dificuldade
            UpdateMusicStream(music);
            BeginDrawing();
            ClearBackground((Color){28, 16, 28, 255});
            DrawTexture(box, (GetScreenWidth() - box.width)/2, 150, WHITE);
            draw_st_text(game.font, "Select your level", (float)GetScreenHeight()/2 - 50, WHITE);
            draw_st_text(game.font, "1: Easy", (float)GetScreenHeight()/2 + 25, LIGHTGRAY);
            draw_st_text(game.font, "2: Hard", (float)GetScreenHeight()/2 + 50, LIGHTGRAY);
            EndDrawing();
            if(WindowShouldClose()) exit(0); 
        }
        PlaySound(ting);
        if(IsKeyPressed(KEY_ONE)) game.difficulty = 1;
        if(IsKeyPressed(KEY_TWO)) game.difficulty = 2;
        game.hero.bullets_left = 3 - game.difficulty;
        game.timer = clock(); // Inicializa o timer do jogo

        UnloadTexture(box);
        UnloadMusicStream(music);
        music = LoadMusicStream("res/bgm/thesilentlight.mp3");
        PlayMusicStream(music);

        while(!game.gameover && !game.boss_trigger) { // Loop principal do jogo
            UpdateMusicStream(music);
            UpdateGame(&game);
            DrawGame(&game);
            if(WindowShouldClose()) exit(0);
        }

        UnloadMusicStream(music);
        music = LoadMusicStream("res/bgm/themastermind.mp3");
        PlayMusicStream(music);
        if(!game.gameover) PlaySound(game.general_sfx[SnBossCry1]);

        while(!game.gameover){ // Loop da batalha contra o boss final
            UpdateMusicStream(music);
            UpdateBossBattle(&game);
            DrawBossBattle(&game);
            if(WindowShouldClose()) exit(0);
        }

        UnloadMusicStream(music);
        UnloadTexture(game.hero.bullet_texture);
        UnloadTexture(game.hero.texture);
        for(int i=0;i<7;i++) UnloadTexture(game.en_globals.enemy_gfx[i]);
        UnloadTexture(game.en_globals.bullets_gfx);
        music = LoadMusicStream("res/bgm/thehero.mp3");
        PlayMusicStream(music);

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
            if(current_score > highscores[i] && game.boss_trigger){
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
        while(!IsKeyPressed(KEY_R)){ // Desenha a tela de gameover
            UpdateMusicStream(music);
            draw_st_text(game.font, ((game.boss_trigger)?"The End":"Game Over"), 100, WHITE);
            draw_highscores(box, game.font, names, highscores);
            draw_st_text(game.font, "Press 'R' to restart", 424, GRAY);
            if(WindowShouldClose()) exit(0);
        }
        PlaySound(ting);

        // Descarrega os recursos que serão carregados no próximo loop
        UnloadResources(&game);
        UnloadTexture(box);
        UnloadMusicStream(music);
    }
    exit(0);
}