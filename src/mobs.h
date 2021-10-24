#ifndef mobs_h_
#define mobs_h_

#include <stdint.h>
#include "world.h"

typedef struct PassiveMob {
    int x, y;
    int dx, dy;
    uint8_t dir;
    uint16_t id;
    int hp;
    uint8_t inGame;
    uint8_t render;
} PassiveMob;


typedef struct GameState GameState;

void spawnPassiveMob(GameState * game, PassiveMob * mob, int id, int x, int y);
void updatePassiveMob(GameState * game, PassiveMob * mob);


#endif