#include "event_handler.h"

#include <stdio.h>
#include <SDL2/SDL.h>
#include "game.h"

#include "defines.h"
#include <stdbool.h>

void processEvents(SDL_Window * window, GameState * game) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_WINDOWEVENT_CLOSE: {
                if(window) {
                    game->running = 0;
                }
                break;
            }
            case SDL_KEYDOWN: {
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        game->paused = !(game->paused);
                        break;
                    case SDLK_TAB:
                        printf("\nPosition: %d, %d\n", game->player.x, game->player.y);
                        printf("   Chunk: %d, %d\n", game->player.chunk_x, game->player.chunk_y);
                        printf("  Height: %d\n", findHeight(game->player.x));
                        break;
                    case SDLK_p:
                        game->enablePhysics = !(game->enablePhysics);
                        break;
                    case SDLK_m:
                        game->enableChunkMap = !(game->enableChunkMap);
                        break;
                    case SDLK_SPACE:
                        printf("Center chunk at %p\n", game->centerChunk);
                        printf("N: %p\n", game->centerChunk->next[N]);
                        printf("S: %p\n", game->centerChunk->next[S]);
                        printf("W: %p\n", game->centerChunk->next[W]);
                        printf("E: %p\n", game->centerChunk->next[E]);
                        printf("Loaded chunk at %p\n", game->loadedChunk);
                        break;
                    
                }
                break;
            }
            case SDL_QUIT:
                game->running = 0;
                break;
        }
    }

    return;
}