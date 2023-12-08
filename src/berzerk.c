#include <stdlib.h>
#include <string.h>
#include "berzerk.h"

//------------------------------------------------------------------------------------
// Definition of module functions
//------------------------------------------------------------------------------------

// Initialization of game variables
void InitGame(Game *g){
    g->curr_map = 0;
    g->num_maps = 10;
    strcpy(g->hero.name, "\0");
    g->hero.current_frame = 0;
    g->hero.texture = LoadTexture("res/gfx/berzerker.png");
    g->hero.bullet_texture = LoadTexture("res/gfx/bullet1.png");
    g->hero.pos = (Rectangle){150, 300, STD_SIZE_X,  STD_SIZE_Y};
    g->hero.speed = 4;
    g->hero.direction = KEY_DOWN;
    g->hero.bullets_left = 2;
    g->hero.bullets[0] = (Bullet){
        (Rectangle){SCREEN_SIZE_X, SCREEN_SIZE_Y, 6, 6},
        0,
        8,
        (Vector2){0, 0},
    };
    g->hero.bullets[1] = g->hero.bullets[0];
    g->gameover = 0;
    g->boss_trigger = 0;

    // Global textures
    g->grnd_texture = LoadTexture("res/gfx/ground.png");
    g->wall_texture = LoadTexture("res/gfx/wall.png");

    // Global Sounds
    g->general_sfx[SnPlayerAttack]  = LoadSound("res/snd/attack1.mp3");
    g->general_sfx[SnEnemyAttack]   = LoadSound("res/snd/gun.mp3");
    g->general_sfx[SnEnemyDeath]    = LoadSound("res/snd/enmydeath.mp3");
    g->general_sfx[SnBossCry1]      = LoadSound("res/snd/sonofman.mp3");
    g->general_sfx[SnBossCry2]      = LoadSound("res/snd/sonofman2.mp3");
    g->general_sfx[SnBossAttack]    = LoadSound("res/snd/bossattack.mp3");

    // Definition of global enemy data
    g->en_globals.bullets_gfx = LoadTexture("res/gfx/bullet2.png");
    g->en_globals.enemy_gfx[Zombie]   = LoadTexture("res/gfx/zombie.png");
    g->en_globals.enemy_gfx[Soldier]  = LoadTexture("res/gfx/soldier.png");
    g->en_globals.enemy_gfx[Knight]   = LoadTexture("res/gfx/knight.png");
    g->en_globals.enemy_gfx[Magitek]  = LoadTexture("res/gfx/magitek.png");
    g->en_globals.enemy_gfx[Vampire]  = LoadTexture("res/gfx/vampire.png");
    g->en_globals.enemy_gfx[SonOfMan] = LoadTexture("res/gfx/son_of_man.png");
    g->en_globals.enemy_gfx[Crystal]  = LoadTexture("res/gfx/crystal.png");

    g->en_globals.enemy_defs[Zombie][EnMaxBullets]   = 0;
    g->en_globals.enemy_defs[Zombie][EnSpeed]        = 3;
    g->en_globals.enemy_defs[Zombie][EnHP]           = 1;
    g->en_globals.enemy_defs[Soldier][EnMaxBullets]  = 1;
    g->en_globals.enemy_defs[Soldier][EnSpeed]       = 2;
    g->en_globals.enemy_defs[Soldier][EnHP]          = 1;
    g->en_globals.enemy_defs[Knight][EnMaxBullets]   = 1;
    g->en_globals.enemy_defs[Knight][EnSpeed]        = 3;
    g->en_globals.enemy_defs[Knight][EnHP]           = 2;
    g->en_globals.enemy_defs[Magitek][EnMaxBullets]  = 4;
    g->en_globals.enemy_defs[Magitek][EnSpeed]       = 2;
    g->en_globals.enemy_defs[Magitek][EnHP]          = 3;
    g->en_globals.enemy_defs[Vampire][EnMaxBullets]  = 2;
    g->en_globals.enemy_defs[Vampire][EnSpeed]       = 4;
    g->en_globals.enemy_defs[Vampire][EnHP]          = 4;

    g->en_globals.enemy_defs[SonOfMan][EnMaxBullets]  = 20;
    g->en_globals.enemy_defs[SonOfMan][EnHP]          = 20;
    g->en_globals.enemy_defs[Crystal][EnMaxBullets]   = 10;
    g->en_globals.enemy_defs[Crystal][EnHP]           = 10;

    map0_setup(g);
    map1_setup(g);
    map2_setup(g);
    map3_setup(g);
    map4_setup(g);
    map5_setup(g);
    map6_setup(g);
    map7_setup(g);
    boss_scene_setup(g);
}

// Clear resources loaded in InitGame()
void UnloadResources(Game *g){
    UnloadTexture(g->hero.texture);
    UnloadTexture(g->hero.bullet_texture);
    UnloadTexture(g->en_globals.bullets_gfx);
    UnloadTexture(g->grnd_texture);
    UnloadTexture(g->wall_texture);
    for(int i=0;i<6;i++) UnloadSound(g->general_sfx[i]);
    for(int i=0;i<7;i++) UnloadTexture(g->en_globals.enemy_gfx[i]);
}

// Updates the game (single frame)
void UpdateGame(Game *g){
    Map *m = &g->maps[g->curr_map];
    Hero *h = &g->hero;
    
    update_hero_pos(g);
    if(IsKeyPressed(KEY_SPACE)) shoot_bullet(g);

    for(int i=0; i < m->num_enemies; i++){
        if(!m->enemies[i].hp) continue;
        
        // Updates the position of enemy bullets
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
            PlaySound(g->general_sfx[SnEnemyDeath]);
            if(m->enemies[i].has_key){
                m->door_locked = 0;
            }
        }
        if(CheckCollisionRecs(h->pos, m->enemies[i].pos))
            g->gameover = 1;
    }

    if(CheckCollisionRecs(h->pos, m->door) && !m->door_locked){
        g->curr_map = m->next_map;
        h->pos = (Rectangle){32, (float)SCREEN_SIZE_Y/2-24, STD_SIZE_X, STD_SIZE_Y};
    }

    if(CheckCollisionRecs(h->pos, m->prev_door)){
        g->curr_map = m->prev_map;
        h->pos = (Rectangle){SCREEN_SIZE_X-64, (float)SCREEN_SIZE_Y/2-24, STD_SIZE_X, STD_SIZE_Y};
   }

   if(g->curr_map == 8) g->boss_trigger = 1;
}

// Draws the screen (single frame)
void DrawGame(Game *g){
    Hero *h = &g->hero;

    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawTextureRec(
        g->grnd_texture,
        (Rectangle){0, 0, SCREEN_SIZE_X, SCREEN_SIZE_Y},
        (Vector2){0, 0},
        WHITE
    );
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

void UpdateBossBattle(Game *g){
    Hero *h = &g->hero;
    Map *m = &g->maps[g->curr_map];
    int num_crystals = 0;

    update_hero_pos(g);
    if(IsKeyReleased(KEY_SPACE)) shoot_bullet(g);
    
    for(int i=0; i < m->num_enemies; i++){
        if(!m->enemies[i].hp){
            if(!i) g->gameover = 1;
            else continue;
        }

        if(i){
            num_crystals++;
            update_boss(g, &m->enemies[i]);
        }
        
        // Updates the position of enemy bullets
        for(int j=0;j<g->en_globals.enemy_defs[m->enemies[i].type][EnMaxBullets];j++){
            update_bullet_pos(
                g,
                &m->enemies[i].bullets[j],
                &m->enemies[i].bullets_left,
                g->en_globals.enemy_defs[m->enemies[i].type][EnMaxBullets]
            );
            if(CheckCollisionRecs(m->enemies[i].bullets[j].pos, g->hero.pos)){
                g->boss_trigger = 0;
                g->gameover = 1;
            }
        }

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
            if(num_crystals == 1) PlaySound(g->general_sfx[SnBossCry2]);
            PlaySound(g->general_sfx[SnEnemyDeath]);
        }

        if(CheckCollisionRecs(h->pos, m->enemies[i].pos)){
            g->boss_trigger = 0;
            g->gameover = 1;
        }
    }

    if(num_crystals){
        m->enemies[0].hp = g->en_globals.enemy_defs[SonOfMan][EnHP];
        return;
    }
    update_boss(g, &m->enemies[0]);
}

// Draws final battle (single frame)
void DrawBossBattle(Game *g){
    Hero *h = &g->hero;
    Map *m = &g->maps[8];

    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawTextureRec(
        g->grnd_texture,
        (Rectangle){0, 0, SCREEN_SIZE_X, SCREEN_SIZE_Y},
        (Vector2){0, 0},
        WHITE
    );
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
    draw_hero(*h);
    draw_borders(g);
    draw_boss(g);
    draw_enemy_bullets(g, m->enemies[0]);
    for(int i=0;i<5;i++){
        if(!(m->enemies[i].hp) || m->enemies[i].type == SonOfMan) continue;
        draw_crystal(g, m->enemies[i]);
        draw_enemy_bullets(g, m->enemies[i]);
    }
    EndDrawing();
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
            (float)184+rand()%536,
            (float)SCREEN_BORDER+rand()%(SCREEN_SIZE_Y-64),
            (float)g->en_globals.enemy_gfx[e->type].width/16,
            ((float)g->en_globals.enemy_gfx[e->type].height/2)
        };
    } while(CheckCollisionRecs(g->hero.pos, e->pos) || barrier_collision(&m, &e->pos));
    for(int i=0;i<g->en_globals.enemy_defs[e->type][EnMaxBullets];i++){
        e->bullets[i] = (Bullet){
        (Rectangle){SCREEN_SIZE_X, SCREEN_SIZE_Y, 6, 6},
        0,
        8,
        {0, 0},
    };
    }
    e->bullets_left = g->en_globals.enemy_defs[e->type][EnMaxBullets];
    e->hp = g->en_globals.enemy_defs[e->type][EnHP];
    e->speed = g->en_globals.enemy_defs[e->type][EnSpeed];
    e->direction = KEY_RIGHT + (rand()%4);
    e->has_key = 0;
}

// Map setup
void map0_setup(Game *g){
    Map *m = &g->maps[0];
    m->num_barriers = 1;
    m->barriers[0] = (Rectangle){(float)3*SCREEN_SIZE_X/5, 16, 32, 0.65*SCREEN_SIZE_Y};
    m->door = (Rectangle){SCREEN_SIZE_X-(SCREEN_BORDER+5), (float)SCREEN_SIZE_Y/2-24, SCREEN_BORDER, 48};
    m->door_locked = 1;
    m->num_enemies = 2;

    for(int i=0; i< m->num_enemies; i++){
        enemy_setup(g, &m->enemies[i], 1, *m);
    }
    m->enemies[0].has_key = 1;
    m->prev_map = 0;
    m->next_map = 1;
}

void map1_setup(Game *g){
    Map *m = &g->maps[1];
    m->num_barriers = 2;
    m->barriers[0] = (Rectangle){(float)3*SCREEN_SIZE_X/5, 16, 32, 0.6*SCREEN_SIZE_Y};
    m->barriers[1] = (Rectangle){(float)2*SCREEN_SIZE_X/5, (float)SCREEN_SIZE_Y/2, 32, SCREEN_SIZE_Y};
    m->door = (Rectangle){SCREEN_SIZE_X-(SCREEN_BORDER+5), (float)SCREEN_SIZE_Y/2-24, SCREEN_BORDER, 48};
    m->prev_door = (Rectangle){5, (float)SCREEN_SIZE_Y/2-24, SCREEN_BORDER, 48};
    m->num_enemies = 3;
    m->door_locked = 1;

    for(int i=0; i< m->num_enemies; i++){
        enemy_setup(g, &m->enemies[i], 2, *m);
    }
    m->enemies[0].has_key = 1;
    m->prev_map = 0;
    m->next_map = 2;
}

void map2_setup(Game *g){
    Map *m = &g->maps[2];
    m->num_barriers = 6;
    m->barriers[0] = (Rectangle){16, 176, 200, 32};
    m->barriers[1] = (Rectangle){16, 272, 200, 32};
    m->barriers[2] = (Rectangle){600, 176, 200, 32};
    m->barriers[3] = (Rectangle){600, 272, 200, 32};
    m->barriers[4] = (Rectangle){(float)SCREEN_SIZE_X/2-168, 16, 336, 32};
    m->barriers[5] = (Rectangle){(float)SCREEN_SIZE_X/2-168, SCREEN_SIZE_Y-48, 336, 32};
    m->door = (Rectangle){SCREEN_SIZE_X-(SCREEN_BORDER+5), (float)SCREEN_SIZE_Y/2-24, SCREEN_BORDER, 48};
    m->prev_door = (Rectangle){5, (float)SCREEN_SIZE_Y/2-24, SCREEN_BORDER, 48};
    m->num_enemies = 4;
    m->door_locked = 1;

    for(int i=0; i< m->num_enemies; i++){
        enemy_setup(g, &m->enemies[i], 3, *m);
    }
    m->enemies[0].has_key = 1;
    m->prev_map = 1;
    m->next_map = 3;
}

void map3_setup(Game *g){
    Map *m = &g->maps[3];
    m->num_barriers = 3;
    m->barriers[0] = (Rectangle){(float)SCREEN_SIZE_X/4-16, 16, 32, 0.6 * SCREEN_SIZE_Y};
    m->barriers[1] = (Rectangle){(float)2*SCREEN_SIZE_X/4-16, 16, 32, 0.6 * SCREEN_SIZE_Y};
    m->barriers[2] = (Rectangle){(float)3*SCREEN_SIZE_X/4-16, 16, 32, 0.6 * SCREEN_SIZE_Y};
    m->door = (Rectangle){SCREEN_SIZE_X-(SCREEN_BORDER+5), (float)SCREEN_SIZE_Y/2-24, SCREEN_BORDER, 48};
    m->prev_door = (Rectangle){5, (float)SCREEN_SIZE_Y/2-24, SCREEN_BORDER, 48};
    m->num_enemies = 4;
    m->door_locked = 1;

    for(int i=0; i< m->num_enemies; i++){
        enemy_setup(g, &m->enemies[i], 3, *m);
    }
    m->enemies[0].has_key = 1;
    m->prev_map = 2;
    m->next_map = 4;
}

void map4_setup(Game *g){
    Map *m = &g->maps[4];
    m->num_barriers = 7;
    m->barriers[0] = (Rectangle){208, 128, 384, 32};
    m->barriers[1] = (Rectangle){208, 320, 160, 32};
    m->barriers[2] = (Rectangle){432, 320, 160, 32};
    m->barriers[3] = (Rectangle){176, 128, 32, 224};
    m->barriers[4] = (Rectangle){592, 128, 32, 224};
    m->barriers[5] = (Rectangle){176, 16, 32, 64};
    m->barriers[6] = (Rectangle){592, 400, 32, 64};
    m->door = (Rectangle){SCREEN_SIZE_X-(SCREEN_BORDER+5), (float)SCREEN_SIZE_Y/2-24, SCREEN_BORDER, 48};
    m->prev_door = (Rectangle){5, (float)SCREEN_SIZE_Y/2-24, SCREEN_BORDER, 48};
    m->num_enemies = 4;
    m->door_locked = 1;

    for(int i=0; i< m->num_enemies; i++){
        enemy_setup(g, &m->enemies[i], 4, *m);
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
    m->door = (Rectangle){SCREEN_SIZE_X-(SCREEN_BORDER+5), (float)SCREEN_SIZE_Y/2-24, SCREEN_BORDER, 48};
    m->prev_door = (Rectangle){5, (float)SCREEN_SIZE_Y/2-24, SCREEN_BORDER, 48};
    m->num_enemies = 4;
    m->door_locked = 1;

    for(int i=0; i< m->num_enemies; i++){
        enemy_setup(g, &m->enemies[i], 5, *m);
    }
    m->enemies[0].has_key = 1;
    m->prev_map = 4;
    m->next_map = 6;
}

void map6_setup(Game *g){
    Map *m = &g->maps[6];
    m->num_barriers = 5;
    m->barriers[0] = (Rectangle){16, 288, 272, 32};
    m->barriers[1] = (Rectangle){288, 288, 32, 96};
    m->barriers[2] = (Rectangle){512, 160, 272, 32};
    m->barriers[3] = (Rectangle){480, 96, 32, 96};
    m->barriers[4] = (Rectangle){384, 224, 32, 32};
    m->door = (Rectangle){SCREEN_SIZE_X-(SCREEN_BORDER+5), (float)SCREEN_SIZE_Y/2-24, SCREEN_BORDER, 48};
    m->prev_door = (Rectangle){5, (float)SCREEN_SIZE_Y/2-24, SCREEN_BORDER, 48};
    m->num_enemies = 5;
    m->door_locked = 7;

    for(int i=0; i< m->num_enemies; i++){
        enemy_setup(g, &m->enemies[i], 5, *m);
    }
    m->enemies[0].has_key = 1;
    m->prev_map = 5;
    m->next_map = 7;
}

void map7_setup(Game *g){
    Map *m = &g->maps[7];
    m->num_barriers = 3;
    m->barriers[0] = (Rectangle){160, 112, 480, 32};
    m->barriers[1] = (Rectangle){160, 336, 480, 32};
    m->barriers[2] = (Rectangle){384, 192, 32, 96};
    m->door = (Rectangle){SCREEN_SIZE_X-(SCREEN_BORDER+5), (float)SCREEN_SIZE_Y/2-24, SCREEN_BORDER, 48};
    m->prev_door = (Rectangle){5, (float)SCREEN_SIZE_Y/2-24, SCREEN_BORDER, 48};
    m->num_enemies = 6;
    m->door_locked = 7;

    for(int i=0; i< m->num_enemies; i++){
        enemy_setup(g, &m->enemies[i], 5, *m);
    }
    m->enemies[0].has_key = 1;
    m->prev_map = 6;
    m->next_map = 8;
}

void boss_scene_setup(Game *g){
    Map *m = &g->maps[8];
    m->num_barriers = 0;
    m->num_enemies = 5;
    for(int i=0;i<5;i++){
        for(int j=0;j<10;j++){
            m->enemies[i].bullets[j] = (Bullet){
                (Rectangle){SCREEN_SIZE_X, SCREEN_SIZE_Y, 6, 6},
                0,
                8,
                (Vector2){0, 0},
            };
        }
    }
    m->enemies[0].type = SonOfMan;
    m->enemies[0].hp = g->en_globals.enemy_defs[SonOfMan][EnHP];
    m->enemies[0].bullets_left = g->en_globals.enemy_defs[SonOfMan][EnMaxBullets];
    for(int i=1;i<=4;i++){
        m->enemies[i].type = Crystal;
        m->enemies[i].current_frame = 0;
        m->enemies[i].hp = g->en_globals.enemy_defs[Crystal][EnHP];
        m->enemies[i].bullets_left = g->en_globals.enemy_defs[Crystal][EnMaxBullets];
    }
    m->enemies[0].pos = (Rectangle){(float)SCREEN_SIZE_X/2-256, -16, 512, 232};
    m->enemies[1].pos = (Rectangle){60, 80, 80, 80};
    m->enemies[2].pos = (Rectangle){SCREEN_SIZE_X-140, 80, 80, 80};
    m->enemies[3].pos = (Rectangle){160, 384, 80, 80};
    m->enemies[4].pos = (Rectangle){SCREEN_SIZE_X-240, 384, 80, 80};
}