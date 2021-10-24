#ifndef load_h_
#define load_h_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "defines.h"

struct Textures {
    SDL_Texture * block[4][1024];
    SDL_Texture * icon[1024];
    SDL_Texture * player[64];
    SDL_Texture * sky;
    SDL_Texture * cloud;
    SDL_Texture * blockCursor;
    SDL_Texture * miningCircle[60];
    SDL_Texture * cursorX[2];
    SDL_Texture * passiveMobs[BYTE_1];
};

struct Fonts {
    TTF_Font * arial_big;
    TTF_Font * arial_med;
    TTF_Font * arial_small;
};

typedef struct Textures Textures;
typedef struct Fonts Fonts;

typedef struct GameState GameState;

void loadTextures(GameState * game);
void destroyTextures(GameState * game);

void loadFonts(GameState * game);
void destroyFonts(GameState * game);


#endif