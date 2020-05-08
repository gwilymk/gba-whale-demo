/**
 * @file background.h
 * @brief Handles everything you'd ever want to handle with backgrounds (except the tilemap)
 * 
 * @defgroup BACKGROUNDS Backgrounds
 * @{
 * 
 * The valid backgrounds are numbered 0 - 3 but which ones are available and which 'types' they
 * are depends on the current graphics mode.
 * 
 * mode | BG0 | BG1 | BG2 | BG3
 * -----|-----|-----|-----|----
 * 0    | reg | reg | reg | reg
 * 1    | reg | reg | aff | -
 * 2    | -   | -   | aff | aff
 * 
 * They must also be explicitly enabled as part of the current graphics mode.
 */

#pragma once

#include "GbaTypes.h"
#include "LostGbaUtil.h"

/** Set the priority of a given background (0 - 4) */
void Background_SetPriority(int backgroundNumber, int priority);
/** Set the background number (character base block). TileMap_CopyToBackgroundTiles sets the tiles themselves */
void Background_SetTileBackgroundNumber(int backgroundNumber, int tileBackgroundNumber);

/** The colour mode of the background */
enum BackgroundColourMode
{
    /** 4pp (16 colours). Can choose the sub-palette for each tile */
    BackgroundColourMode_4PP,
    /** 8pp (256 colours) */
    BackgroundColourMode_8PP
};

/** Set the current colour mode of the background */
void Background_SetColourMode(int backgroundNumber, enum BackgroundColourMode colourMode);
/** Set the current screenblock for the given background. This is what determines which tiles get placed where */
#define Background_SetScreenBlock(backgroundNumber, screenBlock)                                                 \
    do                                                                                                           \
    {                                                                                                            \
        _Static_assert(0 <= screenBlock && screenBlock <= 31, "Screenblock must be between 0 and 31 inclusive"); \
        LOSTGBA_UNSAFE(Background_SetScreenblock)                                                                \
        (backgroundNumber, screenBlock);                                                                         \
    } while (0)
void LOSTGBA_UNSAFE(Background_SetScreenblock)(int backgroundNumber, int screenblock);

/** @} */