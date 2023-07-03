#ifndef BERZERK_H

#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define BERZERK_H               1
#define STD_SIZE_X              32
#define STD_SIZE_Y              32
#define SCREEN_BORDER           16
#define SCREEN_SIZE_X           800
#define SCREEN_SIZE_Y           480
#define CHARACTER_NAME_SIZE     6
#define EN_SHOOTING_RANGE       20

typedef struct Bullet{
    Rectangle pos;
    int active;
    int speed;
    Vector2 direction2;
} Bullet;

typedef struct Hero{
    Texture2D texture;
    Texture2D bullet_texture;
    Rectangle pos;
    Bullet bullets[2];
    char name[CHARACTER_NAME_SIZE+1];
    int bullets_left;
    int direction;
    int speed;
    int current_frame;
} Hero;

typedef struct Enemy{
    Rectangle pos;
    Bullet bullets[20];
    int hp;
    int walking;
    int type;
    int bullets_left;
    int speed;
    int direction;
    int current_frame;
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
    Texture2D enemy_gfx[7];
    Texture2D bullets_gfx;
    int enemy_defs[7][3]; // enemy_defs[EnemyTypes][EnemyStats]
} EnemyGlobals;

typedef struct Game{ 
    Map maps[9];
    int num_maps;
    int curr_map;
    Hero hero;
    Font font;
    Sound general_sfx[6]; // general_sfx[GeneralSounds]
    EnemyGlobals en_globals;
    Texture2D wall_texture;
    Texture2D grnd_texture;
    int difficulty;
    int boss_trigger;
    int gameover;
    clock_t timer;
} Game;

enum EnemyStats{
    EnMaxBullets,
    EnSpeed,
    EnHP
};

enum EnemyTypes{
    Zombie,
    Soldier,
    Knight,
    Magitek,
    Vampire,
    SonOfMan,
    Crystal
};

enum GeneralSounds{
    SnPlayerAttack,
    SnEnemyAttack,
    SnEnemyDeath,
    SnSonOfMan1,
    SnSonOfMan2,
    SnBossAttack
};

//------------------------------------------------------------------------------------
// Protótipos das funções do módulo
//------------------------------------------------------------------------------------

void InitGame(Game *g);         // Inicializa a variáveis do jogo
void UpdateGame(Game *g);       // Atualiza o jogo (um frame)
void DrawGame(Game *g);         // Desenha a tela (um frame)
void UpdateBossBattle(Game *g); // Atualiza a batalha final (um frame)
void DrawBossBattle(Game *g);   // Desenha a batalha final (um frame)

//------------------------------------------------------------------------------------
// Protótipos das funções auxiliares
//------------------------------------------------------------------------------------

void draw_st_text(Font font, char *str, float y_pos, Color color);
void draw_highscores(Texture2D bg, Font font, char names[3][7], int *scores);
void draw_hero(Hero h);
void draw_enemy_bullets(Game *g, Enemy e);
void draw_enemy(Enemy e, Texture2D texture);
void draw_boss(Game *g);
void draw_crystal(Game *g, Enemy e);
void draw_borders(Game *g);
void draw_map(Game *g);
void update_hero_pos(Game *g);
void update_enemy(Game *g, Enemy *e);
void update_boss(Game *g, Enemy *e);
void shoot_bullet(Game *g);
void shoot_enemy_bullet(Game *g, Enemy *e);
void shoot_boss_bullet(Game *g, Enemy *e);
void update_bullet_pos(Game *g, Bullet *b, int *bullets_left,int max_bullets);

int barrier_collision(Map *m, Rectangle *t);
void enemy_setup(Game *g, Enemy *e, int max_type, Map m);
void map0_setup(Game *g);
void map1_setup(Game *g);
void map2_setup(Game *g);
void map3_setup(Game *g);
void map4_setup(Game *g);
void map5_setup(Game *g);
void map6_setup(Game *g);
void map7_setup(Game *g);
void boss_scene_setup(Game *g);

#endif