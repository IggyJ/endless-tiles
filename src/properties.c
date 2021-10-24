#include "properties.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

/*
void printResources(ResourceProperties resources[], int n) {
    printf("\nPRINTING RESOURCES:\n\n");
    for (int i = 0; i < n; i++) {
        printf("  ID: %d\n", resources[i].id);
        printf("Name: %s\n", resources[i].name);
        printf("Icon: %s\n", resources[i].icon);
        printf("\n");
    }
    return;
}

void printBlocks(BlockProperties blocks[], int n) {
    printf("\nPRINTING BLOCKS:\n\n");
    for (int i = 0; i < n; i++) {
        printf("      ID: %d\n", blocks[i].id);
        printf("    Name: %s\n", blocks[i].name);
        printf(" Texture: %s\n", blocks[i].texture);
        printf(" Opacity: %d\n", blocks[i].opacity);
        if (blocks[i].spawnable) {
            printf("Min  =  %d x h  +  %d\n", blocks[i].minMult, blocks[i].minOffset);
            printf("Max  =  %d x h  +  %d\n", blocks[i].maxMult, blocks[i].maxOffset);
            printf("Noisemap: %d\n", blocks[i].useNoisemap);
            if (blocks[i].useNoisemap) {
                printf("   Spread: %d\n",  blocks[i].spread);
                printf("Frequency: %d\n",  blocks[i].frequency);
            }
        }
        if (blocks[i].hasDrop) {
            printf("    Drop: %d\n", blocks[i].drop);
        }
        printf("\n");
    }
    return;
}
*/

void parseHeightEquation(char str[], int * multiple, int * offset) {
    if (str[0] == 'i') {
        *multiple = 0;
        *offset = 0;
    }
    else {
        // Parse multiple
        char digits[64];
        char c;
        int i = 0;
        while (c != 'h') {
            c = str[i];
            if (c != 'h') {
                digits[i] = c;
            }
            else {
                digits[i] = '\0';
            }
            i++;
        }
        *multiple = atoi(digits);

        // Parse offset
        i += 1;
        int j = 0;
        while (c != '\0') {
            c = str[i];
            if (c != '\0') {
                digits[j] = c;
            }
            else {
                digits[j] = '\0';
            }
            i++; j++;
        }
        *offset = atoi(digits);
    }

    return;
}

void readString(FILE * file, char * c, char destination[]) {
    char str[64];
    int i;
    for (i = 0; *c != ';'; i++) {
        str[i] = *c;
        *c = getc(file);
    }
    str[i] = '\0';
    strcpy(destination, str);
    return;
}

void readInt(FILE * file, char * c, int * destination) {
    char digits[16];
    int i;
    for (i = 0; *c != ';'; i++) {
        digits[i] = *c;
        *c = getc(file);
    }
    digits[i] = '\0';
    *destination = atoi(digits);
    return;
}


void readProperties(GameState * game, ResourceProperties * resources[], BlockProperties * blocks[], PassiveMobProperties * pmobs[]) {
    printf("Loading properties...\n");
    FILE * file;
    file = fopen("res/properties.txt", "r");
    char c;

    game->RES_amount = 0;
    game->BLK_amount = 0;
    game->PMOB_amount = 0;

    int intTemp;

    if (file) {
        // Read resource count number
        while((c = getc(file)) != '='); c = getc(file); c = getc(file);
        readInt(file, &c, &(game->RES_amount));
        
        // Find block count number
        while((c = getc(file)) != '='); c = getc(file); c = getc(file);
        readInt(file, &c, &(game->BLK_amount));

        // Find passive mob count number
        while((c = getc(file)) != '='); c = getc(file); c = getc(file);
        readInt(file, &c, &(intTemp));
        game->PMOB_amount = intTemp;

        // START READING RESOURCES
        for (int i = 0; i < game->RES_amount; i++) {

            // Read resource ID
            while((c = getc(file)) != '='); c = getc(file); c = getc(file);
            int id;
            readInt(file, &c, &id);
            resources[id]->id = id;

            // Read resource name
            while((c = getc(file)) != '='); c = getc(file); c = getc(file); 
            readString(file, &c, resources[id]->name);

            // Read icon filename
            while((c = getc(file)) != '='); c = getc(file); c = getc(file);
            readString(file, &c, resources[id]->icon);

        }
        // END READING RESOURCES

        // START READING BLOCKS
        for (int i = 0; i < game->BLK_amount; i++) {
            
            // Read block ID
            while((c = getc(file)) != '='); c = getc(file); c = getc(file);
            int id;
            readInt(file, &c, &id);
            blocks[id]->id = id;

            // Read block name
            while((c = getc(file)) != '='); c = getc(file); c = getc(file);
            readString(file, &c, blocks[id]->name);

            // Read texture variants
            while((c = getc(file)) != '='); c = getc(file); c = getc(file);
            readInt(file, &c, &(blocks[id]->textureVariants));

            // Read texture filenames
            //printf("loading %d textures\n", blocks[id].textureVariants);
            for (int texture = 0; texture < blocks[id]->textureVariants; texture++) {
                while((c = getc(file)) != '='); c = getc(file); c = getc(file);
                blocks[id]->texture[texture] = malloc(sizeof(String_128));
                readString(file, &c, blocks[id]->texture[texture]->str);
            }

            // Read block opacity
            while((c = getc(file)) != '='); c = getc(file); c = getc(file);
            readInt(file, &c, &(blocks[id]->opacity));

            // Read block hardness
            while((c = getc(file)) != '='); c = getc(file); c = getc(file);
            int millisecs;
            readInt(file, &c, &millisecs);
            blocks[id]->hardness = ((double)millisecs)/1000.0;

            // Read spawnability
            while((c = getc(file)) != '='); c = getc(file); c = getc(file);
            readInt(file, &c, &(blocks[id]->spawnable));

            if (blocks[id]->spawnable) {
                char eqStr1[64];
                char eqStr2[64];

                // Read spawn min
                while((c = getc(file)) != '='); c = getc(file); c = getc(file);
                readString(file, &c, eqStr1);
                parseHeightEquation(eqStr1, &(blocks[id]->minMult), &(blocks[id]->minOffset));
            
                // Read spawn max
                while((c = getc(file)) != '='); c = getc(file); c = getc(file);
                readString(file, &c, eqStr2);
                parseHeightEquation(eqStr2, &(blocks[id]->maxMult), &(blocks[id]->maxOffset));

                // Read use noisemap
                while((c = getc(file)) != '='); c = getc(file); c = getc(file);
                readInt(file, &c, &(blocks[id]->useNoisemap));

                if (blocks[id]->useNoisemap) {
                    while((c = getc(file)) != '='); c = getc(file); c = getc(file);
                    readInt(file, &c, &(blocks[id]->spread));   
                    while((c = getc(file)) != '='); c = getc(file); c = getc(file);
                    readInt(file, &c, &(blocks[id]->frequency));   
                }

            }

            // Read drop
            char drop[64];
            while((c = getc(file)) != '='); c = getc(file); c = getc(file);
            readString(file, &c, drop);
            if (drop[0] == '?') {
                blocks[id]->hasDrop = 0;
            }
            else {
                blocks[id]->hasDrop = 1;
                blocks[id]->drop = atoi(drop);
            }

        }
        // END READING BLOCKS

        // START READING PASSIVE MOBS
        for (int i = 0; i < game->PMOB_amount; i++) {

            // Read mob ID
            while((c = getc(file)) != '='); c = getc(file); c = getc(file);
            int id;
            readInt(file, &c, &id);
            pmobs[id]->id = id;

            // Read resource name
            while((c = getc(file)) != '='); c = getc(file); c = getc(file); 
            readString(file, &c, pmobs[id]->name);

            // Read texture path
            while((c = getc(file)) != '='); c = getc(file); c = getc(file);
            pmobs[id]->texture = malloc(sizeof(String_128));
            readString(file, &c, pmobs[id]->texture->str);

            // Read width
            while((c = getc(file)) != '='); c = getc(file); c = getc(file);
            readInt(file, &c, &intTemp);
            pmobs[id]->width = intTemp;

            // Read height
            while((c = getc(file)) != '='); c = getc(file); c = getc(file);
            readInt(file, &c, &intTemp);
            pmobs[id]->height = intTemp;

            // Read max HP
            while((c = getc(file)) != '='); c = getc(file); c = getc(file);
            readInt(file, &c, &intTemp);
            pmobs[id]->hpMax = intTemp;


        }
        // END READING PASSIVE MOBS

        fclose(file);
        //printf("Done loading properties.\n");
        //printBlocks(blocks, game->BLK_amount);

    }
    else {
        printf("Error, could not find properties.txt\n");
    }

    return;
}