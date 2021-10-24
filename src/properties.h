#ifndef properties_h_
#define properties_h_

#include <stdint.h>


typedef struct String_128 {
    char str[128];
} String_128;

typedef struct PassiveMobProperties {
    uint8_t id;
    int hpMax;
    uint16_t width, height;
    String_128 * texture;
    char name[64];
} PassiveMobProperties;

typedef struct ResourceProperties {
    int id;
    char name[64];
    char icon[64];
} ResourceProperties;

typedef struct BlockProperties {
    int id;
    char name[64];
    String_128 * texture[4];
    int textureVariants;
    int opacity;
    double hardness;
    int hasDrop;
    int drop;
    // Spawning properties
    int spawnable;
    int minMult;
    int minOffset;
    int maxMult;
    int maxOffset;
    int useNoisemap;
    int spread;
    int frequency;
} BlockProperties;

typedef struct GameState GameState;

void readProperties(GameState * game, ResourceProperties * resources[], BlockProperties * blocks[], PassiveMobProperties * pmobs[]);

#endif