#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "renderer.h"
#include "interface.h"
#include "world.h"

#include "defines.h"


// Takes in coordinate component (x or y) and snaps it to grid size (roudning down)
int snap(int coordinate, int gridSize) {
    if (coordinate >= 0 || coordinate%gridSize == 0) {
        return ((coordinate/gridSize) * gridSize);
    }
    else {
        return (((gridSize-coordinate)/gridSize) * -gridSize);
    }
}

// Takes in block coordinates and returns pointer to block
// Starts search for block in specified chunk
Block * findBlock(ChunkNode * chunkStart, int block_x, int block_y) {
    Block * BlkPtr = NULL;
    ChunkNode * chunk = chunkStart;
    // If block is not in starting chunk, traverse towards block
    while (block_x - chunk->data.x >= 1024) {
        chunk = chunk->next[E];
    }
    while (block_x - chunk->data.x < 0) {
        chunk = chunk->next[W];
    }
    while (block_y - chunk->data.y >= 1024) {
        chunk = chunk->next[S];
    }
    while (block_y - chunk->data.y < 0) {
        chunk = chunk->next[N];
    }

    // Find index for chunk block matrix
    int index_x = (block_x - chunk->data.x)/64;
    int index_y = (block_y - chunk->data.y)/64;
    BlkPtr = &(chunk->data.block[index_x][index_y]);
    if (BlkPtr == NULL) {
        printf("Error, could not find block.\n");
    }
    
    return BlkPtr;
}

// returns difference in time in seconds (t1 - t2)
double findTimeDiff(struct timeval * t1, struct timeval * t2) {
    double dt_s = (double)(t1->tv_sec - t2->tv_sec);
    double dt_us = (double)(t1->tv_usec - t2->tv_usec);
    return dt_s + dt_us/1000000.0;
}

void copyTime(struct timeval * destination, struct timeval * source) {
    destination->tv_sec = source->tv_sec;
    destination->tv_usec = source->tv_usec;
    return;
}

// sets seconds and microseconds to 0
void clearTime(struct timeval * time) {
    time->tv_sec = 0L;
    time->tv_usec = 0L;
    return;
}

int intDist(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2-x1, 2) + pow(y2-y1, 2));
}


void update(GameState * game, SDL_Renderer * renderer) {
    //printf("updating\n");

    // Update game timer
    gettimeofday(&(game->time), NULL);

    // Fetch mouse position and state
    SDL_GetMouseState(&(game->mouse.x), &(game->mouse.y));
    game->mouse.left = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT);
    game->mouse.right = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT);

    // Fetch controller state
    if (game->controllerEnabled) {
        game->controller.buttonA = SDL_GameControllerGetButton(game->controller.data, SDL_CONTROLLER_BUTTON_A);
        game->controller.dpad[N] = SDL_GameControllerGetButton(game->controller.data, SDL_CONTROLLER_BUTTON_DPAD_UP);
        game->controller.dpad[S] = SDL_GameControllerGetButton(game->controller.data, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        game->controller.dpad[W] = SDL_GameControllerGetButton(game->controller.data, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        game->controller.dpad[E] = SDL_GameControllerGetButton(game->controller.data, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    }

    // ------------------------------------ GAME SIMULATION START ------------------------------------
    
    if ( !(game->paused) ) {
        SDL_ShowCursor(SDL_DISABLE);

        const Uint8 * state = SDL_GetKeyboardState(NULL);
        game->player.dxTarget = 0;
        game->player.animation = STILL;
        if (state[SDL_SCANCODE_D]) {
            game->player.dxTarget += 10;
            game->player.colliding[W] = 0;
            game->player.facing = R;
            game->player.animation = RUNNING;
        }
        if (state[SDL_SCANCODE_A]) {  
            game->player.dxTarget -= 10;
            game->player.colliding[E] = 0;
            game->player.facing = L;
            game->player.animation = RUNNING;
        }
        if (state[SDL_SCANCODE_W] || game->controller.buttonA || game->controller.dpad[N]) {
            if (game->player.colliding[S]) {
                game->player.dy = -15;
                game->player.y -= 2;
                game->player.colliding[S] = 0;
            }
        }
          // FOR DEBUGGING
        if ( !(game->enablePhysics) ) {
            if (state[SDL_SCANCODE_RIGHT]) {
                game->player.x += 20;
            }
            if (state[SDL_SCANCODE_LEFT]) {  
                game->player.x -= 20;
            }
            if (state[SDL_SCANCODE_UP]) {
                game->player.y -= 20;
            }
            if (state[SDL_SCANCODE_DOWN]) {
                game->player.y += 20;
            }
        }
        
    
        // ------------ UPDATE CHUNKS ------------

        updateChunks(game, 3);

        // ------------ PLAYER PHYSICS  ------------

        updatePlayer(game);

        for (int mob = 0; mob < 64; mob++) {
            if (game->passiveMobs[mob].inGame) {
                if (intDist(game->player.x, game->player.y, game->passiveMobs[mob].x, game->passiveMobs[mob].y) < 1024) {
                    updatePassiveMob(game, &(game->passiveMobs[mob]));
                    game->passiveMobs[mob].render = 1;
                }
                else {
                    game->passiveMobs[mob].render = 0;
                }
            }
        }
        

        

        // Camera effects
        
        int targetSprite_x = 640 - game->player.width/2 - game->player.dx*2;
        if (targetSprite_x > game->player.sprite_x) {
            game->player.sprite_x += 1;
        }
        if (targetSprite_x < game->player.sprite_x) {
            game->player.sprite_x -= 1;
        }
        
        
        int targetSprite_y = 360 - game->player.width/2 - game->player.dy*2;
        if (targetSprite_y > game->player.sprite_y) {
            game->player.sprite_y += 1;
        }
        if (targetSprite_y < game->player.sprite_y) {
            game->player.sprite_y -= 1;
        }
        
        //printf("updating cursor\n");
        // Update block cursor position
        game->blockCursor.x = snap((game->player.x - game->player.sprite_x) + game->mouse.x, 64);
        game->blockCursor.y = snap((game->player.y - game->player.sprite_y) + game->mouse.y, 64);
        game->blockCursor.sprite_x = (game->blockCursor.x - game->player.x) + game->player.sprite_x;
        game->blockCursor.sprite_y = (game->blockCursor.y - game->player.y) + game->player.sprite_y;

        // Placing and destroying block
        Block * mouseBlock = findBlock(game->loadedChunk, game->blockCursor.x, game->blockCursor.y);
        Block * blockAbove = findBlock(game->loadedChunk, game->blockCursor.x, (game->blockCursor.y - 64));
        Block * blockBelow = findBlock(game->loadedChunk, game->blockCursor.x, (game->blockCursor.y + 64));

        // Check if cursor is inside player
        if (game->blockCursor.x+64 > game->player.x && game->blockCursor.x < game->player.x+game->player.width &&
            game->blockCursor.y+64 > game->player.y && game->blockCursor.y < game->player.y+game->player.height
        ) {
            game->player.canPlace = 0;
        }
        else {
            game->player.canPlace = 1;
        }

        // Check if cursor is out of range
        if (intDist(game->blockCursor.x, game->blockCursor.y, game->player.x, game->player.y) > 256) {
            game->player.canPlace = 0;
            game->player.canBreak = 0;
        }
        else {
            game->player.canBreak = 1;
        }

        if (game->player.canBreak || game->player.canPlace) {
            game->blockCursor.visible = 1;
        }
        else {
            game->blockCursor.visible = 0;
        }

        if (game->mouse.left && mouseBlock->block_id == 0 && game->player.canPlace) {
            if (game->player.resources[0] > 0) {
                mouseBlock->block_id = 2;
                mouseBlock->variant = rand() % game->BLK_prop[mouseBlock->block_id]->textureVariants;
                game->player.resources[0]--;

                // Turn grass below to soil
                if (blockBelow->block_id == 1) {
                    blockBelow->block_id = 2;
                    blockBelow->variant = rand() % game->BLK_prop[2]->textureVariants;
                }
                updatePinnedRes(game);
                updateResPanel(game, game->ui->resPanel);
            }
        }
        if (game->mouse.right && mouseBlock->block_id != 0 && mouseBlock == game->player.prevMouseBlock && game->player.canBreak) {
            if (game->player.mining == 0) {
                copyTime(&(game->player.miningTimer), &(game->time));
            }
            game->player.mining = 1;
            
            if (findTimeDiff(&(game->time), &(game->player.miningTimer)) >= game->BLK_prop[mouseBlock->block_id]->hardness) {
                if (game->BLK_prop[mouseBlock->block_id]->hasDrop) {
                    game->player.resources[game->BLK_prop[mouseBlock->block_id]->drop] += 1;
                }
                mouseBlock->block_id = 0;

                // Delete tall grass
                if (blockAbove->block_id == 9) {
                    blockAbove->block_id = 0;
                }
                int idtoshow[1] = {0};
                updatePinnedRes(game);
                updateResPanel(game, game->ui->resPanel);
                game->player.mining = 0;
            }
            else {
                game->player.miningFrame = (int)( ( findTimeDiff(&(game->time), &(game->player.miningTimer)) / game->BLK_prop[mouseBlock->block_id]->hardness )*60.0 );
            } 
        }
        else {
            game->player.mining = 0;
        }
        game->player.prevMouseBlock = mouseBlock;


        //printf("done updating cursor\n");

        game->cloud.fl_x += 0.01f;
        game->cloud.x = (int)game->cloud.fl_x;



    }
    // ------------------------------------ GAME SIMULATION END ------------------------------------

    else {
        SDL_ShowCursor(SDL_ENABLE);
        
        updatePauseMenu(game, game->ui->pauseMenu);

        if (game->ui->pauseMenu->buttons[0].mouseOver && game->mouse.left) {
            game->paused = 0;
        }
        if (game->ui->pauseMenu->buttons[1].mouseOver && game->mouse.left) {
            game->running = 0;
        }

    }


    //printf("done updating\n");
}