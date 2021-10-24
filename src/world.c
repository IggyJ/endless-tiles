#include "world.h"

#include <stdio.h>
#include <stdlib.h>
#include "sdnoise1234.h"
#include "game.h"
#include "properties.h"
#include "player.h"

#include "defines.h"
#define _USE_MATH_DEFINES


// Returns between -1.0f and 1.0f
float fractalNoise2D(float x, float y, float lacunarity, float persistance, int octaves) {
    float val = 0.0f;
    for (int oct = 0; oct < octaves; oct++) {
        val += pow(persistance, (float)oct)*sdnoise2(pow(lacunarity, (float)oct)*x, pow(lacunarity, (float)oct)*y, NULL, NULL);
    }
    return val;
}

// Returns between 0.0f and 1.0f
float ridgedMulti2D(float x, float y, float lacunarity, float persistance, int octaves) {
    return fabs(fractalNoise2D(x, y, lacunarity, persistance, octaves));
}

int findHeight(int x) {
    float x_calc = (float)x/60000.0f;
    float y_calc = 1200.0f + 2000.0f*(fractalNoise2D(x_calc, 0.0f, 2.0f, 0.8f, 3));
    int y = (int)y_calc;
    return y;
}

int noiseMap(int x, int y, int spread, float frequency, int offset) {
    float x_calc = (float)(x+offset)/spread;
    float y_calc = (float)(y+offset)/spread;
    float z_calc = (sdnoise2(x_calc, y_calc, NULL, NULL) + 1.0f) / 2.0f;
    int present = 0;
    if (z_calc > frequency) {
        present = 1;
    }
    return present;
}

int makeCave(int x, int y, int h) {
    int carve = 1;
    float thickness = ((float)(y - h))/5000000.0f;
    if (thickness > 0.1f) {
        thickness = 0.1f;
    }
    float x_calc = (float)x/5000.0f;
    float y_calc = (float)y/5000.0f;
    if ( ridgedMulti2D(x_calc, y_calc, 2.0f, 0.5f, 3) > 0.1f+thickness) {
        carve = 0;
    }
    return carve;
}

void fillChunk(Chunk * chunk, int block_id) {
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            chunk->block[x][y].block_id = block_id;
        }
    }
    //printf("Filled chunk %p with block %d.\n", chunk, block_id);
    return;
}

/*
void printChunkBlocks(ChunkNode * chunk) {
    printf("------ BLOCK COORDINATES FOR CHUNK (%d, %d) ------\n", chunk->data.x, chunk->data.y);
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            printf("(%d, %d) ", chunk->data.block[x][y].x, chunk->data.block[x][y].y);
        }
        printf("\n");
    }
    printf("\n");
    return;
}
*/

void spawnTree(GameState * game, ChunkNode * chunk, int block_x, int block_y) {
    int h = 4 + rand() % 3;
    int r = 1 + rand() % 2;
    for (int i = 0; i < h; i++) {
        Block * blk = findBlock(chunk, block_x, block_y - 64*i);
        blk->block_id = 4;
        blk->variant = rand() % game->BLK_prop[4]->textureVariants;
    }
    int top_y = block_y - 64*(h-1);

    findBlock(chunk, block_x+64, top_y)->block_id = 5;
    findBlock(chunk, block_x-64, top_y)->block_id = 5;
    findBlock(chunk, block_x, top_y-64)->block_id = 5;
    findBlock(chunk, block_x, top_y-128)->block_id = 5;

    findBlock(chunk, block_x+64, top_y-64)->block_id = 5;
    findBlock(chunk, block_x-64, top_y-64)->block_id = 5;

    findBlock(chunk, block_x+64, top_y)->variant = 0;
    findBlock(chunk, block_x-64, top_y)->variant = 0;
    findBlock(chunk, block_x, top_y-64)->variant = 0;
    findBlock(chunk, block_x, top_y-128)->variant = 0;

    findBlock(chunk, block_x+64, top_y-64)->variant = 0;
    findBlock(chunk, block_x-64, top_y-64)->variant = 0;

    return;
}

void spawnTrees(GameState * game, ChunkNode * chunk) {
    for (int x = 0; x < 16; x++) {
        int h = findHeight(chunk->data.x + x*64);
        for (int y = 0; y < 16; y++) {
            int bx = chunk->data.x + x*64;
            int by = chunk->data.y + y*64;
            if (by >= h-128 && by <= h-65) {
                if (noiseMap(x, y, 1, 0.65, 0)) {
                    spawnTree(game, chunk, bx, by);
                }
            }
        }
    }
    chunk->spawnedTrees = 1;
    return;
}

void initChunk(GameState * game, ChunkNode * chunk, int chunk_x, int chunk_y) {
    if (chunk->defined == 0) {
        chunk->data.x = chunk_x;
        chunk->data.y = chunk_y;
        int blk_id;
        for (int x = 0; x < 16; x++) {
            int h = findHeight(chunk->data.x + x*64);
            for (int y = 0; y < 16; y++) {
                int bx = chunk->data.x + x*64;
                int by = chunk->data.y + y*64;
                //chunk->data.block[x][y].x = bx;
                //chunk->data.block[x][y].y = by;

                // Default block to air
                blk_id = 0;

                for (int id = 1; id < game->BLK_amount; id++) {
                    if (game->BLK_prop[id]->spawnable) {
                        BlockProperties * p = game->BLK_prop[id];
                        int inRange = 0;

                        if (p->minMult == 0 && p->minOffset == 0) {
                            if (by <= (p->maxMult)*h + p->maxOffset) {
                                inRange = 1;
                            }
                        }
                        else if (p->maxMult == 0 && p->maxOffset == 0) {
                            if (by >= (p->minMult)*h + p->minOffset) {
                                inRange = 1;
                            }
                        }
                        else if (by >= (p->minMult)*h + p->minOffset && by <= (p->maxMult)*h + p->maxOffset) {
                            inRange = 1;
                        }

                        if (inRange) {
                            if (p->useNoisemap) {
                                if (noiseMap(bx, by, (float)p->spread, (float)p->frequency/100.0, 1000*id)) {
                                    blk_id = p->id;
                                }
                            }
                            else {
                                blk_id = p->id;
                            }
                        }
                    }
                }

                if (by >= h+512) {
                    if (makeCave(bx, by, h)) {
                        blk_id = 0;
                    }
                }

                chunk->data.block[x][y].block_id = blk_id;
                if (game->BLK_prop[blk_id]->textureVariants > 0) {
                    chunk->data.block[x][y].variant = rand() % game->BLK_prop[blk_id]->textureVariants;
                }
                else {
                    chunk->data.block[x][y].variant = 0;
                }
            }
        }
        chunk->defined = 1;
    }
    else {
        printf("Error, tried to redefine chunk.\n");
    }
    
    return;
}


// Traverses matrix in direction of "direction", creating new undefined nodes towards "target"
void createUndefinedChunks(ChunkNode * nodeStart, int target, int direction) {
    ChunkNode * node = nodeStart;
    ChunkNode * lastCreated = NULL;
    int opposite[] = {3, 2, 1, 0};

    while (node != NULL) {
        //printf("At node %p with connections: %p %p %p %p\n", node, node->next[0], node->next[1], node->next[2], node->next[3]);
        if (node->next[target] == NULL) {
            node->next[target] = malloc(sizeof(ChunkNode));
            node->next[target]->defined = 0;
            node->next[target]->spawnedTrees = 0;
            //printf("Created undefined chunk at %p\n", node->next[target]);

            node->next[target]->next[opposite[target]] = node;
            node->next[target]->next[target] = NULL;
            node->next[target]->next[direction] = NULL;
            node->next[target]->next[opposite[direction]] = NULL;
        }

        // Link last new node to new new node
        if (lastCreated != NULL) {
            lastCreated->next[direction] = node->next[target];
            node->next[target]->next[opposite[direction]] = lastCreated;
        }
        lastCreated = node->next[target];

        if (node == node->next[direction]) {
            printf("Loop error.\n");
            break;
        }

        // Traverse in direction
        node = node->next[direction];
        //printf("Going to %p\n", node);
    }

    return;
}

void createChunk(GameState * game, ChunkNode * node, int target) {
    node->next[target] = malloc(sizeof(ChunkNode));
    node->next[target]->defined = 0;
    //printf("Created new chunk at %p\n", node->next[target]);

    int opposite[] = {3, 2, 1, 0};
    int directions[][2] = {{1, 2}, {0, 3}, {0, 3}, {1, 2}};
    node->next[target]->next[opposite[target]] = node;
    node->next[target]->next[target] = NULL;
    node->next[target]->next[directions[target][0]] = NULL;
    node->next[target]->next[directions[target][1]] = NULL;

    int chunk_x[] = {0, -1024, 1024, 0};
    int chunk_y[] = {-1024, 0, 0, 1024};
    initChunk(game, node->next[target], (node->data.x + chunk_x[target]), (node->data.y + chunk_y[target]));
    node->next[target]->defined = 1;

    //printf("Creating undefined chunks towards %d in directions %d and %d\n", target, directions[target][0], directions[target][1]);
    createUndefinedChunks(node, target, directions[target][0]);
    createUndefinedChunks(node, target, directions[target][1]);

    //printf("done creating new chunks\n");
    return;
}

void initChunkMatrix(GameState * game) {
    game->centerChunk = malloc(sizeof(ChunkNode));
    game->centerChunk->defined = 0;
    for (int i = 0; i <= 3; i++) {
        game->centerChunk->next[i] = NULL;
        //createChunk(game, game->centerChunk, i);
    }
    initChunk(game, game->centerChunk, 0, 0);
    //printf("Created centre chunk at %p\n", game->centerChunk);
    game->loadedChunk = game->centerChunk;

    return;
}

void updateColumn(GameState * game, ChunkNode * startNode, int chunk_x, int chunk_y, int radius) {
    ChunkNode * node = startNode;
    // Create chunks above
    for (int y = 1; y <= radius; y++) {
        if (node->next[N] == NULL) {
            createChunk(game, node, N);
        }
        if ( !(node->next[N]->defined) ) {
            initChunk(game, node->next[N], chunk_x, chunk_y - 1024*y);
        }
        node = node->next[N];
    }

    // Create chunks below
    node = startNode;
    for (int y = 1; y <= radius; y++) {
        if (node->next[S] == NULL) {
            createChunk(game, node, S);
        }
        if ( !(node->next[S]->defined) ) {
            initChunk(game, node->next[S], chunk_x, chunk_y + 1024*y);
        }
        node = node->next[S];
    }
    return;
}

// ---------- UPDATE ----------

// RADIUS MUST BE AT LEAST 2
void updateChunks(GameState * game, int radius) {
    //printf("updating chunks\n");
    int old_chunk_x = game->player.chunk_x;
    int old_chunk_y = game->player.chunk_y;
    game->player.chunk_x = snap(game->player.x, 1024);
    game->player.chunk_y = snap(game->player.y, 1024);

    // Find direction the player went
    if (game->player.chunk_x != old_chunk_x || game->player.chunk_y != old_chunk_y) {
        int dir;
        if (game->player.chunk_x > old_chunk_x) {
            dir = E;
        }
        else if (game->player.chunk_x < old_chunk_x) {
            dir = W;
        }
        else if (game->player.chunk_y > old_chunk_y) {
            dir = S;
        }
        else if (game->player.chunk_y < old_chunk_y) {
            dir = N;
        }

        // Make sure chunk in that direction is created
        if (game->loadedChunk->next[dir] == NULL) {
            createChunk(game, game->loadedChunk, dir);
        }

        // Change loaded chunk to the one the player is in
        game->loadedChunk = game->loadedChunk->next[dir];

        // Make sure loaded chunk has been initialised
        if ( !(game->loadedChunk->defined) ) {
            initChunk(game, game->loadedChunk, game->player.chunk_x, game->player.chunk_y);
        }
    }


    ChunkNode * node = game->loadedChunk;
    int chunk_x = game->player.chunk_x;
    int chunk_y = game->player.chunk_y;


    // Update central column
    updateColumn(game, node, chunk_x, chunk_y, radius);

    // Update left columns
    for (int x = 1; x <= radius; x++) {
        if (node->next[W] == NULL) {
            createChunk(game, node, W);
        }
        if ( !(node->next[W]->defined) ) {
            initChunk(game, node->next[W], game->player.chunk_x - 1024*x, game->player.chunk_y);
        }

        node = node->next[W];
        updateColumn(game, node, chunk_x - 1024*x, chunk_y, radius);
    }

    // Update right columns
    node = game->loadedChunk;
    for (int x = 1; x <= radius; x++) {
        if (node->next[E] == NULL) {
            createChunk(game, node, E);
        }
        if ( !(node->next[E]->defined) ) {
            initChunk(game, node->next[E], game->player.chunk_x + 1024*x, game->player.chunk_y);
        }

        node = node->next[E];
        updateColumn(game, node, chunk_x + 1024*x, chunk_y, radius);
    }
    
    
    if ( !(game->loadedChunk->spawnedTrees) ) {
        spawnTrees(game, game->loadedChunk);
    }
    node = game->loadedChunk->next[W];
    if ( !(node->spawnedTrees) ) {
        spawnTrees(game, node);
    }
    if ( !(node->next[N]->spawnedTrees) ) {
        spawnTrees(game, node->next[N]);
    }
    if ( !(node->next[S]->spawnedTrees) ) {
        spawnTrees(game, node->next[S]);
    }
    node = game->loadedChunk->next[E];
    if ( !(node->spawnedTrees) ) {
        spawnTrees(game, node);
    }
    if ( !(node->next[N]->spawnedTrees) ) {
        spawnTrees(game, node->next[N]);
    }
    if ( !(node->next[S]->spawnedTrees) ) {
        spawnTrees(game, node->next[S]);
    }
    

    return;
}
