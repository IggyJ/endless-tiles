#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "event_handler.h"
#include "renderer.h"
#include "game.h"
#include "init_game.h"

#include "mobs.h"
#include <stdint.h>


int main(int argc, char *argv[]) {
    //printf("%d B\n", sizeof(Label));
    printf("GameState size: %d kB\n", sizeof(GameState)/1024);
    //return 0;

    GameState game;
    initGameState(&game);
  
    
    while(game.running) {
        processEvents(game.window, &game);
        update(&game, game.renderer);
        doRender(game.renderer, &game);
    }
    
    destroyTextures(&game);
    destroyFonts(&game);
    SDL_DestroyWindow(game.window);
    SDL_DestroyRenderer(game.renderer);
    TTF_Quit();
    SDL_Quit();
    return 0;
}