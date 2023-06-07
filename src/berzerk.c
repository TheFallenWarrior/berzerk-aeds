#include "berzerk.h"

//------------------------------------------------------------------------------------
// Definições das funções do módulo
//------------------------------------------------------------------------------------

// Inicializa a variáveis do jogo
void InitGame(Game *g){
    g->curr_map = 0;
    g->num_maps = 10;
    g->hero.pos = (Rectangle){150, 300, STD_SIZE_X, STD_SIZE_Y};
    g->hero.color = BLACK;
    g->hero.speed = 6;
    g->hero.special = 0;
    g->hero.bullets[0] = (HeroBullet){
        (Rectangle){SCREEN_SIZE_X, SCREEN_SIZE_Y, 5, 5},
        BLACK,
        0,
        0,
        10,
    };
    g->hero.bullets[1] = (HeroBullet){
        (Rectangle){SCREEN_SIZE_X, SCREEN_SIZE_Y, 5, 5},
        BLACK,
        0,
        0,
        10,
    };
    g->gameover = 0;
    map0_setup(g);
    map1_setup(g);
    map2_setup(g);
}

// Atualiza o jogo (um frame)
void UpdateGame(Game *g){
    update_hero_pos(g);

    Map *map = &g->maps[g->curr_map];
    for(int i; i < map->num_enemies; i++){
        if(!map->enemies[i].draw_enemy) continue;
        update_enemy_pos(g, &map->enemies[i]);
        if(!CheckCollisionRecs(g->hero.pos, map->enemies[i].pos))
            continue;
        
        if(g->hero.special){
            map->enemies[i].draw_enemy = 0;
            if(map->enemies[i].has_key) {
                map->door_locked = 0;
            }
            continue;
        }
        g->gameover = 1;
    }

    if(CheckCollisionRecs(g->hero.pos, map->special_item) && map->draw_special_item){
        g->hero.pos.width += 10;
        g->hero.pos.height += 10;
        g->hero.special = 1;
        map->draw_special_item = 0;
    }

    if(CheckCollisionRecs(g->hero.pos, map->door) && !map->door_locked){
        g->curr_map = map->next_map;
        g->hero.pos = (Rectangle) { 50, 200, STD_SIZE_X, STD_SIZE_Y};
        g->hero.special = 0;
    }

    if(map->prev_map != -1 && CheckCollisionRecs(g->hero.pos, map->prev_door)){
        g->curr_map = map->prev_map;
        g->hero.pos = (Rectangle) { g->screenWidth-50, g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
        g->hero.special = 0;
    }

}

// Desenha a tela (um frame)
void DrawGame(Game *g){
    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawRectangle(0, 0, g->screenWidth, g->screenHeight, GRAY);
    draw_borders(g);
    draw_map(g);

    DrawRectangleRec(g->hero.pos, g->hero.color);

    EndDrawing();
}

// Atualiza e desenha
void UpdateDrawFrame(Game *g){
    UpdateGame(g);
    DrawGame(g);
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

    if(map->draw_special_item)
        DrawRectangleRec(map->special_item, PURPLE);
}

void update_hero_pos(Game *g){
    Hero *h = &g->hero;
    Map *m = &g->maps[g->curr_map];

    if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        if(h->pos.x > SCREEN_BORDER)
            h->pos.x -= h->speed;
        if(barrier_collision(m, &h->pos)) h->pos.x += h->speed;
        h->direction = KEY_LEFT;
        
    } else if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        if(h->pos.x + h->pos.width < g->screenWidth - SCREEN_BORDER)
            h->pos.x += h->speed;
        if(barrier_collision(m, &h->pos)) h->pos.x -= h->speed;
        h->direction = KEY_RIGHT;

    } else if(IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        if(h->pos.y > SCREEN_BORDER)
            h->pos.y -= h->speed;
        if(barrier_collision(m, &h->pos)) h->pos.y += h->speed;
        h->direction = KEY_UP;

    } else if(IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        if(h->pos.y + h->pos.height < g->screenHeight - SCREEN_BORDER)
            h->pos.y += h->speed;
        if(barrier_collision(m, &h->pos)) h->pos.y -= h->speed;
        h->direction = KEY_DOWN;
    }
}

void update_bullet_pos(Game *g, int index){
    HeroBullet *hb = &g->hero.bullets[index];
    Map *m = &g->maps[g->curr_map];

    if(hb->active){
        switch(hb->direction){
            case KEY_LEFT:
                if(hb->pos.x > SCREEN_BORDER)
                    hb->pos.x -= hb->speed;
                break;
            
            case KEY_RIGHT:
                if(hb->pos.x + hb->pos.width < g->screenWidth - SCREEN_BORDER)
                    hb->pos.x += hb->speed;
                break;
            
            case KEY_UP:
                if(hb->pos.y > SCREEN_BORDER)
                    hb->pos.y -= hb->speed;
                break;
            
            case KEY_DOWN:
                if(hb->pos.y + hb->pos.height < g->screenHeight - SCREEN_BORDER)
                    hb->pos.y += hb->speed;
                break;
        }
        if(barrier_collision(m, &hb->pos)) hb->active = 0;
    }
}

void update_enemy_pos(Game *g, Enemy *e){
    Map *m = &g->maps[g->curr_map];

    if(e->direction == KEY_LEFT) {
        if(e->pos.x > SCREEN_BORDER)
            e->pos.x -= e->speed;
        else{
            e->direction = KEY_RIGHT + (rand()%4);
        }
        if(barrier_collision(m, &e->pos)){
            e->pos.x += e->speed;
            e->direction = KEY_RIGHT + (rand()%4);
        }

    } else if(e->direction == KEY_RIGHT) {
        if(e->pos.x + e->pos.width < g->screenWidth - SCREEN_BORDER)
            e->pos.x += e->speed;
        else
            e->direction = KEY_RIGHT + (rand()%4);
        
        if(barrier_collision(m, &e->pos)){
            e->pos.x -= e->speed;
            e->direction = KEY_RIGHT + (rand()%4);
        }
        
    } else if(e->direction == KEY_UP) {
        if(e->pos.y > SCREEN_BORDER)
            e->pos.y -= e->speed;
        else
            e->direction = KEY_RIGHT + (rand()%4);
        
        if(barrier_collision(m, &e->pos)){
            e->pos.y += e->speed;
            e->direction = KEY_RIGHT + (rand()%4);
        }
    } else if(e->direction == KEY_DOWN) {
        if(e->pos.y + e->pos.height < g->screenHeight - SCREEN_BORDER)
            e->pos.y += e->speed;
        else
            e->direction = KEY_RIGHT + (rand()%4);
        
        if(barrier_collision(m, &e->pos)){
            e->pos.y -= e->speed;
            e->direction = KEY_RIGHT + (rand()%4);
        }
    }
    if(rand() % 100 == 1) // 1% de chance do inimigo mudar de direção
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
    g->maps[0].num_barriers = 1;
    g->maps[0].barriers[0] = (Rectangle) {g->screenWidth/2, 0, 2, 0.8 * g->screenHeight};
    g->maps[0].color = GRAY;
    g->maps[0].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
    g->maps[0].num_enemies = 2;
    g->maps[0].door_locked = 1;

    for(int i=0; i< g->maps[0].num_enemies; i++){
        g->maps[0].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
        g->maps[0].enemies[i].color = BLACK;
        g->maps[0].enemies[i].speed = 6;
        g->maps[0].enemies[i].direction = KEY_RIGHT + (rand() % 4);
        g->maps[0].enemies[i].draw_enemy = 1;
        g->maps[0].enemies[i].has_key = 0;
    }
    g->maps[0].enemies[0].has_key = 1;
    g->maps[0].special_item = (Rectangle) {2*g->screenWidth/3, 20, 15, 15};
    g->maps[0].draw_special_item = 1;
    g->maps[0].prev_map = -1;
    g->maps[0].next_map = 1;
}

void map1_setup(Game *g){
    g->maps[1].num_barriers = 2;
    g->maps[1].barriers[0] = (Rectangle) {3*g->screenWidth/4, 0, 2, 0.6 * g->screenHeight};
    g->maps[1].barriers[1] = (Rectangle) {g->screenWidth/4, 0.4*g->screenHeight, 2, g->screenHeight};
    g->maps[1].color = GRAY;
    g->maps[1].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
    g->maps[1].prev_door = (Rectangle) {SCREEN_BORDER, g->screenHeight/3, 5, 50};
    g->maps[1].num_enemies = 3;
    g->maps[1].door_locked = 1;

    for(int i=0; i< g->maps[1].num_enemies; i++){
        g->maps[1].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
        g->maps[1].enemies[i].color = BLACK;
        g->maps[1].enemies[i].speed = 6;
        g->maps[1].enemies[i].direction = KEY_RIGHT + (rand() % 4);
        g->maps[1].enemies[i].draw_enemy = 1;
        g->maps[1].enemies[i].has_key = 0;
    }
    g->maps[1].enemies[0].has_key = 1;
    g->maps[1].special_item = (Rectangle) {4*g->screenWidth/5, 20, 15, 15};
    g->maps[1].draw_special_item = 1;
    g->maps[1].prev_map = 0;
    g->maps[1].next_map = 2;
}

void map2_setup(Game *g){
    g->maps[2].num_barriers = 3;
    g->maps[2].barriers[0] = (Rectangle) {g->screenWidth/4, 0, 2, 0.6 * g->screenHeight};
    g->maps[2].barriers[1] = (Rectangle) {2*g->screenWidth/4, 0, 2, 0.6 * g->screenHeight};
    g->maps[2].barriers[2] = (Rectangle) {3*g->screenWidth/4, 0, 2, 0.6 * g->screenHeight};
    g->maps[2].color = GRAY;
    g->maps[2].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
    g->maps[2].prev_door = (Rectangle) {SCREEN_BORDER, g->screenHeight/3, 5, 50};
    g->maps[2].num_enemies = 4;
    g->maps[2].door_locked = 1;

    for(int i=0; i< g->maps[2].num_enemies; i++){
        g->maps[2].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
        g->maps[2].enemies[i].color = BLACK;
        g->maps[2].enemies[i].speed = 6;
        g->maps[2].enemies[i].direction = KEY_RIGHT + (rand() % 4);
        g->maps[2].enemies[i].draw_enemy = 1;
        g->maps[2].enemies[i].has_key = 0;
    }
    g->maps[2].enemies[0].has_key = 1;
    g->maps[2].special_item = (Rectangle) {4*g->screenWidth/5, 20, 15, 15};
    g->maps[2].draw_special_item = 1;
    g->maps[2].prev_map = 1;
    g->maps[2].next_map = 3;
}