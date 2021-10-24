#ifndef world_h_
#define world_h_

#include <stdint.h>

// Information for a single block in game.
struct Block {
    short int block_id;
    uint8_t variant;
    uint8_t light;
};

// Chunk data (blocks and coordinates).
struct Chunk {
    int x, y;
    struct Block block[16][16];
};

// Node in data structure used to store chunks and link chunks together.
struct ChunkNode {
    struct Chunk data;
    int defined;
    int spawnedTrees;
    struct ChunkNode * next[4];
};




typedef struct Block Block;
typedef struct Chunk Chunk;
typedef struct ChunkNode ChunkNode;
typedef struct GameState GameState;

int findHeight(int x);

//void printChunkBlocks(ChunkNode * chunk);

//void fillChunk(Chunk * chunk, int block_id);
void initChunk(GameState * game, ChunkNode * chunk, int chunk_x, int chunk_y);
void createChunk(GameState * game, ChunkNode * node, int target);

void spawnTrees(GameState * game, ChunkNode * chunk);

void createUndefinedChunks(ChunkNode * nodeStart, int target, int direction);

void initChunkMatrix(GameState * game);

// RADIUS MUST BE AT LEAST 2
void updateChunks(GameState * game, int radius);

#endif
