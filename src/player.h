#ifndef player_h_
#define player_h_

#include <sys/time.h>
#include "world.h"

// Player animation statuses.
#define STILL 0
#define RUNNING 1

// Player modes
#define CONSTRUCTION 0
#define COMBAT 1

struct Player {
    int x, y;
    int dx, dy;
    int dxTarget;
    int width, height;
    int sprite_x, sprite_y;
    int facing;
    int animation;
    int frame;
    struct timeval frameTimer;

    int chunk_x, chunk_y;
    int block_x1, block_x2, block_y1, block_y2;
    struct Block * detectorBlock[4][4];
    int justCollided[4];
    int colliding[4];

    int resources[1024];
    uint16_t pinnedRes[16];
    struct timeval miningTimer;
    int mining;
    int miningFrame;
    struct Block * prevMouseBlock;
    int canPlace;
    int canBreak;
    short int mode;
};

typedef struct Player Player;


typedef struct GameState GameState;

// Update player physics simulation.
void updatePlayer(GameState * game);

#endif