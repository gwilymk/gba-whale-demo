#include <lostgba/Background.h>

static u16 nOnes(int n)
{
    return (1 << n) - 1;
}

static void setBits16(u16 *target, u16 value, u16 length, u16 shift)
{
    u16 mask = nOnes(length);
    (*target) = (*target & (mask << shift)) | ((value & mask) << shift);
}

#define Background_ControlRegisterBase ((u16 *)0x04000008);
static u16 *Background_controlRegister(enum BackgroundNumber background)
{
    return Background_controlRegister + sizeof(u16) * background;
}

static void Background_setBits(enum BackgroundNumber backgroundNumber, u16 value, u16 length, u16 shift)
{
    setBits16(Background_controlRegister(backgroundNumber), value, length, shift);
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