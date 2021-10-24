#include "init_game.h"

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "game.h"
#include "mobs.h"
#include "player.h"


void initGameState(GameState * game) {
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();


    game->window = SDL_CreateWindow("Endless Tiles",      // Window title
                               SDL_WINDOWPOS_UNDEFINED,   // initial x position
                               SDL_WINDOWPOS_UNDEFINED,   // initial y position
                               1280,                      // width 
                               720,                       // height
                               0                          // flags
                               );
    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);


    for (int i = 0; i < BYTE_1; i++) {
        game->PMOB_prop[i] = malloc(sizeof(PassiveMobProperties));
    }
    for (int i = 0; i < BYTE_2; i++) {
        game->BLK_prop[i] = malloc(sizeof(BlockProperties));
        game->RES_prop[i] = malloc(sizeof(ResourceProperties));
    }
    readProperties(game, game->RES_prop, game->BLK_prop, game->PMOB_prop);
    loadTextures(game);
    loadFonts(game);

    initChunkMatrix(game);

    game->running = 1;
    game->enablePhysics = 1;
    game->enableChunkMap = 0;

    game->cloud.fl_x = 200.0f;
    game->cloud.x = 200;
    game->cloud.y = 200;

    game->player.x = 32;
    game->player.y = 32;
    game->player.dx = 0;
    game->player.dy = 0;
    game->player.width = 40;
    game->player.height = 120;
    game->player.sprite_x = 640 - game->player.width/2;
    game->player.sprite_y = 360 - game->player.height/2;
    game->player.chunk_x = 0;
    game->player.chunk_y = 0;
    game->player.dxTarget = 0;
    game->player.facing = 1;
    game->player.animation = 0;
    game->player.frame = 0;
    game->player.mode = CONSTRUCTION;

    for (int i = 0; i < 1024; i++) {
        game->player.resources[i] = 0;
    }
    clearTime(&(game->player.frameTimer));
    clearTime(&(game->player.miningTimer));
    game->player.mining = 0;
    game->player.miningFrame = 0;
    for (int i = 0; i <= 3; i++) {
        game->player.colliding[i] = 0;
    }

    game->gravity = 1;

    game->ui = malloc(sizeof(Interface));
    initInterface(game, game->ui);

    /*
    strcpy(game->ui->pauseMenu.buttons[0].text, "Return to Game");
    strcpy(game->ui->pauseMenu.buttons[1].text, "Quit to Desktop");
    initInterfaceBox(game, game->renderer, &(game->ui->pauseMenu), 490, 160, 300, 200, "Menu", 2, 0);

    initInterfaceBox(game, game->renderer, &(game->ui->resourcePanel), 1070, 10, 200, 200, "Resources", 0, game->RES_amount);
    */

    game->paused = 0;

    updateChunks(game, 3);

    game->controller.buttonA = 0;
    for (int btn = 0; btn <= 3; btn++) {
        game->controller.dpad[btn] = 0;
    }
    if (SDL_NumJoysticks() > 0) {
        if (SDL_IsGameController(0)) {
            game->controller.data = SDL_GameControllerOpen(0);
            game->controllerEnabled = 1;
        }
    }
    else {
        game->controllerEnabled = 0;
    }


    for (int i = 0; i < 64; i++) {
        game->passiveMobs[i].inGame = 0;
    }

    spawnPassiveMob(game, &(game->passiveMobs[0]), 0, 100, 512);
    return;
}