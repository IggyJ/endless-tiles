#ifndef renderer_h_
#define renderer_h_

#include <SDL2/SDL.h>

typedef struct GameState GameState;

void doRender(SDL_Renderer * renderer, GameState * game);

#endif 