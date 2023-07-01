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
    g->hero.bullet_texture = LoadTexture("res/gfx/bullet1.png");
    g->hero.pos = (Rectangle){150, 300, STD_SIZE_X,  STD_SIZE_Y};
    g->hero.color = BLACK;
    g->hero.speed = 4;
    g->hero.direction = KEY_DOWN;
    g->hero.bullets_left = 2;
    g->hero.bullets[0] = (Bullet){
        (Rectangle){SCREEN_SIZE_X, SCREEN_SIZE_Y, 6, 6},
        0,
        0,
        8,
    };
    g->hero.bullets[1] = g->hero.bullets[0];
    g->gameover = 0;

    //Definição das informações globais dos inimigos
    g->en_globals.bullets_gfx = LoadTexture("res/gfx/bullet2.png");
    g->en_globals.enemy_gfx[0] = LoadTexture("res/gfx/zombie.png");
    g->en_globals.enemy_gfx[1] = LoadTexture("res/gfx/knight.png");
    g->en_globals.enemy_gfx[2] = LoadTexture("res/gfx/soldier.png");
    g->en_globals.enemy_gfx[3] = LoadTexture("res/gfx/vampire.png");
    g->en_globals.enemy_gfx[4] = LoadTexture("res/gfx/magitek.png");
    g->en_globals.enemy_defs[Zombie][EnMaxBullets]   = 0;
    g->en_globals.enemy_defs[Zombie][EnSpeed]        = 3;
    g->en_globals.enemy_defs[Zombie][EnHP]           = 1;
    g->en_globals.enemy_defs[Knight][EnMaxBullets]   = 1;
    g->en_globals.enemy_defs[Knight][EnSpeed]        = 3;
    g->en_globals.enemy_defs[Knight][EnHP]           = 2;
    g->en_globals.enemy_defs[Soldier][EnMaxBullets]  = 1;
    g->en_globals.enemy_defs[Soldier][EnSpeed]       = 2;
    g->en_globals.enemy_defs[Soldier][EnHP]          = 1;
    g->en_globals.enemy_defs[Vampire][EnMaxBullets]  = 2;
    g->en_globals.enemy_defs[Vampire][EnSpeed]       = 4;
    g->en_globals.enemy_defs[Vampire][EnHP]          = 4;
    g->en_globals.enemy_defs[Magitek][EnMaxBullets]  = 4;
    g->en_globals.enemy_defs[Magitek][EnSpeed]       = 2;
    g->en_globals.enemy_defs[Magitek][EnHP]          = 3;

    map0_setup(g);
    map1_setup(g);
    map2_setup(g);
    map3_setup(g);
    map4_setup(g);
    map5_setup(g);
}

// Atualiza o jogo (um frame)
void UpdateGame(Game *g){
    update_hero_pos(g);

    Map *m = &g->maps[g->curr_map];
    Hero *h = &g->hero;

    if(IsKeyReleased(KEY_SPACE)) shoot_bullet(g);

    for(int i=0; i < m->num_enemies; i++){
        if(!m->enemies[i].draw_enemy) continue;
        
        // Atualiza a posição das balas dos inimigos
        for(int j=0;j<g->en_globals.enemy_defs[m->enemies[i].type][EnMaxBullets];j++){
            update_bullet_pos(
                g,
                &m->enemies[i].bullets[j],
                &m->enemies[i].bullets_left,
                g->en_globals.enemy_defs[m->enemies[i].type][EnMaxBullets]
            );
            if(CheckCollisionRecs(m->enemies[i].bullets[j].pos, g->hero.pos))
                g->gameover = 1;
        }

        update_enemy(g, &m->enemies[i]);
        if(CheckCollisionRecs(h->bullets[0].pos, m->enemies[i].pos)){
            m->enemies[i].hp--;
            h->bullets[0].active = 0;
            h->bullets_left = 3 - g->difficulty;
            h->bullets[0].pos = (Rectangle){SCREEN_SIZE_X, SCREEN_SIZE_Y, 6, 6};
        }
        if(CheckCollisionRecs(h->bullets[1].pos, m->enemies[i].pos)){
            m->enemies[i].hp--;
            h->bullets[1].active = 0;
            h->bullets_left = 3 - g->difficulty;
            h->bullets[1].pos = (Rectangle){SCREEN_SIZE_X, SCREEN_SIZE_Y, 6, 6};
        }
        if(m->enemies[i].hp <= 0){
            m->enemies[i].draw_enemy = 0;
            if(m->enemies[i].has_key){
                m->door_locked = 0;
            }
        }
        if(CheckCollisionRecs(h->pos, m->enemies[i].pos))
            g->gameover = 1;
    }

    if(CheckCollisionRecs(h->pos, m->door) && !m->door_locked){
        g->curr_map = m->next_map;
        h->pos = (Rectangle){50, 200, STD_SIZE_X, STD_SIZE_Y};
    }

    if(m->prev_map != -1 && CheckCollisionRecs(h->pos, m->prev_door)){
        g->curr_map = m->prev_map;
        h->pos = (Rectangle){SCREEN_SIZE_X-50, (float)SCREEN_SIZE_Y/3, STD_SIZE_X, STD_SIZE_Y};
   }
}

// Desenha a tela (um frame)
void DrawGame(Game *g){
    Hero *h = &g->hero;

    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawRectangle(0, 0, SCREEN_SIZE_X, SCREEN_SIZE_Y, GRAY);
    draw_borders(g);
    DrawTexture(
        h->bullet_texture,
        h->bullets[0].pos.x-2,
        h->bullets[0].pos.y-2,
        WHITE
    );
    DrawTexture(
        h->bullet_texture,
        h->bullets[1].pos.x-2,
        h->bullets[1].pos.y-2,
        WHITE
    );
    draw_map(g);
    draw_hero(*h);

    EndDrawing();
}

// Atualiza e desenha
void UpdateDrawFrame(Game *g){
    UpdateGame(g);
    DrawGame(g);
}

// Mostra texto estilizado
void draw_st_text(Font font, char *str, float y_pos, Color color){
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

void draw_highscores(Texture2D bg, Font font, char names[3][7], int *scores){
    BeginDrawing();
    ClearBackground((Color){28, 16, 28, 255});
    DrawTexture(bg, (GetScreenWidth() - bg.width)/2, 184, WHITE);
    draw_st_text(font, "Game Over", 100, WHITE);
    draw_st_text(font, "Highscores", 200, WHITE);
    for(int i=0;i<3;i++){
        char str[24];
        sprintf(str, "(%d) %s", scores[i], names[i]);
        draw_st_text(font, str, 255 + 40*i, LIGHTGRAY);
    }
    EndDrawing();
}

void draw_hero(Hero h){
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

void draw_enemy(Enemy e, Texture2D texture){
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

void draw_borders(Game *g){
    DrawRectangle(0, 0, SCREEN_BORDER, SCREEN_SIZE_Y, BLACK);
    DrawRectangle(0, 0, SCREEN_SIZE_X, SCREEN_BORDER, BLACK);
    DrawRectangle(SCREEN_SIZE_X-SCREEN_BORDER, 0, SCREEN_SIZE_X, SCREEN_SIZE_Y, BLACK);
    DrawRectangle(0, SCREEN_SIZE_Y-SCREEN_BORDER, SCREEN_SIZE_X, SCREEN_SIZE_Y, BLACK);
}

void draw_map(Game *g){
    Map *m = &g->maps[g->curr_map];
    for(int i = 0; i < m->num_barriers; i++){
        DrawRectangleRec(m->barriers[i], BLACK);
    }

    if(m->door_locked)
        DrawRectangleRec(m->door, RED);
    else
        DrawRectangleRec(m->door, GREEN);

    if(m->prev_map != -1){
        DrawRectangleRec(m->prev_door, GREEN);
    }

    for(int i=0;i<m->num_enemies;i++){
        if(!m->enemies[i].draw_enemy) continue;
        draw_enemy(m->enemies[i], g->en_globals.enemy_gfx[m->enemies[i].type]);
        // Mostra as balas do inimigo
        for(int j=0;j<g->en_globals.enemy_defs[m->enemies[i].type][EnMaxBullets];j++){
            if(m->enemies[i].bullets[j].active){
                DrawTexture(
                    g->en_globals.bullets_gfx,
                    m->enemies[i].bullets[j].pos.x-2,
                    m->enemies[i].bullets[j].pos.y-2,
                    WHITE
                );
            }
        }
    }
}

void update_hero_pos(Game *g){
    Hero *h = &g->hero;
    Map *m = &g->maps[g->curr_map];
    static int frame_counter = 0;

    update_bullet_pos(g, &h->bullets[0], &h->bullets_left, 3-g->difficulty);
    update_bullet_pos(g, &h->bullets[1], &h->bullets_left, 3-g->difficulty);

    if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)){
        if(h->pos.x > SCREEN_BORDER)
            h->pos.x -= h->speed;
        if(barrier_collision(m, &h->pos)) h->pos.x += h->speed;
        h->direction = KEY_LEFT;
        frame_counter++;
        
    } else if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)){
        if(h->pos.x + h->pos.width < SCREEN_SIZE_X - SCREEN_BORDER)
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
        if(h->pos.y + h->pos.height < SCREEN_SIZE_Y - SCREEN_BORDER)
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

void update_enemy(Game *g, Enemy *e){
    Map *m = &g->maps[g->curr_map];
    int sr_x = abs((int)(g->hero.pos.x-e->pos.x));
    int sr_y = abs((int)(g->hero.pos.y-e->pos.y));
    static int frame_counter = 0;

    //Atualiza as informações das balas
    if(!(rand()%50) && !e->walking) shoot_enemy_bullet(g, e);

    if(sr_y < EN_SHOOTING_RANGE && e->bullets_left){
        e->walking = 0;
        if(g->hero.pos.x < e->pos.x)
            e->direction = KEY_LEFT;
        else
            e->direction = KEY_RIGHT;
        
    } else if(sr_x < EN_SHOOTING_RANGE && e->bullets_left){
        e->walking = 0;
        if(g->hero.pos.y < e->pos.y)
            e->direction = KEY_UP;
        else
            e->direction = KEY_DOWN;
    
    // Movimento
    } else if(!(rand()%150))
        e->walking = !e->walking;

    if(e->walking){
        frame_counter++;
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
            if(e->pos.x + e->pos.width < SCREEN_SIZE_X - SCREEN_BORDER)
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
            if(e->pos.y + e->pos.height < SCREEN_SIZE_Y - SCREEN_BORDER)
                e->pos.y += e->speed;
            else
                e->direction = KEY_RIGHT + (rand()%4);
            
            if(barrier_collision(m, &e->pos)){
                e->pos.y -= e->speed;
                e->direction = KEY_RIGHT + (rand()%4);
            }
        }
        if(!(rand()%50)) // 2% de chance do inimigo mudar de direção
            e->direction = KEY_RIGHT + (rand()%4);
        if(frame_counter >= 15){
            frame_counter = 0;
            e->current_frame++;
        }
        if(e->current_frame >= 4) e->current_frame = 0;
    }

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

void shoot_enemy_bullet(Game *g, Enemy *e){
    Bullet *eb = &e->bullets[g->en_globals.enemy_defs[e->type][EnMaxBullets] - e->bullets_left];
    fflush(stdout);
    if(e->bullets_left>0){
        eb->pos.x = e->pos.x-3 + (float)(g->en_globals.enemy_gfx[e->type].width)/32;
        eb->pos.y = e->pos.y;
        eb->active = 1;
        eb->direction = e->direction;
        e->bullets_left--;
    }
}

// Atualiza uma bala
void update_bullet_pos(Game *g, Bullet *b, int *bullets_left, int max_bullets){
    Map *m = &g->maps[g->curr_map];

    Bullet blank_bullet = (Bullet){
        (Rectangle){SCREEN_SIZE_X, SCREEN_SIZE_Y, 6, 6},
        0,
        0,
        8,
    };

    if(b->active){
        switch(b->direction){
            case KEY_LEFT:
                if(b->pos.x > SCREEN_BORDER)
                    b->pos.x -= b->speed;
                else{
                    *b = blank_bullet;
                    *bullets_left = max_bullets;
                }
                break;
            
            case KEY_RIGHT:
                if(b->pos.x + b->pos.width < SCREEN_SIZE_X - SCREEN_BORDER)
                    b->pos.x += b->speed;
                else{
                    *b = blank_bullet;
                    *bullets_left = max_bullets;
                }
                break;
            
            case KEY_UP:
                if(b->pos.y > SCREEN_BORDER)
                    b->pos.y -= b->speed;
                else{
                    *b = blank_bullet;
                    *bullets_left = max_bullets;
                }
                break;
            
            case KEY_DOWN:
                if(b->pos.y + b->pos.height < SCREEN_SIZE_Y - SCREEN_BORDER)
                    b->pos.y += b->speed;
                else{
                    *b = blank_bullet;
                    *bullets_left = max_bullets;
                }
                break;
        }
        if(barrier_collision(m, &b->pos)){
            *b = blank_bullet;
            *bullets_left = max_bullets;
        }
    }
}

int barrier_collision(Map *m, Rectangle *t){
    for(int i = 0; i < m->num_barriers; i++){
        if(CheckCollisionRecs(*t, m->barriers[i])){
            return 1;
        }
    }
    return 0;
}

void enemy_setup(Game *g, Enemy *e, int max_type, Map m){
    e->walking = 0;
    e->type = rand()%max_type;
    do{
        e->pos = (Rectangle){
            (float)16+rand()%(SCREEN_SIZE_X-64),
            (float)16+rand()%(SCREEN_SIZE_Y-64),
            (float)g->en_globals.enemy_gfx[e->type].width/16,
            ((float)g->en_globals.enemy_gfx[e->type].height/2)
        };
    } while(CheckCollisionRecs(g->hero.pos, e->pos) || barrier_collision(&m, &e->pos));
    for(int i=0;i<g->en_globals.enemy_defs[e->type][EnMaxBullets];i++){
        e->bullets[i] = (Bullet){
            (Rectangle){SCREEN_SIZE_X, SCREEN_SIZE_Y, 6, 6},
            0,
            0,
            8,
        };
    }
    e->bullets_left = g->en_globals.enemy_defs[e->type][EnMaxBullets];
    e->hp = g->en_globals.enemy_defs[e->type][EnHP];
    e->speed = g->en_globals.enemy_defs[e->type][EnSpeed];
    e->direction = KEY_RIGHT + (rand()%4);
    e->draw_enemy = 1;
    e->has_key = 0;
}

// Setup dos mapas
void map0_setup(Game *g){
    Map *m = &g->maps[0];
    m->num_barriers = 1;
    m->barriers[0] = (Rectangle){(float)3*SCREEN_SIZE_X/5, 0, 32, 0.65*SCREEN_SIZE_Y};
    m->color = GRAY;
    m->door = (Rectangle){SCREEN_SIZE_X-(SCREEN_BORDER+5), (float)SCREEN_SIZE_Y/3, SCREEN_BORDER, 50};
    m->num_enemies = 2;
    m->door_locked = 1;

    for(int i=0; i< m->num_enemies; i++){
        enemy_setup(g, &m->enemies[i], 1, *m);
    }
    m->enemies[0].has_key = 1;
    m->prev_map = -1;
    m->next_map = 1;
}

void map1_setup(Game *g){
    Map *m = &g->maps[1];
    m->num_barriers = 2;
    m->barriers[0] = (Rectangle){(float)3*SCREEN_SIZE_X/5, 0, 32, 0.6*SCREEN_SIZE_Y};
    m->barriers[1] = (Rectangle){(float)2*SCREEN_SIZE_X/5, (float)SCREEN_SIZE_Y/2, 32, SCREEN_SIZE_Y};
    m->color = GRAY;
    m->door = (Rectangle){SCREEN_SIZE_X-(SCREEN_BORDER+5), (float)SCREEN_SIZE_Y/3, SCREEN_BORDER, 50};
    m->prev_door = (Rectangle){5, (float)SCREEN_SIZE_Y/3, SCREEN_BORDER, 50};
    m->num_enemies = 3;
    m->door_locked = 1;

    for(int i=0; i< m->num_enemies; i++){
        enemy_setup(g, &m->enemies[i], 4, *m);
    }
    m->enemies[0].has_key = 1;
    m->prev_map = 0;
    m->next_map = 2;
}

void map2_setup(Game *g){
    Map *m = &g->maps[2];
    m->num_barriers = 6;
    m->barriers[0] = (Rectangle){0, 150, 200, 32};
    m->barriers[1] = (Rectangle){0, 250, 200, 32};
    m->barriers[2] = (Rectangle){600, 150, 200, 32};
    m->barriers[3] = (Rectangle){600, 250, 200, 32};
    m->barriers[4] = (Rectangle){(float)SCREEN_SIZE_X/2-168, 16, 336, 32};
    m->barriers[5] = (Rectangle){(float)SCREEN_SIZE_X/2-168, SCREEN_SIZE_Y-48, 336, 32};
    m->color = GRAY;
    m->door = (Rectangle){(float)SCREEN_SIZE_X-(SCREEN_BORDER+5), (float)SCREEN_SIZE_Y/3, SCREEN_BORDER, 50};
    m->prev_door = (Rectangle){5, (float)SCREEN_SIZE_Y/3, SCREEN_BORDER, 50};
    m->num_enemies = 4;
    m->door_locked = 1;

    for(int i=0; i< m->num_enemies; i++){
        enemy_setup(g, &m->enemies[i], 5, *m);
    }
    m->enemies[0].has_key = 1;
    m->prev_map = 1;
    m->next_map = 3;
}

void map3_setup(Game *g){
    Map *m = &g->maps[3];
    m->num_barriers = 3;
    m->barriers[0] = (Rectangle){(float)SCREEN_SIZE_X/4-16, 0, 32, 0.6 * SCREEN_SIZE_Y};
    m->barriers[1] = (Rectangle){(float)2*SCREEN_SIZE_X/4-16, 0, 32, 0.6 * SCREEN_SIZE_Y};
    m->barriers[2] = (Rectangle){(float)3*SCREEN_SIZE_X/4-16, 0, 32, 0.6 * SCREEN_SIZE_Y};
    m->color = GRAY;
    m->door = (Rectangle){(float)SCREEN_SIZE_X-(SCREEN_BORDER+5), (float)SCREEN_SIZE_Y/3, SCREEN_BORDER, 50};
    m->prev_door = (Rectangle){5, (float)SCREEN_SIZE_Y/3, SCREEN_BORDER, 50};
    m->num_enemies = 4;
    m->door_locked = 1;

    for(int i=0; i< m->num_enemies; i++){
        enemy_setup(g, &m->enemies[i], 5, *m);
    }
    m->enemies[0].has_key = 1;
    m->prev_map = 4;
    m->next_map = 4;
}

void map4_setup(Game *g){
    Map *m = &g->maps[4];
    m->num_barriers = 3;
    m->barriers[0] = (Rectangle){(float)SCREEN_SIZE_X/4, 0, 5, 0.6 * SCREEN_SIZE_Y};
    m->barriers[1] = (Rectangle){(float)2*SCREEN_SIZE_X/4, 0, 5, 0.6 * SCREEN_SIZE_Y};
    m->barriers[2] = (Rectangle){(float)3*SCREEN_SIZE_X/4, 0, 5, 0.6 * SCREEN_SIZE_Y};
    m->color = GRAY;
    m->door = (Rectangle){(float)SCREEN_SIZE_X-(SCREEN_BORDER+5), (float)SCREEN_SIZE_Y/3, SCREEN_BORDER, 50};
    m->prev_door = (Rectangle){5, (float)SCREEN_SIZE_Y/3, SCREEN_BORDER, 50};
    m->num_enemies = 4;
    m->door_locked = 1;

    for(int i=0; i< m->num_enemies; i++){
        enemy_setup(g, &m->enemies[i], 5, *m);
    }
    m->enemies[0].has_key = 1;
    m->prev_map = 3;
    m->next_map = 5;
}

void map5_setup(Game *g){
    Map *m = &g->maps[5];
    m->num_barriers = 9;
    m->barriers[0] = (Rectangle){16, 176, 32, 32};
    m->barriers[1] = (Rectangle){16, 272, 32, 32};
    m->barriers[2] = (Rectangle){SCREEN_SIZE_X-48, 176, 32, 32};
    m->barriers[3] = (Rectangle){SCREEN_SIZE_X-48, 272, 32, 32};
    m->barriers[4] = (Rectangle){240, 224, 112, 32};
    m->barriers[5] = (Rectangle){448, 224, 112, 32};
    m->barriers[6] = (Rectangle){384, 80, 32, 112};
    m->barriers[7] = (Rectangle){384, 288, 32, 112};
    m->barriers[8] = (Rectangle){384, 224, 32, 32};
    m->color = GRAY;
    m->door = (Rectangle){(float)SCREEN_SIZE_X-(SCREEN_BORDER+5), (float)SCREEN_SIZE_Y/3, SCREEN_BORDER, 50};
    m->prev_door = (Rectangle){5, (float)SCREEN_SIZE_Y/3, SCREEN_BORDER, 50};
    m->num_enemies = 4;
    m->door_locked = 1;

    for(int i=0; i< m->num_enemies; i++){
        enemy_setup(g, &m->enemies[i], 5, *m);
    }
    m->enemies[0].has_key = 1;
    m->prev_map = 3;
    m->next_map = 4;
}