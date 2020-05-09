#include <lostgba/Background.h>
#include "LostGbaInternal.h"

u16 *Background_ControlRegisterBaseAddr = (vu16 *)0x04000008;

static void Background_setBits(enum BackgroundNumber backgroundNumber, u16 value, u16 length, u16 shift)
{
    LostGBA_SetBits16(&Background_ControlRegisterBaseAddr[backgroundNumber], value, length, shift);
}

void Background_SetPriority(enum BackgroundNumber backgroundNumber, int priority)
{
    Background_setBits(backgroundNumber, priority, 2, 0);
}

void Background_SetTileBackgroundNumber(enum BackgroundNumber backgroundNumber, int tileBackgroundNumber)
{
    Background_setBits(backgroundNumber, tileBackgroundNumber, 2, 2);
}

void Background_SetColourMode(enum BackgroundNumber backgroundNumber, enum BackgroundColourMode colourMode)
{
    Background_setBits(backgroundNumber, colourMode, 1, 7);
}

void LOSTGBA_UNSAFE(Background_SetScreenBaseBlock)(enum BackgroundNumber backgroundNumber, int screenblock)
{
    Background_setBits(backgroundNumber, screenblock, 5, 8);
}

void Background_SetSize(enum BackgroundNumber backgroundNumber, enum BackgroundSize backgroundSize)
{
    Background_setBits(backgroundNumber, backgroundSize, 2, 14);
}