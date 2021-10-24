#include "mobs.h"

#include "game.h"

#include "defines.h"

void spawnPassiveMob(GameState * game, PassiveMob * mob, int id, int x, int y) {
    mob->inGame = 1;
    mob->id = id;
    mob->hp = game->PMOB_prop[id]->hpMax;
    mob->x = x;
    mob->y = y;
    mob->dx = 0;
    mob->dy = 0;
    mob->dir = L;
    mob->render = 1;

    return;
}

void updatePassiveMob(GameState * game, PassiveMob * mob) {
    PassiveMobProperties * p = game->PMOB_prop[mob->id];
    mob->dy += game->gravity;

    Block * detectorBlock[4][4];
    int s_y = snap(mob->y + 64, 64);
    detectorBlock[S][W] = findBlock(game->loadedChunk, snap(mob->x, 64), s_y);
    detectorBlock[S][E] = findBlock(game->loadedChunk, snap(mob->x + p->width, 64), s_y);
    
    detectorBlock[W][W] = findBlock(game->loadedChunk, snap(mob->x, 64), snap(mob->y + 32, 64));
    detectorBlock[E][E] = findBlock(game->loadedChunk, snap(mob->x + p->width, 64), snap(mob->y + 32, 64));

    detectorBlock[N][W] = findBlock(game->loadedChunk, snap(mob->x, 64), snap(mob->y - 32, 64));
    detectorBlock[N][E] = findBlock(game->loadedChunk, snap(mob->x + p->width, 64), snap(mob->y - 32, 64));
    
    if (game->BLK_prop[detectorBlock[S][W]->block_id]->opacity || game->BLK_prop[detectorBlock[S][E]->block_id]->opacity) {
        mob->dy = 0;
        mob->y = s_y - 64;
    }
    

    if (game->BLK_prop[detectorBlock[W][W]->block_id]->opacity) {
        if (game->BLK_prop[detectorBlock[N][W]->block_id]->opacity) {
            mob->dir = R;
        }
        else {
            mob->dy = -10;
        }
    }
    else if (game->BLK_prop[detectorBlock[E][E]->block_id]->opacity) {
        if (game->BLK_prop[detectorBlock[N][E]->block_id]->opacity) {
            mob->dir = L;
        }
        else {
            mob->dy = -10;
        }
    }


    if (mob->dir == R) {
        mob->dx = 1;
    }
    if (mob->dir == L) {
        mob->dx = -1;
    }


    mob->x += mob->dx;
    mob->y += mob->dy;

    return;
}