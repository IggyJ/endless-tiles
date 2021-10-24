#ifndef game_h_
#define game_h_

#include <SDL2/SDL.h>
#include "world.h"
#include "properties.h"
#include "player.h"
#include "interface.h"
#include "load.h"
#include "mobs.h"
#include "defines.h"


struct Mouse {
    int x, y;
    int left, right;
};


struct Controller {
    SDL_GameController * data;
    Uint8 buttonA;
    Uint8 dpad[4];
};


struct BlockCursor {
    int x, y;
    int sprite_x, sprite_y;
    int visible;
};

struct Cloud {
    int x, y;
    float fl_x;
};

struct GameState {
    // ENGINE ELEMENTS
    int running;
    struct SDL_Window * window;
    SDL_Renderer * renderer;
    struct timeval time;
    int controllerEnabled;
    struct Controller controller;

    // Resources and properties
    struct Textures textures;
    struct Fonts fonts;
    int RES_amount;
    int BLK_amount;
    short int PMOB_amount;
    ResourceProperties * RES_prop[BYTE_2];  // 65,536 max resources and blocks.
    BlockProperties * BLK_prop[BYTE_2];
    PassiveMobProperties * PMOB_prop[BYTE_1]; // 256 max mobs.
    int gravity;
    int enablePhysics;

    // Game objects
    struct Player player;
    struct Mouse mouse;
    struct BlockCursor blockCursor;
    struct Cloud cloud;

    // World
    struct ChunkNode * centerChunk;
    struct ChunkNode * loadedChunk;
    int paused;
    
    // Mobs
    PassiveMob passiveMobs[64];

    // Interface Elements
    Interface * ui;
    int enableChunkMap;
};


typedef struct Mouse Mouse;
typedef struct Controller Controller;
typedef struct BlockCursor BlockCursor;
typedef struct Cloud Cloud;
typedef struct GameState GameState;


int snap(int coordinate, int gridSize);
Block * findBlock(ChunkNode * chunkStart, int block_x, int block_y);
void update(GameState * game, SDL_Renderer * renderer);
void updateChunks(GameState * game, int radius);

double findTimeDiff(struct timeval * t1, struct timeval * t2);
void copyTime(struct timeval * destination, struct timeval * source);
void clearTime(struct timeval * time);

#endif
