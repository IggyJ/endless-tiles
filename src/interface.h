#ifndef interface_h_
#define interface_h_

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct RGBA {
    Uint8 r, g, b, a;
} RGBA;

typedef struct Label {
    SDL_Texture * texture;
    short int width, height;
} Label;

typedef struct Button {
    short int x, y, width, height;
    Label label;
    bool mouseOver;
} Button;


/*
typedef struct ResLabel {
    int res_id;
    int x, y;
    int width, height;
    char textRes[128];
    char text[128];
    SDL_Texture * labelTexture;
    int texture_w, texture_h;
} ResLabel;
*/


/*
typedef struct InterfaceBox {
    int x, y;
    int width, height;
    char text[128];
    SDL_Texture * labelTexture;
    int texture_w, texture_h;
    int enabled;
    int buttonsAmount;
    struct Button buttons[16];
    int resLabelsAmount;
    struct ResLabel resLabels[16];
} InterfaceBox;
*/


typedef struct ResourcePanel {
    short int x, y, width, height;
    Label title;

    uint8_t labelsAmount;
    short int resID[16];
    Label resLabels[16];
    SDL_Texture * resIcons[16];

    bool enabled;
} ResourcePanel;


typedef struct PauseMenu {
    short int x, y, width, height;
    Label title;

    Button buttons[2];

    bool enabled;
} PauseMenu;


typedef struct Interface {
    RGBA default_fg;
    RGBA default_bg;
    ResourcePanel * resPanel;
    PauseMenu * pauseMenu;
    //struct InterfaceBox resourcePanel;
    //struct InterfaceBox pauseMenu;
} Interface;



typedef struct GameState GameState;

void initInterface(GameState * game, Interface * ui);

void updatePinnedRes(GameState * game);
void updateResPanel(GameState * game, ResourcePanel * panel);

void updatePauseMenu(GameState * game, PauseMenu * menu);

void renderInterface(GameState * game);

//void initInterfaceBox(GameState * game, SDL_Renderer * renderer, InterfaceBox * data, int x, int y, int width, int height, char label[], int buttonsAmount, int resLabelAmount);
//void updateInterfaceBox(InterfaceBox * data, GameState * game, SDL_Renderer * renderer);
//void drawInterfaceBox(SDL_Renderer * renderer, InterfaceBox * data, GameState * game);


#endif