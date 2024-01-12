#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "berzerk.h"

//------------------------------------------------------------------------------------
// Program's entry point
//------------------------------------------------------------------------------------
int main(){
    Game game;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Berzerk");
    InitAudioDevice();
    SetTargetFPS(60);
    SetExitKey(KEY_F3);
    Sound ting = LoadSound("res/snd/ting1.mp3");
    game.font = LoadFont("res/fnt/alpha_beta.png");

    while(1){ 
        Texture2D logo = LoadTexture("res/gfx/title.png");
        Texture2D box = LoadTexture("res/gfx/box.png");
        Music music = LoadMusicStream("res/bgm/theprelude.mp3");
        PlayMusicStream(music);

        while(!IsKeyPressed(KEY_ENTER)){ // Draws the titlescreen before the game starts
            UpdateMusicStream(music);
            BeginDrawing();
            ClearBackground((Color){28, 16, 28, 255});
            DrawTexture(logo, (GetScreenWidth() - logo.width)/2, 20, WHITE);
            drawCentrText(game.font, "Press Enter to continue", (float)SCREEN_HEIGHT/2 + 75, GRAY);
            EndDrawing();
            if(WindowShouldClose()) exit(0); // Detects window exit request
        }
        PlaySound(ting);
        initGame(&game); // Initializes the game
        UnloadTexture(logo);

        int letter_count = 0;
        while(!IsKeyPressed(KEY_ENTER) || letter_count < 3){ // Player name input screen
            int key = GetCharPressed();
            letter_count = strlen(game.hero.name);
            while(key != 0){
                // NOTE: valid characters are in the interval [32, 125]
                if((key >= 32) && (key <= 125) && (letter_count < CHARACTER_NAME_SIZE)){
                    game.hero.name[letter_count] = (char)key;
                    game.hero.name[letter_count+1] = 0; // Adds '\0' to the end of the string
                    letter_count++;
                }
                key = GetCharPressed(); // Reads the next character in queue
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
            drawCentrText(game.font, "Type your name", (float)SCREEN_HEIGHT/2 - 50, WHITE);
            drawCentrText(game.font, game.hero.name, (float)SCREEN_HEIGHT/2, WHITE);
            if(letter_count >= 3)
                drawCentrText(game.font, "Press Enter to continue", (float)SCREEN_HEIGHT/2 + 50, LIGHTGRAY);
            EndDrawing();
            if(WindowShouldClose()) exit(0);
        }
        PlaySound(ting);

        while(!IsKeyPressed(KEY_ONE) && !IsKeyPressed(KEY_TWO)){ // Difficulty selection screen
            UpdateMusicStream(music);
            BeginDrawing();
            ClearBackground((Color){28, 16, 28, 255});
            DrawTexture(box, (GetScreenWidth() - box.width)/2, 150, WHITE);
            drawCentrText(game.font, "Select your level", (float)SCREEN_HEIGHT/2 - 50, WHITE);
            drawCentrText(game.font, "1: Easy", (float)SCREEN_HEIGHT/2 + 25, LIGHTGRAY);
            drawCentrText(game.font, "2: Hard", (float)SCREEN_HEIGHT/2 + 50, LIGHTGRAY);
            EndDrawing();
            if(WindowShouldClose()) exit(0); 
        }
        PlaySound(ting);
        if(IsKeyPressed(KEY_ONE)) game.difficulty = 1;
        if(IsKeyPressed(KEY_TWO)) game.difficulty = 2;
        game.hero.bullets_left = 3 - game.difficulty;
        game.timer = clock(); // Initializes game timer

        UnloadTexture(box);
        UnloadMusicStream(music);
        music = LoadMusicStream("res/bgm/thesilentlight.mp3");
        PlayMusicStream(music);

        while(!game.gameover && !game.boss_trigger){ // Game's main loop
            UpdateMusicStream(music);
            updateGame(&game);
            drawGame(&game);
            if(WindowShouldClose()) exit(0);
        }

        UnloadMusicStream(music);
        music = LoadMusicStream("res/bgm/themastermind.mp3");
        PlayMusicStream(music);
        if(!game.gameover) PlaySound(game.general_sfx[SnBossCry1]);

        while(!game.gameover){ // Final boss battle loop
            UpdateMusicStream(music);
            updateBossBattle(&game);
            drawBossBattle(&game);
            if(WindowShouldClose()) exit(0);
        }

        UnloadMusicStream(music);
        UnloadTexture(game.hero.bullet_texture);
        UnloadTexture(game.hero.texture);
        for(int i=0;i<7;i++) UnloadTexture(game.en_globals.enemy_gfx[i]);
        UnloadTexture(game.en_globals.bullets_gfx);
        music = LoadMusicStream("res/bgm/thehero.mp3");
        PlayMusicStream(music);

        // Reading and treatment of highscores
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
        while(!IsKeyPressed(KEY_R)){ // Draws the game over screen
            UpdateMusicStream(music);
            drawCentrText(game.font, ((game.boss_trigger)?"The End":"Game Over"), 100, WHITE);
            drawHighscores(box, game.font, names, highscores);
            drawCentrText(game.font, "Press 'R' to restart", 424, GRAY);
            if(WindowShouldClose()) exit(0);
        }
        PlaySound(ting);

        // Unload the resources that will be loaded on the next loop
        unloadResources(&game);
        UnloadTexture(box);
        UnloadMusicStream(music);
    }
}