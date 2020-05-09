#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>

#include <lostgba/ObjectAttribute.h>
#include <lostgba/TileMap.h>
#include <lostgba/Background.h>

#include <string.h>

#include <whale.h>
#include <tilemap.h>

void setupSprites(void)
{
    TileMap_CopyToSpritePalette(whalePal);
    TileMap_CopyToSpriteTiles(0, whaleTiles, whaleTilesLen);

    for (int i = 0; i < 128; i++)
    {
        ObjectAttribute_SetDisplayMode(&objectAttributeBuffer[i], ObjectAttributeDisplayMode_Hidden);
    }
}

void setupTilemap(void)
{
    TileMap_CopyToBackgroundPalette(tilemapPal);
    TileMap_CopyToBackgroundTiles(0, tilemapTiles, tilemapTilesLen);
}

u32 randomNumber(void)
{
    static u32 state = 12023908;

    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;

    return state;
}

void updateTilemapEntries(void)
{
    for (int i = 0; i < 32 * 32; i++)
    {
        int r = randomNumber();

        s16 tileToUse = !(r & 0x4);

        s16 screenEntry = tileToUse;
        ((s16 *)MAP_BASE_ADR(30))[i] = screenEntry;
    }
}

int max(int a, int b)
{
    return a > b ? a : b;
}

int min(int a, int b)
{
    return a < b ? a : b;
}

int main(void)
{
    irqInit();
    irqEnable(IRQ_VBLANK);

    REG_IME = 1; // allow interrupts

    SetMode(MODE_0 | BG0_ENABLE | OBJ_ENABLE | OBJ_1D_MAP);
    Background_SetColourMode(BackgroundNumber_0, BackgroundColourMode_4PP);
    Background_SetSize(BackgroundNumber_0, BackgroundSize_32x32);
    Background_SetTileBackgroundNumber(BackgroundNumber_0, 0);
    Background_SetScreenBaseBlock(BackgroundNumber_0, 30);
    REG_BG0CNT = BG_16_COLOR | BG_SIZE_0 | TILE_BASE(0) | MAP_BASE(30);

    setupTilemap();
    updateTilemapEntries();

    setupSprites();
    ObjectAttributeBuffer_CopyBufferToMemory();

    int x = 96;
    int y = 32;

    struct ObjectAttribute *whale = &objectAttributeBuffer[0];
    ObjectAttribute_SetShape(whale, ObjectAttributeShape_Square);
    ObjectAttribute_SetSize(whale, ObjectAttributeSize_16);
    ObjectAttribute_SetPaletteBank(whale, 0);
    ObjectAttribute_SetDisplayMode(whale, ObjectAttributeDisplayMode_Normal);
    ObjectAttribute_SetGraphicsMode(whale, ObjectAttributeGraphicsMode_Normal);

    int tile = 0;

    setRepeat(1, 1);

    int bobbing = 0;

#define BOBBING_MAX_DELAY 20
    int bobbingTime = BOBBING_MAX_DELAY;

    bool blowing = false;
#define BLOWING_FRAMES 8
#define BLOWING_FRAME_SKIP 4
    int blowingFrame = 0;

#define TILE_UPDATE_DELAY 40
    int tileUpdate = TILE_UPDATE_DELAY;

    // 0 = left
    // 1 = up
    // 2 = right
    // 3 = down
    int direction = 0;

    while (true)
    {
        scanKeys();
        u16 keyState = keysDownRepeat();
        int speed = 0;

        if (keyState & KEY_UP)
        {
            direction = 1;
            speed = 1;
        }

        if (keyState & KEY_LEFT)
        {
            direction = 0;
            speed = 1;
        }

        if (keyState & KEY_RIGHT)
        {
            direction = 2;
            speed = 1;
        }

        if (keyState & KEY_DOWN)
        {
            direction = 3;
            speed = 1;
        }

        if (keyState & KEY_A)
        {
            blowing = true;
        }

        switch (direction)
        {
        case 0:
            x -= speed;
            x = max(x, 0);
            tile = blowing ? 4 + blowingFrame / BLOWING_FRAME_SKIP : 0;
            ObjectAttribute_SetHFlip(whale, false);
            break;
        case 1:
            y -= speed;
            y = max(y, 0);
            tile = blowing ? 22 + blowingFrame / BLOWING_FRAME_SKIP : 3;
            ObjectAttribute_SetHFlip(whale, false);
            break;
        case 2:
            x += speed;
            x = min(x, SCREEN_WIDTH - 16);
            tile = blowing ? 4 + blowingFrame / BLOWING_FRAME_SKIP : 2;
            ObjectAttribute_SetHFlip(whale, blowing);
            break;
        case 3:
            y += speed;
            y = min(y, SCREEN_HEIGHT - 16);
            tile = blowing ? 13 + blowingFrame / BLOWING_FRAME_SKIP : 1;
            ObjectAttribute_SetHFlip(whale, false);
            break;
        }

        if (blowing)
        {
            if (blowingFrame >= BLOWING_FRAMES * BLOWING_FRAME_SKIP)
            {
                blowing = 0;
                blowingFrame = 0;
            }
            else
            {
                blowingFrame++;
            }
        }

        ObjectAttribute_SetTile(whale, tile * 4);
        ObjectAttribute_SetPos(whale, x, y + bobbing);

        if (--bobbingTime == 0)
        {
            bobbing = !bobbing;
            bobbingTime = BOBBING_MAX_DELAY;
        }

        if (--tileUpdate == 0)
        {
            tileUpdate = TILE_UPDATE_DELAY;
            updateTilemapEntries();
        }

        VBlankIntrWait();
        ObjectAttributeBuffer_CopyBufferToMemory();
    }
}