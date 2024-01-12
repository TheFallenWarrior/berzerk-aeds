#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include "berzerk.h"

//------------------------------------------------------------------------------------
// Definition of rendering functions
//------------------------------------------------------------------------------------

// Displays stylized text at the center of the screen
void drawCentrText(Font font, char *str, float y_pos, Color color){
    DrawTextEx(
        font,
        str,
        (Vector2){
            -(MeasureTextEx(font, str, font.baseSize*2, 4).x/2) + (float)GetScreenWidth()/2,
            y_pos
        },
        font.baseSize*2,
        4,
        color
    );
}

void drawHighscores(Texture2D bg, Font font, char names[3][7], int *scores){
    BeginDrawing();
    ClearBackground((Color){28, 16, 28, 255});
    DrawTexture(bg, (GetScreenWidth() - bg.width)/2, 184, WHITE);
    drawCentrText(font, "Highscores", 200, WHITE);
    for(int i=0;i<3;i++){
        char str[24];
        sprintf(str, "(%d) %s", scores[i], names[i]);
        drawCentrText(font, str, 255 + 40*i, LIGHTGRAY);
    }
    EndDrawing();
}

void drawHero(Hero h){
    int hero_sprite_offset = 0;

    switch (h.direction){
        case KEY_DOWN:
            hero_sprite_offset = 0;
            break;
        
        case KEY_UP:
            hero_sprite_offset = 256;
            break;
        
        case KEY_LEFT:
            hero_sprite_offset = 384;
            break;
        
        case KEY_RIGHT:
            hero_sprite_offset = 128;
    }
    
    DrawTextureRec(
        h.texture,
        (Rectangle){hero_sprite_offset+(h.current_frame*32), 0, 32, 48},
        (Vector2){h.pos.x, h.pos.y - (h.texture.height-STD_SIZE_Y)},
        WHITE
    );
}

// Displays enemy bullets
void drawEnemyBullets(Game *g, Enemy e){
    for(int i=0;i<g->en_globals.enemy_defs[e.type][EnMaxBullets];i++){
        if(e.bullets[i].active){
            DrawTexture(
                g->en_globals.bullets_gfx,
                e.bullets[i].pos.x-4,
                e.bullets[i].pos.y-4,
                WHITE
            );
        }
    }
}

void drawEnemy(Enemy e, Texture2D texture){
    int sprite_offset = 0;
    if(!e.walking) e.current_frame = 0;
    switch(e.direction){
        case KEY_DOWN:
            sprite_offset = 0;
            break;

        case KEY_UP:
            sprite_offset = texture.width/2;
            break;

        case KEY_LEFT:
            sprite_offset = 3 * (texture.width/4);
            break;
        
        case KEY_RIGHT:
            sprite_offset = texture.width/4;
    }
    DrawTextureRec(
        texture,
        (Rectangle){
            sprite_offset+((float)e.current_frame*((float)texture.width/16)),
            0,
            (float)texture.width/16,
            texture.height
        },
        (Vector2){e.pos.x, e.pos.y - (float)texture.height/2},
        WHITE
    );
}

void drawBoss(Game *g){
    Map *m = &g->maps[8];
    static int frame_counter = 0;

    frame_counter++;
    if(frame_counter >= 240) frame_counter = 0;

    DrawTextureRec(
        g->en_globals.enemy_gfx[SonOfMan],
        (Rectangle){0, 0, 512, 336},
        (Vector2){m->enemies[0].pos.x, m->enemies[0].pos.y},
        WHITE
    );
    DrawTextureRec(
        g->en_globals.enemy_gfx[SonOfMan],
        (Rectangle){512, 0, 128, 80},
        (Vector2){(float)SCREEN_WIDTH/2 - 192, 176 + 12*sin(PI*(float)frame_counter/120)},
        WHITE
    );
    DrawTextureRec(
        g->en_globals.enemy_gfx[SonOfMan],
        (Rectangle){512, 80, 128, 80},
        (Vector2){(float)SCREEN_WIDTH/2 + 64, 176 + 12*sin(PI*(float)frame_counter/120)},
        WHITE
    );
}

void drawCrystal(Game *g, Enemy e){
    DrawTextureRec(
        g->en_globals.enemy_gfx[Crystal],
        (Rectangle){e.current_frame*80, 0, 80, 160},
        (Vector2){e.pos.x, e.pos.y-80},
        WHITE
    );
}

void drawBorders(Game *g){
    DrawTextureRec(
        g->wall_texture,
        (Rectangle){0, 0, SCREEN_BORDER, SCREEN_HEIGHT},
        (Vector2){0, 0},
        WHITE
    );
    DrawTextureRec(
        g->wall_texture,
        (Rectangle){0, 16, SCREEN_WIDTH, SCREEN_BORDER},
        (Vector2){0, 0},
        WHITE
    );
    DrawTextureRec(
        g->wall_texture,
        (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
        (Vector2){SCREEN_WIDTH-SCREEN_BORDER, 0},
        WHITE
    );
    DrawTextureRec(
        g->wall_texture,
        (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
        (Vector2){0, SCREEN_HEIGHT-SCREEN_BORDER},
        WHITE
    );
}

void drawMap(Game *g){
    Map *m = &g->maps[g->curr_map];
    for(int i = 0; i < m->num_barriers; i++){
        DrawTextureRec(
            g->wall_texture,
            (Rectangle){0, 0, m->barriers[i].width, m->barriers[i].height},
            (Vector2){m->barriers[i].x, m->barriers[i].y},
            WHITE
        );
    }

    if(m->door_locked)
        DrawRectangleRec(m->door, RED);
    else
        DrawRectangleRec(m->door, GREEN);

    if(m->prev_map != -1){
        DrawRectangleRec(m->prev_door, GREEN);
    }

    for(int i=0;i<m->num_enemies;i++){
        if(!m->enemies[i].hp) continue;
        drawEnemy(m->enemies[i], g->en_globals.enemy_gfx[m->enemies[i].type]);
        drawEnemyBullets(g, m->enemies[i]);
    }
}