#include "renderer.h"

#include <SDL2/SDL.h>
#include "game.h"
#include "world.h"
#include "interface.h"

#include "defines.h"

void renderChunk(SDL_Renderer * renderer, GameState * game, Chunk * chunk) {
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            if (chunk->block[x][y].block_id != 0) {
                int spr_x = ((chunk->x + x*64) - game->player.x) + game->player.sprite_x;
                int spr_y = ((chunk->y + y*64) - game->player.y) + game->player.sprite_y;
                if (spr_x < 1920 && spr_x > -704 && spr_y < 1360 && spr_y > -704) {
                    SDL_Rect blk = {spr_x, spr_y, 64, 64};
                    SDL_RenderCopy(renderer, game->textures.block[chunk->block[x][y].variant][chunk->block[x][y].block_id], NULL, &blk);
                }
            }
        }
    }
    return;
}

void updateChunkMapColour(SDL_Renderer * renderer, GameState * game, ChunkNode * node) {
    if (node == game->loadedChunk) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    }
    else if (!(node->defined)) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    }
    return;
}

void renderChunkMapColumn(SDL_Renderer * renderer, GameState * game, ChunkNode * startNode, int xStart, int yStart) {
    int x = xStart;
    int y = yStart;
    ChunkNode * node = startNode;

    updateChunkMapColour(renderer, game, node);
    SDL_Rect centerRect = {x, y, 4, 4};
    SDL_RenderFillRect(renderer, &centerRect);

    node = startNode->next[N];
    while (node != NULL) {
        y -= 5;
        updateChunkMapColour(renderer, game, node);
        SDL_Rect rect = {x, y, 4, 4};
        SDL_RenderFillRect(renderer, &rect);

        node = node->next[N];
    }

    y = yStart;
    node = startNode->next[S];
    while (node != NULL) {
        y += 5;
        updateChunkMapColour(renderer, game, node);
        SDL_Rect rect = {x, y, 4, 4};
        SDL_RenderFillRect(renderer, &rect);

        node = node->next[S];
    }

    return;
}

void renderChunkMap(SDL_Renderer * renderer, GameState * game, int xStart, int yStart) {
    int x = xStart;
    int y = yStart;
    ChunkNode * node = game->centerChunk;

    renderChunkMapColumn(renderer, game, node, x, y);

    node = game->centerChunk->next[E];
    while (node != NULL) {
        x += 5;
        renderChunkMapColumn(renderer, game, node, x, y);
        node = node->next[E];
    }   
     
    x =  xStart;
    node = game->centerChunk->next[W];
    while (node != NULL) {
        x -= 5;
        renderChunkMapColumn(renderer, game, node, x, y);
        node = node->next[W];
    }     

    return;
}

void doRender(SDL_Renderer * renderer, GameState * game) {
    float brightness = 1.0f;
    if (game->player.y > 5192) {
        brightness = 1.0f - ((float)(game->player.y - 5192))/1000.0f;
        if (brightness < 0.15f) {
            brightness = 0.15f;
        }
    }
    int r = (int)(158.0f*brightness);
    int g = (int)(192.0f*brightness);
    int b = (int)(247.0f*brightness);
    // Fill screen
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderClear(renderer);
    SDL_Rect skybox = {0, 0, 1280, 720};
    //SDL_RenderCopy(renderer, game->textures.sky, NULL, &skybox);


    SDL_Rect cld1 = {game->cloud.x, game->cloud.y, 256, 256};
    SDL_RenderCopy(renderer, game->textures.cloud, NULL, &cld1);
    SDL_Rect cld2 = {game->cloud.x+600, game->cloud.y-100, 256, 256};
    SDL_RenderCopy(renderer, game->textures.cloud, NULL, &cld2);
    

    // Render chunks
    int clkwise[] = {2, 0, 3, 1};
    renderChunk(renderer, game, &(game->loadedChunk->data));
    for (int i = 0; i <= 3; i++) {
        renderChunk(renderer, game, &(game->loadedChunk->next[i]->data));
        renderChunk(renderer, game, &(game->loadedChunk->next[i]->next[clkwise[i]]->data));
    }


    // Render passive mobs
    for (int mob = 0; mob <= 64; mob++) {
        if (game->passiveMobs[mob].inGame && game->passiveMobs[mob].render) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            int spr_x = (game->passiveMobs[mob].x - game->player.x) + game->player.sprite_x;
            int spr_y = (game->passiveMobs[mob].y - game->player.y) + game->player.sprite_y;
            SDL_RendererFlip flip = SDL_FLIP_NONE;
            if (game->passiveMobs[mob].dir == L) {
                flip = SDL_FLIP_HORIZONTAL;
            } 
            SDL_Rect mobrect = {spr_x, spr_y, 128, 64};
            SDL_RenderCopyEx(renderer, game->textures.passiveMobs[0], NULL, &mobrect, 0.0, NULL, flip);

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_Rect mobhp1 = {spr_x+32, spr_y-20, 64, 8};
            SDL_RenderFillRect(renderer, &mobhp1);
            
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            int barWidth = (int)(64.0f*((float)game->passiveMobs[mob].hp/(float)game->PMOB_prop[mob]->hpMax));
            SDL_Rect mobhp2 = {spr_x+32, spr_y-20, barWidth, 8};
            SDL_RenderFillRect(renderer, &mobhp2);
            
        }
    }

    // Render player
    SDL_RendererFlip playerFlip = SDL_FLIP_NONE;
    if (game->player.facing == L) {
        playerFlip = SDL_FLIP_HORIZONTAL;
    } 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect plyr = {game->player.sprite_x-24, game->player.sprite_y+2, 87, 119};
    SDL_RenderCopyEx(renderer, game->textures.player[game->player.frame], NULL, &plyr, 0.0, NULL, playerFlip);
    //SDL_RenderFillRect(renderer, &plyr);

    // Render block cursor
    if ( !(game->paused) ) {
        SDL_Rect blkCrsr = {game->blockCursor.sprite_x, game->blockCursor.sprite_y, 64, 64};
        if (game->blockCursor.visible) {
            SDL_RenderCopy(renderer, game->textures.blockCursor, NULL, &blkCrsr);
        }
        if (game->player.mining) {
            SDL_RenderCopy(renderer, game->textures.miningCircle[game->player.miningFrame], NULL, &blkCrsr);
        }
        SDL_Rect crsrX = {game->mouse.x-8, game->mouse.y-8, 16, 16};
        if (game->player.canPlace) {
            SDL_RenderCopy(renderer, game->textures.cursorX[0], NULL, &crsrX);
        }
        else {
            SDL_RenderCopy(renderer, game->textures.cursorX[1], NULL, &crsrX);
        }
    }



    renderInterface(game);


    if (game->enableChunkMap) {
        renderChunkMap(renderer, game, 1000, 600);
    }

    // Update screen
    SDL_RenderPresent(renderer);
    return;
}
