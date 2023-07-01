#ifndef BERZERK_H

#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define BERZERK_H               1
#define STD_SIZE_X              32
#define STD_SIZE_Y              32
#define SCREEN_BORDER           16
#define SCREEN_SIZE_X           800
#define SCREEN_SIZE_Y           480
#define CHARACTER_NAME_SIZE     6
#define EN_SHOOTING_RANGE       20

enum EnemyStats{
    EnMaxBullets,
    EnSpeed,
    EnHP
};

enum EnemyTypes{
    Zombie,
    Knight,
    Soldier,
    Vampire,
    Magitek
};

typedef struct Bullet{
    Rectangle pos;
    int active;
    int direction;
    int speed;
} Bullet;

typedef struct Hero{
    Texture2D texture;
    Texture2D bullet_texture;
    Rectangle pos;
    Color color;
    Bullet bullets[2];
    char name[CHARACTER_NAME_SIZE+1];
    int bullets_left;
    int direction;
    int speed;
    int current_frame;
} Hero;

typedef struct Enemy{
    Rectangle pos;
    Bullet bullets[4];
    int hp;
    int walking;
    int type;
    int bullets_left;
    int speed;
    int direction;
    int current_frame;
    int draw_enemy;
    int has_key;
} Enemy;

typedef struct Map{
    Rectangle barriers[10];
    int num_barriers;
    Rectangle door;
    Rectangle prev_door;
    Color color;
    Enemy enemies[10];
    int num_enemies;
    int door_locked;
    int next_map;
    int prev_map;
} Map;

typedef struct EnemyGlobals{
    Texture2D enemy_gfx[5];
    Texture2D bullets_gfx;
    int enemy_defs[5][3]; // enemy_defs[EnemyTypes][EnemyStats]
} EnemyGlobals;

typedef struct Game{ 
    Map maps[10];
    int num_maps;
    int curr_map;
    Hero hero;
    Font font;
    EnemyGlobals en_globals;
    int difficulty;
    int gameover;
    clock_t timer;
} Game;

//------------------------------------------------------------------------------------
// Protótipos das funções do módulo
//------------------------------------------------------------------------------------

void InitGame(Game *g);         // Inicializa a variáveis do jogo
void UpdateGame(Game *g);       // Atualiza o jogo (um frame)
void DrawGame(Game *g);         // Desenha a tela (um frame)
void UpdateDrawFrame(Game *g);  // Atualiza e desenha

//------------------------------------------------------------------------------------
// Protótipos das funções auxiliares
//------------------------------------------------------------------------------------

void draw_st_text(Font font, char *str, float y_pos, Color color);
void draw_highscores(Texture2D bg, Font font, char names[3][7], int *scores);
void draw_hero(Hero h);
void draw_enemy(Enemy e, Texture2D texture);
void draw_borders(Game *g);
void draw_map(Game *g);
void update_hero_pos(Game *g);
void update_enemy(Game *g, Enemy *e);
void shoot_bullet(Game *g);
void shoot_enemy_bullet(Game *g, Enemy *e);
void update_bullet_pos(Game *g, Bullet *b, int *bullets_left,int max_bullets);

int barrier_collision(Map *m, Rectangle *t);
void enemy_setup(Game *g, Enemy *e, int max_type, Map m);
void map0_setup(Game *g);
void map1_setup(Game *g);
void map2_setup(Game *g);
void map3_setup(Game *g);
void map4_setup(Game *g);
void map5_setup(Game *g);

#endif