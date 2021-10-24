#include "load.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "game.h"

#include "defines.h"

void loadTextures(GameState * game) {
    printf("Loading textures...\n");

    // Load block textures
    int freed = 0;
    SDL_Surface * blockSurface[4][game->BLK_amount];
    for (int blk = 0; blk < game->BLK_amount; blk++) {
        for (int texture = 0; texture < game->BLK_prop[blk]->textureVariants; texture++) {
            blockSurface[texture][blk] = IMG_Load(game->BLK_prop[blk]->texture[texture]->str);
            
            freed += sizeof(String_128);
            free(game->BLK_prop[blk]->texture[texture]);
        }
        for (int texture = 3; texture >= game->BLK_prop[blk]->textureVariants; texture--) {
            blockSurface[texture][blk] = NULL;
        }
    }

    for (int blk = 0; blk < game->BLK_amount; blk++) {
        for (int i = 0; i <= 3; i++) {
            if (blockSurface[i][blk] != NULL) {
                game->textures.block[i][blk] = SDL_CreateTextureFromSurface(game->renderer, blockSurface[i][blk]);
                SDL_FreeSurface(blockSurface[i][blk]); 
                freed += sizeof(SDL_Surface);
            }
        }
    }

    // Load icon textuers
    SDL_Surface * iconSurface[game->RES_amount];
    for (int ico = 0; ico < game->RES_amount; ico++) {
            if (game->RES_prop[ico]->icon[0] != '?') {
                iconSurface[ico] = IMG_Load(game->RES_prop[ico]->icon);
            }
            else {
                iconSurface[ico] = NULL;
            }
    }

    for (int ico = 0; ico < game->RES_amount; ico++) {
        if (iconSurface[ico] != NULL) {
            game->textures.icon[ico] = SDL_CreateTextureFromSurface(game->renderer, iconSurface[ico]);
            SDL_FreeSurface(iconSurface[ico]); 
            freed += sizeof(SDL_Surface);
        }
        else {
            printf("Error, could not find icon %d.\n", ico);
        }
    }

    // Load passiv mob textuers
    SDL_Surface * pmobSurface[game->PMOB_amount];
    for (int mob = 0; mob < game->PMOB_amount; mob++) {
        if (game->PMOB_prop[mob]->texture->str[0] != '?') {
            pmobSurface[mob] = IMG_Load(game->PMOB_prop[mob]->texture->str);
        }
        else {
            pmobSurface[mob] = NULL;
        }
        free(game->PMOB_prop[mob]->texture);
        freed += sizeof(String_128);
    }

    for (int mob = 0; mob < game->PMOB_amount; mob++) {
        if (pmobSurface[mob] != NULL) {
            game->textures.passiveMobs[mob] = SDL_CreateTextureFromSurface(game->renderer, pmobSurface[mob]);
            SDL_FreeSurface(pmobSurface[mob]); 
            freed += sizeof(SDL_Surface);
        }
        else {
            printf("Error, could not find mob %d.\n", mob);
        }
    }
    

    // Load player textures
    for (int frame = 0; frame <= 6; frame++) {
        SDL_Surface * playerSurface;
        char path[64];
        sprintf(path, "res/player/%04d.png", frame);
        playerSurface = IMG_Load(path);
        game->textures.player[frame] = SDL_CreateTextureFromSurface(game->renderer, playerSurface);
        SDL_FreeSurface(playerSurface);
        freed += sizeof(SDL_Surface);
    }


    // Load sky texture
    SDL_Surface * skySurface;
    skySurface = IMG_Load("res/sky.png");
    game->textures.sky = SDL_CreateTextureFromSurface(game->renderer, skySurface);
    SDL_FreeSurface(skySurface); 
    freed += sizeof(SDL_Surface);

    // Load cloud texture
    SDL_Surface * cloudSurface;
    cloudSurface = IMG_Load("res/cloud.png");
    game->textures.cloud = SDL_CreateTextureFromSurface(game->renderer, cloudSurface);
    SDL_FreeSurface(cloudSurface); 
    freed += sizeof(SDL_Surface);

    // Load block cursor
    SDL_Surface * blockCursorSurface;
    blockCursorSurface = IMG_Load("res/cursor/blockcursor.png");
    game->textures.blockCursor = SDL_CreateTextureFromSurface(game->renderer, blockCursorSurface);
    SDL_FreeSurface(blockCursorSurface); 
    freed += sizeof(SDL_Surface);

    // Load cursor X
    SDL_Surface * cursorXSurface;
    cursorXSurface = IMG_Load("res/cursor/cursor_x.png");
    game->textures.cursorX[0] = SDL_CreateTextureFromSurface(game->renderer, cursorXSurface);
    SDL_FreeSurface(cursorXSurface); 
    SDL_Surface * cursorXredSurface;
    cursorXredSurface = IMG_Load("res/cursor/cursor_x_red.png");
    game->textures.cursorX[1] = SDL_CreateTextureFromSurface(game->renderer, cursorXredSurface);
    SDL_FreeSurface(cursorXredSurface); 
    freed += sizeof(SDL_Surface)*2;


    // Load cursor mining cirle 
    for (int frame = 0; frame < 60; frame++) {
        SDL_Surface * miningCircleSurface;
        char path[64];
        sprintf(path, "res/cursor/mining%d.png", frame+1);
        miningCircleSurface = IMG_Load(path);
        game->textures.miningCircle[frame] = SDL_CreateTextureFromSurface(game->renderer, miningCircleSurface);
        SDL_FreeSurface(miningCircleSurface); 
        freed += sizeof(SDL_Surface);
    }


    //printf("Done loading textures.\n");
    printf("Freed %d kB.\n", freed/1024);
    return;
}


void loadFonts(GameState * game) {
    printf("Loading fonts...\n");

    game->fonts.arial_big = TTF_OpenFont("res/arial.ttf", 30);
    if (game->fonts.arial_big == NULL) {
        printf("Error loading arial font.\n");
    }

    game->fonts.arial_med = TTF_OpenFont("res/arial.ttf", 20);
    if (game->fonts.arial_med == NULL) {
        printf("Error loading arial font.\n");
    }

    game->fonts.arial_small = TTF_OpenFont("res/arial.ttf", 14);
    if (game->fonts.arial_small == NULL) {
        printf("Error loading arial font.\n");
    }

    //printf("Done loading fonts.\n");
    return;
}


// -----------------------------------------------------------------------------------------------------------------------------------------------------------------


void destroyFonts(GameState * game) {

    TTF_CloseFont(game->fonts.arial_big);
    TTF_CloseFont(game->fonts.arial_med);
    TTF_CloseFont(game->fonts.arial_small);

    return;
}


void destroyTextures(GameState * game) {
    printf("Destroying textures...\n");

    for (int blk = 0; blk < game->BLK_amount; blk++) {
        SDL_DestroyTexture(game->textures.block[0][blk]);
    }
    for (int ico = 0; ico < game->RES_amount; ico++) {
        SDL_DestroyTexture(game->textures.icon[ico]);
    }

    //SDL_DestroyTexture(game->textures.player[L]);
    //SDL_DestroyTexture(game->textures.player[R]);
    SDL_DestroyTexture(game->textures.sky);
    SDL_DestroyTexture(game->textures.blockCursor);

    return;
}