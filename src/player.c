#include "player.h"

#include <stdio.h>
#include "game.h"

#include "defines.h"


void updatePlayer(GameState * game) {

    /*--------------------------- PLAYER PHYSICS ---------------------------*/

    if (game->enablePhysics) {
        int x_w = snap(game->player.x - 1, 64);
        game->player.detectorBlock[W][N] = findBlock(game->loadedChunk, x_w, snap(game->player.y + 16, 64));
        game->player.detectorBlock[W][S] = findBlock(game->loadedChunk, x_w, snap(game->player.y + game->player.height - 16, 64));
        game->player.detectorBlock[W][W] = findBlock(game->loadedChunk, x_w, snap(game->player.y + game->player.height/2, 64));

        int x_e = snap(game->player.x + game->player.width + 1, 64);
        game->player.detectorBlock[E][N] = findBlock(game->loadedChunk, x_e, snap(game->player.y + 16, 64));
        game->player.detectorBlock[E][S] = findBlock(game->loadedChunk, x_e, snap(game->player.y + game->player.height - 16, 64));
        game->player.detectorBlock[E][E] = findBlock(game->loadedChunk, x_e, snap(game->player.y + game->player.height/2, 64));

        game->player.dy += game->gravity;

        if (game->player.dx > 0 || game->player.dxTarget > 0) {
            if (game->BLK_prop[game->player.detectorBlock[E][N]->block_id]->opacity ||
                game->BLK_prop[game->player.detectorBlock[E][S]->block_id]->opacity ||
                game->BLK_prop[game->player.detectorBlock[E][E]->block_id]->opacity
            ) {
                game->player.dx = 0;
                game->player.dxTarget = 0;
                game->player.x = x_e - game->player.width;
                game->player.animation = STILL;
                if (game->player.colliding[E] == 0) {
                    game->player.justCollided[E] = 1;
                    //printf("just collided right!\n");
                }
                else {
                    game->player.justCollided[E] = 0;
                }
                game->player.colliding[E] = 1;
            }
        }

        if (game->player.dx < 0 || game->player.dxTarget < 0) {
            if (game->BLK_prop[game->player.detectorBlock[W][N]->block_id]->opacity ||
                game->BLK_prop[game->player.detectorBlock[W][S]->block_id]->opacity ||
                game->BLK_prop[game->player.detectorBlock[W][W]->block_id]->opacity
            ) {
                game->player.dx = 0;
                game->player.dxTarget = 0;
                game->player.x = x_w + 64;
                game->player.animation = STILL;
                if (game->player.colliding[W] == 0) {
                    game->player.justCollided[W] = 1;
                    //printf("just collided left!\n");
                }
                else {
                    game->player.justCollided[W] = 0;
                }
                game->player.colliding[W] = 1;
            }
        }

        if (game->player.dxTarget > game->player.dx) {
            game->player.dx += 1;
        }
        if (game->player.dxTarget < game->player.dx) {
            game->player.dx -= 1;
        }
        

        int y_n = snap(game->player.y - 1, 64);
        game->player.detectorBlock[N][W] = findBlock(game->loadedChunk, snap(game->player.x + 2, 64), y_n);
        game->player.detectorBlock[N][E] = findBlock(game->loadedChunk, snap(game->player.x + game->player.width - 2, 64), y_n);

        int y_s = snap(game->player.y + game->player.height + game->player.dy/2, 64);
        game->player.detectorBlock[S][W] = findBlock(game->loadedChunk, snap(game->player.x + 4, 64), y_s);
        game->player.detectorBlock[S][E] = findBlock(game->loadedChunk, snap(game->player.x + game->player.width - 2, 64), y_s);

        if (game->player.dy > 0) {
            if (game->BLK_prop[game->player.detectorBlock[S][W]->block_id]->opacity || game->BLK_prop[game->player.detectorBlock[S][E]->block_id]->opacity) {
                game->player.dy = 0;
                game->player.y = y_s - game->player.height;
                if (game->player.colliding[S] == 0) {
                    game->player.justCollided[S] = 1;
                    //printf("just collided floor!\n");
                }
                else {
                    game->player.justCollided[S] = 0;
                }
                game->player.colliding[S] = 1;
            } 
        }
        if (game->player.dy < 0) {
            if (game->BLK_prop[game->player.detectorBlock[N][W]->block_id]->opacity || game->BLK_prop[game->player.detectorBlock[N][E]->block_id]->opacity) {
                game->player.dy = 0;
                game->player.y = y_n + 65;
                if (game->player.colliding[N] == 0) {
                    game->player.justCollided[N] = 1;
                    //printf("just collided ceiling!\n");
                }
                else {
                    game->player.justCollided[N] = 0;
                }
                game->player.colliding[N] = 1;
                game->player.colliding[S] = 0;
            } 
        }

        game->player.x += game->player.dx;
        game->player.y += game->player.dy;

    }

    /*--------------------------- PLAYER ANIMATION ---------------------------*/

    if (game->player.animation == STILL) {
        game->player.frame = 0;
    }
    else {
        if (findTimeDiff(&(game->time), &(game->player.frameTimer)) > 0.1) {
            game->player.frame++;
            if (game->player.frame > 6) {
                game->player.frame = 1;
            }
            copyTime(&(game->player.frameTimer), &(game->time));
        }
    }


    return;
}