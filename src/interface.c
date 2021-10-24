#include "interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "game.h"


/* -------------------------- INITIALISATION -------------------------- */

void initResPanel(GameState * game, ResourcePanel * panel) {
    panel->x = 1070;
    panel->y = 10;
    panel->width = 200;
    panel->height = 200;
    panel->enabled = false;
    panel->labelsAmount = 0;
    for (int i = 0; i < 16; i++) {
        panel->resLabels[i].texture = NULL;
        panel->resIcons[i] = NULL;
    }

    // Create panel title
    SDL_Color colour = {255, 255, 255, 255};
    SDL_Surface * tmp = TTF_RenderText_Blended(game->fonts.arial_med, "Resources", colour);
    panel->title.texture = SDL_CreateTextureFromSurface(game->renderer, tmp);
    SDL_FreeSurface(tmp);
    int w, h;
    SDL_QueryTexture(panel->title.texture, NULL, NULL, &w, &h);
    panel->title.width = w;
    panel->title.height = h;

    return;
}

void initPauseMenu(GameState * game, PauseMenu * menu) {
    menu->x = 490;
    menu->y = 160;
    menu->width = 300;
    menu->height = 200;
    menu->enabled = false;

    // Create panel title
    SDL_Color colour = {255, 255, 255, 255};
    SDL_Surface * tmp = TTF_RenderText_Blended(game->fonts.arial_med, "Menu", colour);
    menu->title.texture = SDL_CreateTextureFromSurface(game->renderer, tmp);
    SDL_FreeSurface(tmp);
    int w, h;
    SDL_QueryTexture(menu->title.texture, NULL, NULL, &w, &h);
    menu->title.width = w;
    menu->title.height = h;

    // Init buttons
    char buttonLabels[2][64] = {
        "Return to Game",
        "Quit to Desktop"
    };
    for (int i = 0; i < 2; i++) {
        // Create texture label for button
        SDL_Color colour = {255, 255, 255, 255};
        SDL_Surface * tmp2 = TTF_RenderText_Blended(game->fonts.arial_med, buttonLabels[i], colour);
        menu->buttons[i].label.texture = SDL_CreateTextureFromSurface(game->renderer, tmp2);
        SDL_FreeSurface(tmp2);
        int w, h;
        SDL_QueryTexture(menu->buttons[i].label.texture, NULL, NULL, &w, &h);
        menu->buttons[i].label.width = w;
        menu->buttons[i].label.height = h;

        menu->buttons[i].width = 200;
        menu->buttons[i].height = 50;
        menu->buttons[i].x = menu->x + (menu->width - menu->buttons[i].width)/2,
        menu->buttons[i].y = menu->y + 50 + 60*i;
        menu->buttons[i].mouseOver = false;
    }

    return;
}


void initInterface(GameState * game, Interface * ui) {
    ui->resPanel = malloc(sizeof(ResourcePanel));
    ui->pauseMenu = malloc(sizeof(PauseMenu));

    ui->default_fg = (RGBA){150, 150, 150, 150};
    ui->default_bg = (RGBA){255, 255, 255, 100};

    initResPanel(game, ui->resPanel);
    initPauseMenu(game, ui->pauseMenu);
    return;
}


/* -------------------------- UPDATE -------------------------- */

void updatePinnedRes(GameState * game) {
    int n = 0;
    for (int i = 0; i < game->RES_amount && n < 16; i++) {
        if (game->player.resources[i] > 0) {
            game->ui->resPanel->resID[n] = i;
            n++;
        }
    }
    game->ui->resPanel->labelsAmount = n;

    return;
}

void updateResPanel(GameState * game, ResourcePanel * panel) { 
    if (panel->labelsAmount > 0) {
        panel->enabled = true;
    }
    else {
        panel->enabled = false;
    }
    for (int i = 0; i < panel->labelsAmount; i++) {
        if (panel->resIcons[i] != NULL) {
            SDL_DestroyTexture(panel->resIcons[i]);
        }
        if (panel->resLabels[i].texture != NULL) {
            SDL_DestroyTexture(panel->resLabels[i].texture);
        }

        // Update text label
        char str[64];
        sprintf(str, "%s: %d", game->RES_prop[panel->resID[i]]->name, game->player.resources[panel->resID[i]]);
        SDL_Color colour = {255, 255, 255, 255};
        SDL_Surface * tmp = TTF_RenderText_Blended(game->fonts.arial_med, str, colour);
        panel->resLabels[i].texture = SDL_CreateTextureFromSurface(game->renderer, tmp);
        SDL_FreeSurface(tmp);
        int w, h;
        SDL_QueryTexture(panel->resLabels[i].texture, NULL, NULL, &w, &h);
        panel->resLabels[i].width = w;
        panel->resLabels[i].height = h;
    }
    return;
}

void updateButton(GameState * game, Button * button) {
    if (game->mouse.x > button->x && game->mouse.x < button->x + button->width && game->mouse.y > button->y && game->mouse.y < button->y + button->height) {
        button->mouseOver = true;
    }
    else {
        button->mouseOver = false;
    }
    return;
}

void updatePauseMenu(GameState * game, PauseMenu * menu) {
    for (int i = 0; i < 2; i++) {
        updateButton(game, &(menu->buttons[i]));
    }
    return;
}


/* -------------------------- RENDER -------------------------- */

void drawBox(SDL_Renderer * renderer, int x, int y, int width, int height, RGBA fg, RGBA bg) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_Rect rect1 = {x, y, width, height};
    SDL_RenderFillRect(renderer, &rect1);

    SDL_SetRenderDrawColor(renderer, fg.r, fg.g, fg.b, fg.a);
    SDL_Rect rect2 = {x + 5, y + 5, width - 10, height - 10};
    SDL_RenderFillRect(renderer, &rect2);

    return;
}

void drawButton(GameState * game, Button * button) {
    if (button->mouseOver) {
        RGBA fg = {10, 240, 67, 150};
        RGBA bg = {6, 158, 44, 150};
        drawBox(game->renderer, button->x, button->y, button->width, button->height, fg, bg);
    }
    else {
        drawBox(game->renderer, button->x, button->y, button->width, button->height, game->ui->default_fg, game->ui->default_bg);
    }

    SDL_Rect labelRect = {
        button->x + (button->width - button->label.width)/2,
        button->y + (button->height - button->label.height)/2,
        button->label.width,
        button->label.height
    };
    SDL_RenderCopy(game->renderer, button->label.texture, NULL, &labelRect);

    return;
}

void drawResPanel(GameState * game, ResourcePanel * panel) {
    if (panel->enabled) {
        drawBox(game->renderer, panel->x, panel->y, panel->width, 60 + panel->labelsAmount*38, game->ui->default_fg, game->ui->default_bg);
        
        // Draw title
        SDL_Rect titleRect = {panel->x + (panel->width - panel->title.width)/2, panel->y + 15, panel->title.width, panel->title.height};
        SDL_RenderCopy(game->renderer, panel->title.texture, NULL, &titleRect);

        for (int i = 0; i < panel->labelsAmount; i++) {
            // Draw text
            SDL_Rect rect1 = {panel->x + 60, panel->y + 44 + i*38, panel->resLabels[i].width, panel->resLabels[i].height};
            SDL_RenderCopy(game->renderer, panel->resLabels[i].texture, NULL, &rect1);

            // Drawn icon
            SDL_Rect rect2 = {panel->x + 20, panel->y + 40 + i*38, 32, 32};
            SDL_RenderCopy(game->renderer, game->textures.icon[panel->resID[i]], NULL, &rect2);
        }
    }

    return;
}

void drawPauseMenu(GameState * game, PauseMenu * menu) {
    menu->enabled = game->paused;
    if (menu->enabled) {
        drawBox(game->renderer, menu->x, menu->y, menu->width, menu->height, game->ui->default_fg, game->ui->default_bg);
        
        // Draw title
        SDL_Rect titleRect = {menu->x + (menu->width - menu->title.width)/2, menu->y + 15, menu->title.width, menu->title.height};
        SDL_RenderCopy(game->renderer, menu->title.texture, NULL, &titleRect);

        for (int i = 0; i < 2; i++) {
            drawButton(game, &(menu->buttons[i]));
        }
    }

    return;
}

void renderInterface(GameState * game) {
    drawResPanel(game, game->ui->resPanel);
    drawPauseMenu(game, game->ui->pauseMenu);
}





/*

void initResLabel(GameState * game, SDL_Renderer * renderer, ResLabel * data, int x, int y, int width, int height) {
    data->x = x;
    data->y = y;
    data->width = width;
    data->height = height;
    strcpy(data->textRes, game->RES_prop[data->res_id]->name);
    sprintf(data->text, "%s: %d", data->textRes, game->player.resources[data->res_id]);
    SDL_Color colour = {255, 255, 255, 255};
    SDL_Surface * tmp = TTF_RenderText_Blended(game->fonts.arial_med, data->text, colour);
    data->labelTexture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);
    SDL_QueryTexture(data->labelTexture, NULL, NULL, &(data->texture_w), &(data->texture_h));

    return;
}

void updateResLabel(GameState * game, SDL_Renderer * renderer, ResLabel * data) {
    SDL_DestroyTexture(data->labelTexture);
    sprintf(data->text, "%s: %d", data->textRes, game->player.resources[data->res_id]);
    SDL_Color colour = {255, 255, 255, 255};
    SDL_Surface * tmp = TTF_RenderText_Blended(game->fonts.arial_med, data->text, colour);
    data->labelTexture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);
    SDL_QueryTexture(data->labelTexture, NULL, NULL, &(data->texture_w), &(data->texture_h));

    return;
}

void initButton(GameState * game, SDL_Renderer * renderer, Button * data, int x, int y, int width, int height) {
    data->x = x;
    data->y = y;
    data->width = width;
    data->height = height;
    SDL_Color colour = {255, 255, 255, 255};
    SDL_Surface * tmp = TTF_RenderText_Blended(game->fonts.arial_med, data->text, colour);
    data->labelTexture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);
    SDL_QueryTexture(data->labelTexture, NULL, NULL, &(data->texture_w), &(data->texture_h));
    data->mouseOver = 0;

    return;
}

void initInterfaceBox(GameState * game, SDL_Renderer * renderer, InterfaceBox * data, int x, int y, int width, int height, char label[], int buttonsAmount, int resLabelsAmount) {
    data->x = x;
    data->y = y;
    data->width = width;
    data->height = height;
    strcpy(data->text, label);
    SDL_Color colour = {255, 255, 255, 255};
    SDL_Surface * tmp = TTF_RenderText_Blended(game->fonts.arial_med, data->text, colour);
    data->labelTexture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);
    SDL_QueryTexture(data->labelTexture, NULL, NULL, &(data->texture_w), &(data->texture_h));

    data->buttonsAmount = buttonsAmount;
    for (int i = 0; i < buttonsAmount; i++) {
        initButton(game, renderer, &(data->buttons[i]), (x + (width - 200)/2), (y + 50 + i*60), 200, 50);
    }

    data->resLabelsAmount = resLabelsAmount;
    for (int i = 0; i < resLabelsAmount; i++) {
        data->resLabels[i].res_id = i;
        initResLabel(game, renderer, &(data->resLabels[i]), (x + (width - 200)/2), (y + 35 + i*30), 200, 50);
    }
    return;
}

void drawButton(SDL_Renderer * renderer, Button * data) {
    //SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if (data->mouseOver) {
        SDL_SetRenderDrawColor(renderer, 6, 158, 44, 150);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 150);
    }

    SDL_Rect rect1 = {data->x, data->y, data->width, data->height};
    SDL_RenderFillRect(renderer, &rect1);

    if (data->mouseOver) {
        SDL_SetRenderDrawColor(renderer, 10, 240, 67, 150);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 150);
    }

    SDL_Rect rect2 = {data->x + 5, data->y + 5, data->width - 10, data->height - 10};
    SDL_RenderFillRect(renderer, &rect2);

    SDL_Rect rect3 = {data->x + (data->width - data->texture_w)/2, data->y + (data->height - data->texture_h)/2, data->texture_w, data->texture_h};
    SDL_RenderCopy(renderer, data->labelTexture, NULL, &rect3);
    return;
}

void drawResLabel(SDL_Renderer * renderer, ResLabel * data, GameState * game) {
    SDL_Rect ico = {data->x + 20, data->y, 32, 32};
    SDL_RenderCopy(renderer, game->textures.icon[data->res_id], NULL, &ico);

    SDL_Rect text = {data->x + 58, data->y + 5, data->texture_w, data->texture_h};
    SDL_RenderCopy(renderer, data->labelTexture, NULL, &text);

    return;
}

void drawInterfaceBox(SDL_Renderer * renderer, InterfaceBox * data, GameState * game) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
    SDL_Rect rect1 = {data->x, data->y, data->width, data->height};
    SDL_RenderFillRect(renderer, &rect1);

    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 150);
    SDL_Rect rect2 = {data->x + 5, data->y + 5, data->width - 10, data->height - 10};
    SDL_RenderFillRect(renderer, &rect2);

    SDL_Rect rect3 = {data->x + (data->width - data->texture_w)/2, data->y + 15, data->texture_w, data->texture_h};
    SDL_RenderCopy(renderer, data->labelTexture, NULL, &rect3);

    for (int i = 0; i < data->buttonsAmount; i++) {
        drawButton(renderer, &(data->buttons[i]));
    }

    for (int i = 0; i < data->resLabelsAmount; i++) {
        drawResLabel(renderer, &(data->resLabels[i]), game);
    }
    return;
}

void updateButton(Button * data, int mouse_x, int mouse_y) {
    if (mouse_x > data->x && mouse_x < data->x + data->width && mouse_y > data->y && mouse_y < data->y + data->height) {
        data->mouseOver = 1;
    }
    else {
        data->mouseOver = 0;
    }
    return;
}

void updateInterfaceBox(InterfaceBox * data, GameState * game, SDL_Renderer * renderer) {
    
    for (int i = 0; i < data->buttonsAmount; i++) {
        updateButton(&(data->buttons[i]), game->mouse.x, game->mouse.y);
    }

    for (int i = 0; i < data->resLabelsAmount; i++) {
        updateResLabel(game, renderer, &(data->resLabels[i]));
    }
    return;
}
*/