#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define STD_SIZE_X 30
#define STD_SIZE_Y 30
#define SCREEN_BORDER 4
#define SCREEN_SIZE_X 800
#define SCREEN_SIZE_Y 480
#define CHARACTER_NAME_SIZE 6

typedef struct Bullet{
    Rectangle pos;
    int active;
    int direction;
    int speed;
} Bullet;

typedef struct Hero{
    Texture2D texture;
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
    Texture2D texture;
    Rectangle pos;
    Color color;
    Bullet bullets[2];
    int type;
    int speed;
    int direction;
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

typedef struct Game{ 
    Map maps[10];
    int num_maps;
    int curr_map;
    Hero hero;
    int difficulty;
    int screenWidth;
    int screenHeight;
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

void draw_highscores(char names[3][7], int *scores);
void draw_borders(Game *g);
void draw_map(Game *g);
void update_enemy_pos(Game *g, Enemy *e);
void update_hero_pos(Game *g);
void shoot_bullet(Game *g);
void update_bullet_pos(Game *g, int index);

int barrier_collision(Map *m, Rectangle *t);
void map0_setup(Game *g);
void map1_setup(Game *g);
void map2_setup(Game *g);
