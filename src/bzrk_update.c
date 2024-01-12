#include <math.h>
#include <stdlib.h>
#include "berzerk.h"

//------------------------------------------------------------------------------------
// Definition of game logic functions
//------------------------------------------------------------------------------------

void updateHeroPos(Game *g){
    Hero *h = &g->hero;
    Map *m = &g->maps[g->curr_map];
    static int frame_counter = 0;

    updateBulletPos(g, &h->bullets[0], &h->bullets_left, 3-g->difficulty);
    updateBulletPos(g, &h->bullets[1], &h->bullets_left, 3-g->difficulty);
    
    if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)){
        if(h->pos.x > SCREEN_BORDER)
            h->pos.x -= h->speed;
        if(barrierCollision(m, &h->pos)) h->pos.x += h->speed;
        h->direction = KEY_LEFT;
        frame_counter++;
        
    } else if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)){
        if(h->pos.x + h->pos.width < SCREEN_WIDTH - SCREEN_BORDER)
            h->pos.x += h->speed;
        if(barrierCollision(m, &h->pos)) h->pos.x -= h->speed;
        h->direction = KEY_RIGHT;
        frame_counter++;

    } else if(IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)){
        if(h->pos.y > SCREEN_BORDER)
            h->pos.y -= h->speed;
        if(barrierCollision(m, &h->pos)) h->pos.y += h->speed;
        h->direction = KEY_UP;
        frame_counter++;

    } else if(IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)){
        if(h->pos.y + h->pos.height < SCREEN_HEIGHT - SCREEN_BORDER)
            h->pos.y += h->speed;
        if(barrierCollision(m, &h->pos)) h->pos.y -= h->speed;
        h->direction = KEY_DOWN;
        frame_counter++;
    }

    if(frame_counter >= 10){
        frame_counter = 0;
        h->current_frame++;
    }
    if(h->current_frame >= 4) h->current_frame = 0;
}

void updateEnemy(Game *g, Enemy *e){
    Map *m = &g->maps[g->curr_map];
    int sr_x = abs((int)(g->hero.pos.x-e->pos.x));
    int sr_y = abs((int)(g->hero.pos.y-e->pos.y));
    static int frame_counter = 0;

    //Update bullet information
    if(!(rand()%50) && !e->walking) shootEnemyBullet(g, e);

    if(sr_y < EN_DETECT_SPAN && e->bullets_left){
        e->walking = 0;
        if(g->hero.pos.x < e->pos.x)
            e->direction = KEY_LEFT;
        else
            e->direction = KEY_RIGHT;
        
    } else if(sr_x < EN_DETECT_SPAN && e->bullets_left){
        e->walking = 0;
        if(g->hero.pos.y < e->pos.y)
            e->direction = KEY_UP;
        else
            e->direction = KEY_DOWN;
    
    // Movement
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
            if(barrierCollision(m, &e->pos)){
                e->pos.x += e->speed;
                e->direction = KEY_RIGHT + (rand()%4);
            }

        } else if(e->direction == KEY_RIGHT){
            if(e->pos.x + e->pos.width < SCREEN_WIDTH - SCREEN_BORDER)
                e->pos.x += e->speed;
            else
                e->direction = KEY_RIGHT + (rand()%4);
            
            if(barrierCollision(m, &e->pos)){
                e->pos.x -= e->speed;
                e->direction = KEY_RIGHT + (rand()%4);
            }
            
        } else if(e->direction == KEY_UP){
            if(e->pos.y > SCREEN_BORDER)
                e->pos.y -= e->speed;
            else
                e->direction = KEY_RIGHT + (rand()%4);
            
            if(barrierCollision(m, &e->pos)){
                e->pos.y += e->speed;
                e->direction = KEY_RIGHT + (rand()%4);
            }
        } else if(e->direction == KEY_DOWN){
            if(e->pos.y + e->pos.height < SCREEN_HEIGHT - SCREEN_BORDER)
                e->pos.y += e->speed;
            else
                e->direction = KEY_RIGHT + (rand()%4);
            
            if(barrierCollision(m, &e->pos)){
                e->pos.y -= e->speed;
                e->direction = KEY_RIGHT + (rand()%4);
            }
        }
        if(!(rand()%50)) // 2% chance of enemy changing direction
            e->direction = KEY_RIGHT + (rand()%4);
        if(frame_counter >= 15){
            frame_counter = 0;
            e->current_frame++;
        }
        if(e->current_frame >= 4) e->current_frame = 0;
    }

}

void updateBoss(Game *g, Enemy *e){
    static int frame_counter = 0;
    frame_counter++;
    if(!(rand()%((e->type==SonOfMan)?15:75))) shootBossBullet(g, e);
    if(frame_counter >= 15){
            frame_counter = 0;
            e->current_frame++;
    }
}

void shootHeroBullet(Game *g){
    Hero *h = &g->hero;
    Bullet *hb = &h->bullets[2 - h->bullets_left];

    if(h->bullets_left>0){
        PlaySound(g->general_sfx[SnPlayerAttack]);
        hb->pos.x = h->pos.x-1 + (float)(STD_SIZE_X)/2;
        hb->pos.y = h->pos.y-1 + (float)(STD_SIZE_Y)/2;
        hb->active = 1;
        h->bullets_left--;
        h->current_frame++;
        if(h->current_frame >= 4) h->current_frame = 0;
        switch(h->direction){
            case KEY_UP:
                hb->direction2 = (Vector2){0, -1};
                break;

            case KEY_DOWN:
                hb->direction2 = (Vector2){0, 1};
                break;

            case KEY_LEFT:
                hb->direction2 = (Vector2){-1, 0};
                break;

            case KEY_RIGHT:
                hb->direction2 = (Vector2){1, 0};
                break;
        }
    }
}

void shootEnemyBullet(Game *g, Enemy *e){
    Bullet *eb = &e->bullets[g->en_globals.enemy_defs[e->type][EnMaxBullets] - e->bullets_left];
    if(e->bullets_left>0){
        PlaySound(g->general_sfx[SnEnemyAttack]);
        eb->pos.x = e->pos.x-3 + (float)(g->en_globals.enemy_gfx[e->type].width)/32;
        eb->pos.y = e->pos.y;
        eb->active = 1;
        e->bullets_left--;
        switch(e->direction){
            case KEY_UP:
                eb->direction2 = (Vector2){0, -1};
                break;

            case KEY_DOWN:
                eb->direction2 = (Vector2){0, 1};
                break;

            case KEY_LEFT:
                eb->direction2 = (Vector2){-1, 0};
                break;

            case KEY_RIGHT:
                eb->direction2 = (Vector2){1, 0};
                break;
        }
    }
}

void shootBossBullet(Game *g, Enemy *e){
    Bullet *eb = &e->bullets[g->en_globals.enemy_defs[e->type][EnMaxBullets] - e->bullets_left];
    float x;
    if(e->bullets_left>0){
        PlaySound(g->general_sfx[SnBossAttack]);
        eb->active = 1;
        e->bullets_left--;
        if(e->type == SonOfMan){
            x = PI * -((float)rand()/(float)(RAND_MAX));
            eb->pos.x = (float)SCREEN_WIDTH/2;
            eb->pos.y = e->pos.y + (float)2*g->en_globals.enemy_gfx[e->type].height/3;
        } else{
            x = 2* PI * ((float)rand()/(float)(RAND_MAX));
            eb->pos.x = e->pos.x + (float)g->en_globals.enemy_gfx[e->type].width/8;
            eb->pos.y = e->pos.y;
        }
        eb->direction2 = (Vector2){cos(x), -sin(x)};
    }
}

// Updates one bullet
void updateBulletPos(Game *g, Bullet *b, int *bullets_left, int max_bullets){
    Map *m = &g->maps[g->curr_map];

    Bullet blank_bullet = (Bullet){
        (Rectangle){SCREEN_WIDTH, SCREEN_HEIGHT, 6, 6},
        0,
        8,
        {0, 0}
    };

    if(b->active){
        b->pos.x += b->speed*b->direction2.x;
        b->pos.y += b->speed*b->direction2.y;
        if(
            (b->pos.x < SCREEN_BORDER) || (b->pos.x > SCREEN_WIDTH - SCREEN_BORDER) ||
            (b->pos.y < SCREEN_BORDER) || (b->pos.y > SCREEN_HEIGHT - SCREEN_BORDER)
        ){
            *b = blank_bullet;
            *bullets_left = max_bullets;
        }
        if(barrierCollision(m, &b->pos)){
            *b = blank_bullet;
            *bullets_left = max_bullets;
        }
    }
}