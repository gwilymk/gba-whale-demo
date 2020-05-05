#pragma once

#include "GbaTypes.h"
#include "LostGbaUtil.h"

#define TileMap_PaletteLength 256

void TileMap_CopyToSpritePalette(const u16 paletteData[TileMap_PaletteLength]);
#define TileMap_CopyToSpriteTiles(tileNumber, tileData, length)                                          \
    do                                                                                                   \
    {                                                                                                    \
        _Static_assert(0 <= tileNumber && tileNumber <= 1, "Sprites can only use tile numbers 0 and 1"); \
        LOSTGBA_INTERNAL(TileMap_CopyToSpriteTiles)                                                      \
        (tileNumber, tileData, length);                                                                  \
    } while (0)
void LOSTGBA_INTERNAL(TileMap_CopyToSpriteTiles)(int tileNumber, const unsigned int *tileData, int length);

void TileMap_CopyToBackgroundPalette(const u16 paletteData[TileMap_PaletteLength]);
#define TileMap_CopyToBackgroundTiles(backgroundNumber, tileData, length)                                                      \
    do                                                                                                                         \
    {                                                                                                                          \
        _Static_assert(0 <= backgroundNumber && backgroundNumber <= 3, "Background number must be between 0 and 3 inclusive"); \
        LOSTGBA_INTERNAL(TileMap_CopyToBackgroundTiles)                                                                        \
        (backgroundNumber, tileData, length);                                                                                  \
    } while (0)
void LOSTGBA_INTERNAL(TileMap_CopyToBackgroundTiles)(int backgroundNumber, const unsigned int *tileData, int length);