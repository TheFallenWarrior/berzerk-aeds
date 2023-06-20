#include "berzerk.h"

//------------------------------------------------------------------------------------
// Definições das funções do módulo
//------------------------------------------------------------------------------------

// Inicializa a variáveis do jogo
void InitGame(Game *g){
    g->curr_map = 0;
    g->num_maps = 10;
    strcpy(g->hero.name, "\0");
    g->hero.current_frame = 0;
    g->hero.texture = LoadTexture("res/gfx/berzerker.png");
    g->hero.pos = (Rectangle){150, 300, STD_SIZE_X,  STD_SIZE_Y};
    g->hero.color = BLACK;
    g->hero.speed = 4;
    g->hero.direction = KEY_DOWN;
    g->hero.bullets_left = 2;
    g->hero.bullets[0] = (Bullet){
        (Rectangle){SCREEN_SIZE_X, SCREEN_SIZE_Y, 5, 5},
        0,
        0,
        8,
    };
    g->hero.bullets[1] = g->hero.bullets[0];
    g->gameover = 0;
    map0_setup(g);
    map1_setup(g);
    map2_setup(g);
}

// Atualiza o jogo (um frame)
void UpdateGame(Game *g){
    update_hero_pos(g);

    Map *map = &g->maps[g->curr_map];
    Hero *h = &g->hero;

    if(IsKeyReleased(KEY_SPACE)) shoot_bullet(g);

    for(int i=0; i < map->num_enemies; i++){
        if(!map->enemies[i].draw_enemy) continue;
        update_enemy_pos(g, &map->enemies[i]);
        if(CheckCollisionRecs(h->bullets[0].pos, map->enemies[i].pos)){
            map->enemies[i].draw_enemy = 0;
            if(map->enemies[i].has_key){
                map->door_locked = 0;
            }
            h->bullets[0].pos = (Rectangle){SCREEN_SIZE_X, SCREEN_SIZE_Y, 5, 5};
        }
        if(CheckCollisionRecs(h->bullets[1].pos, map->enemies[i].pos)){
            map->enemies[i].draw_enemy = 0;
            if(map->enemies[i].has_key){
                map->door_locked = 0;
            }
            h->bullets[1].pos = (Rectangle){SCREEN_SIZE_X, SCREEN_SIZE_Y, 5, 5};
        }
        if(!CheckCollisionRecs(h->pos, map->enemies[i].pos))
            continue;
        g->gameover = 1;
    }

    if(CheckCollisionRecs(h->pos, map->door) && !map->door_locked){
        g->curr_map = map->next_map;
        h->pos = (Rectangle){50, 200, STD_SIZE_X, STD_SIZE_Y};
    }

    if(map->prev_map != -1 && CheckCollisionRecs(h->pos, map->prev_door)){
        g->curr_map = map->prev_map;
        h->pos = (Rectangle){g->screenWidth-50, (float)g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
   }
}

// Desenha a tela (um frame)
void DrawGame(Game *g){
    Hero *h = &g->hero;
    int hero_sprite_offset = 0;

    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawRectangle(0, 0, g->screenWidth, g->screenHeight, GRAY);
    draw_borders(g);
    draw_map(g);

    switch (h->direction){
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
    
    DrawRectangleRec(h->bullets[0].pos, h->color);
    DrawRectangleRec(h->bullets[1].pos, h->color);
    DrawTextureRec(
        h->texture,
        (Rectangle){0+hero_sprite_offset+(h->current_frame*32), 0, 32, 48},
        (Vector2){h->pos.x, h->pos.y - (h->texture.height-STD_SIZE_Y)},
        WHITE
    );
    EndDrawing();
}

// Atualiza e desenha
void UpdateDrawFrame(Game *g){
    UpdateGame(g);
    DrawGame(g);
}

void draw_highscores(char names[3][7], int *scores){
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Game Over", GetScreenWidth()/2 - MeasureText("Game Over", 20)/2, 100, 20, BLACK);
    DrawText("Highscores", GetScreenWidth()/2 - MeasureText("Highscores", 20)/2, 200, 20, BLACK);
    for(int i=0;i<3;i++){
        char str[24];
        sprintf(str, "(%d) %s", scores[i], names[i]);
        DrawText(str, GetScreenWidth()/2 - MeasureText(str, 20)/2, 250 + 50*i, 20, GRAY);
    }
    EndDrawing();
}

void draw_borders(Game *g){
    DrawRectangle(0, 0, SCREEN_BORDER, g->screenHeight, BLACK);
    DrawRectangle(0, 0, g->screenWidth, SCREEN_BORDER, BLACK);
    DrawRectangle(g->screenWidth-SCREEN_BORDER, 0, g->screenWidth, g->screenHeight, BLACK);
    DrawRectangle(0, g->screenHeight-SCREEN_BORDER, g->screenWidth, g->screenHeight, BLACK);
}

void draw_map(Game *g){
    Map *map = &g->maps[g->curr_map];
    for(int i = 0; i < map->num_barriers; i++){
        DrawRectangleRec(map->barriers[i], BLACK);
    }

    if(map->door_locked)
        DrawRectangleRec(map->door, RED);
    else
        DrawRectangleRec(map->door, GREEN);

    if(map->prev_map != -1){
        DrawRectangleRec(map->prev_door, GREEN);
    }

    for(int i=0; i< map->num_enemies; i++){
        if(!map->enemies[i].draw_enemy) continue;
        DrawRectangleRec(map->enemies[i].pos, PINK);
    }
}

void update_hero_pos(Game *g){
    Hero *h = &g->hero;
    Map *m = &g->maps[g->curr_map];
    static int frame_counter = 0;

    update_bullet_pos(g, 0);
    update_bullet_pos(g, 1);

    if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)){
        if(h->pos.x > SCREEN_BORDER)
            h->pos.x -= h->speed;
        if(barrier_collision(m, &h->pos)) h->pos.x += h->speed;
        h->direction = KEY_LEFT;
        frame_counter++;
        
    } else if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)){
        if(h->pos.x + h->pos.width < g->screenWidth - SCREEN_BORDER)
            h->pos.x += h->speed;
        if(barrier_collision(m, &h->pos)) h->pos.x -= h->speed;
        h->direction = KEY_RIGHT;
        frame_counter++;

    } else if(IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)){
        if(h->pos.y > SCREEN_BORDER)
            h->pos.y -= h->speed;
        if(barrier_collision(m, &h->pos)) h->pos.y += h->speed;
        h->direction = KEY_UP;
        frame_counter++;

    } else if(IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)){
        if(h->pos.y + h->pos.height < g->screenHeight - SCREEN_BORDER)
            h->pos.y += h->speed;
        if(barrier_collision(m, &h->pos)) h->pos.y -= h->speed;
        h->direction = KEY_DOWN;
        frame_counter++;
    }

    if(frame_counter >= 10){
        frame_counter = 0;
        h->current_frame++;
    }
    if(h->current_frame >= 4) h->current_frame = 0;
}

void shoot_bullet(Game *g){
    Hero *h = &g->hero;
    Bullet *hb = &h->bullets[2 - h->bullets_left];

    if(h->bullets_left>0){
        hb->pos.x = h->pos.x-1 + (float)(STD_SIZE_X)/2;
        hb->pos.y = h->pos.y-1 + (float)(STD_SIZE_Y)/2;
        hb->active = 1;
        hb->direction = h->direction;
        h->bullets_left--;
        h->current_frame++;
        if(h->current_frame >= 4) h->current_frame = 0;
    }
}

void update_bullet_pos(Game *g, int index){
    Bullet *hb = &g->hero.bullets[index];
    Map *m = &g->maps[g->curr_map];

    Bullet blank_bullet = (Bullet){
        (Rectangle){SCREEN_SIZE_X, SCREEN_SIZE_Y, 5, 5},
        0,
        0,
        8,
    };

    if(hb->active){
        switch(hb->direction){
            case KEY_LEFT:
                if(hb->pos.x > SCREEN_BORDER)
                    hb->pos.x -= hb->speed;
                else{
                    *hb = blank_bullet;
                    g->hero.bullets_left = 2;
                }
                break;
            
            case KEY_RIGHT:
                if(hb->pos.x + hb->pos.width < g->screenWidth - SCREEN_BORDER)
                    hb->pos.x += hb->speed;
                else{
                    *hb = blank_bullet;
                    g->hero.bullets_left = 2;
                }
                break;
            
            case KEY_UP:
                if(hb->pos.y > SCREEN_BORDER)
                    hb->pos.y -= hb->speed;
                else{
                    *hb = blank_bullet;
                    g->hero.bullets_left = 2;
                }
                break;
            
            case KEY_DOWN:
                if(hb->pos.y + hb->pos.height < g->screenHeight - SCREEN_BORDER)
                    hb->pos.y += hb->speed;
                else{
                    *hb = blank_bullet;
                    g->hero.bullets_left = 2;
                }
                break;
        }
        if(barrier_collision(m, &hb->pos)){
            *hb = blank_bullet;
            g->hero.bullets_left = 2;
        }
    }
}

void update_enemy_pos(Game *g, Enemy *e){
    Map *m = &g->maps[g->curr_map];

    if(e->direction == KEY_LEFT){
        if(e->pos.x > SCREEN_BORDER)
            e->pos.x -= e->speed;
        else{
            e->direction = KEY_RIGHT + (rand()%4);
        }
        if(barrier_collision(m, &e->pos)){
            e->pos.x += e->speed;
            e->direction = KEY_RIGHT + (rand()%4);
        }

    } else if(e->direction == KEY_RIGHT){
        if(e->pos.x + e->pos.width < g->screenWidth - SCREEN_BORDER)
            e->pos.x += e->speed;
        else
            e->direction = KEY_RIGHT + (rand()%4);
        
        if(barrier_collision(m, &e->pos)){
            e->pos.x -= e->speed;
            e->direction = KEY_RIGHT + (rand()%4);
        }
        
    } else if(e->direction == KEY_UP){
        if(e->pos.y > SCREEN_BORDER)
            e->pos.y -= e->speed;
        else
            e->direction = KEY_RIGHT + (rand()%4);
        
        if(barrier_collision(m, &e->pos)){
            e->pos.y += e->speed;
            e->direction = KEY_RIGHT + (rand()%4);
        }
    } else if(e->direction == KEY_DOWN){
        if(e->pos.y + e->pos.height < g->screenHeight - SCREEN_BORDER)
            e->pos.y += e->speed;
        else
            e->direction = KEY_RIGHT + (rand()%4);
        
        if(barrier_collision(m, &e->pos)){
            e->pos.y -= e->speed;
            e->direction = KEY_RIGHT + (rand()%4);
        }
    }
    if(rand()%100 == 1) // 1% de chance do inimigo mudar de direção
        e->direction = KEY_RIGHT + (rand()%4);
}

int barrier_collision(Map *map, Rectangle *target){
    for(int i = 0; i < map->num_barriers; i++){
        if(CheckCollisionRecs(*target, map->barriers[i])){
            return 1;
        }
    }
    return 0;
}

// Setup dos mapas
void map0_setup(Game *g){
    Map *m = &g->maps[0];
    m->num_barriers = 1;
    m->barriers[0] = (Rectangle){(float)g->screenWidth/2, 0, 5, 0.8 * g->screenHeight};
    m->color = GRAY;
    m->door = (Rectangle){g->screenWidth-(SCREEN_BORDER+5), (float)g->screenHeight/3, SCREEN_BORDER, 50};
    m->num_enemies = 2;
    m->door_locked = 1;

    for(int i=0; i< m->num_enemies; i++){
        m->enemies[i].type = 0;
        m->enemies[i].pos = (Rectangle){(float)2*g->screenWidth/3, (float)2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
        m->enemies[i].color = BLACK;
        m->enemies[i].speed = 4;
        m->enemies[i].direction = KEY_RIGHT + (rand()%4);
        m->enemies[i].draw_enemy = 1;
        m->enemies[i].has_key = 0;
    }
    m->enemies[0].has_key = 1;
    m->prev_map = -1;
    m->next_map = 1;
}

void map1_setup(Game *g){
    Map *m = &g->maps[1];
    m->num_barriers = 2;
    m->barriers[0] = (Rectangle){(float)3*g->screenWidth/4, 0, 5, 0.6 * g->screenHeight};
    m->barriers[1] = (Rectangle){(float)g->screenWidth/4, 0.4*g->screenHeight, 5, g->screenHeight};
    m->color = GRAY;
    m->door = (Rectangle){g->screenWidth-(SCREEN_BORDER+5), (float)g->screenHeight/3, SCREEN_BORDER, 50};
    m->prev_door = (Rectangle){SCREEN_BORDER, (float)g->screenHeight/3, 5, 50};
    m->num_enemies = 3;
    m->door_locked = 1;

    for(int i=0; i< m->num_enemies; i++){
        m->enemies[i].type = 0;
        m->enemies[i].pos = (Rectangle){ (float)2*g->screenWidth/3, (float)2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
        m->enemies[i].color = BLACK;
        m->enemies[i].speed = 4;
        m->enemies[i].direction = KEY_RIGHT + (rand()%4);
        m->enemies[i].draw_enemy = 1;
        m->enemies[i].has_key = 0;
    }
    m->enemies[0].has_key = 1;
    m->prev_map = 0;
    m->next_map = 2;
}

void map2_setup(Game *g){
    Map *m = &g->maps[2];
    m->num_barriers = 3;
    m->barriers[0] = (Rectangle){(float)g->screenWidth/4, 0, 5, 0.6 * g->screenHeight};
    m->barriers[1] = (Rectangle){(float)2*g->screenWidth/4, 0, 5, 0.6 * g->screenHeight};
    m->barriers[2] = (Rectangle){(float)3*g->screenWidth/4, 0, 5, 0.6 * g->screenHeight};
    m->color = GRAY;
    m->door = (Rectangle){(float)g->screenWidth-(SCREEN_BORDER+5), (float)g->screenHeight/3, SCREEN_BORDER, 50};
    m->prev_door = (Rectangle){SCREEN_BORDER, (float)g->screenHeight/3, 5, 50};
    m->num_enemies = 4;
    m->door_locked = 1;

    for(int i=0; i< m->num_enemies; i++){
        m->enemies[i].pos = (Rectangle){(float)2*g->screenWidth/3, (float)2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
        m->enemies[i].color = BLACK;
        m->enemies[i].speed = 4;
        m->enemies[i].direction = KEY_RIGHT + (rand()%4);
        m->enemies[i].draw_enemy = 1;
        m->enemies[i].has_key = 0;
    }
    m->enemies[0].has_key = 1;
    m->prev_map = 1;
    m->next_map = 3;
}