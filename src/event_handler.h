#ifndef event_handler_h_
#define event_handler_h_

#include <SDL2/SDL.h>

typedef struct GameState GameState;

void processEvents(SDL_Window * window, GameState * game);

#endif