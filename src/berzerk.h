#ifndef BERZERK_H

#include <raylib.h>
#include <time.h>

#define BERZERK_H               1
#define STD_SIZE_X              32
#define STD_SIZE_Y              32
#define SCREEN_BORDER           16
#define SCREEN_WIDTH            800
#define SCREEN_HEIGHT           480
#define CHARACTER_NAME_SIZE     6
#define EN_DETECT_SPAN       20

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
    SnBossCry1,
    SnBossCry2,
    SnBossAttack
};

//------------------------------------------------------------------------------------
// Function prototypes
//------------------------------------------------------------------------------------

void initGame(Game *g);         // Initializes game variables
void unloadResources(Game *g);  // Clears resources loaded in initGame()
void updateGame(Game *g);       // Updates game state (single frame)
void drawGame(Game *g);         // Draw game screen (single frame)
void updateBossBattle(Game *g); // Update boss battle state (single frame)
void drawBossBattle(Game *g);   // Draws boss battle screen (single frame)

void drawCentrText(Font font, char *str, float y_pos, Color color);
void drawHighscores(Texture2D bg, Font font, char names[3][7], int *scores);
void drawHero(Hero h);
void drawEnemyBullets(Game *g, Enemy e);
void drawEnemy(Enemy e, Texture2D texture);
void drawBoss(Game *g);
void drawCrystal(Game *g, Enemy e);
void drawBorders(Game *g);
void drawMap(Game *g);

void updateHeroPos(Game *g);
void updateEnemy(Game *g, Enemy *e);
void updateBoss(Game *g, Enemy *e);
void shootHeroBullet(Game *g);
void shootEnemyBullet(Game *g, Enemy *e);
void shootBossBullet(Game *g, Enemy *e);
void updateBulletPos(Game *g, Bullet *b, int *bullets_left,int max_bullets);

int barrierCollision(Map *m, Rectangle *t);
void enemySetup(Game *g, Enemy *e, int max_type, Map m);
void map0Setup(Game *g);
void map1Setup(Game *g);
void map2Setup(Game *g);
void map3Setup(Game *g);
void map4Setup(Game *g);
void map5Setup(Game *g);
void map6Setup(Game *g);
void map7Setup(Game *g);
void bossSceneSetup(Game *g);

#endif