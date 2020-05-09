#include <gba_systemcalls.h>

#include <lostgba/Interrupt.h>
#include <lostgba/ObjectAttribute.h>
#include <lostgba/TileMap.h>
#include <lostgba/Background.h>
#include <lostgba/Input.h>
#include <lostgba/Graphics.h>

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
    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            int r = randomNumber();
            int tileToUse = r & 1;

            Background_SetTile(30, BackgroundSize_32x32, x, y, tileToUse, false, false, 0);
        }
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
    struct GraphicsSettings graphicsSettings = {
        .graphicsMode = GraphicsMode_0,
        .enableBG0 = true,
        .enableSprites = true};
    Graphics_SetMode(graphicsSettings);

    Interrupt_Init();
    Interrupt_EnableType(InterruptType_VBlank);
    Interrupt_Enable();

    Background_SetColourMode(BackgroundNumber_0, BackgroundColourMode_4PP);
    Background_SetSize(BackgroundNumber_0, BackgroundSize_32x32);
    Background_SetScreenBaseBlock(BackgroundNumber_0, 30);
    Background_SetTileBackgroundNumber(BackgroundNumber_0, 0);

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
        Input_UpdateKeyState();
        int speed = 0;

        if (Input_IsKeyDown(InputKey_Up))
        {
            direction = 1;
            speed = 1;
        }

        if (Input_IsKeyDown(InputKey_Left))
        {
            direction = 0;
            speed = 1;
        }

        if (Input_IsKeyDown(InputKey_Right))
        {
            direction = 2;
            speed = 1;
        }

        if (Input_IsKeyDown(InputKey_Down))
        {
            direction = 3;
            speed = 1;
        }

        if (Input_IsNewlyPressed(InputKey_A))
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
            x = min(x, Graphics_ScreenWidth - 16);
            tile = blowing ? 4 + blowingFrame / BLOWING_FRAME_SKIP : 2;
            ObjectAttribute_SetHFlip(whale, blowing);
            break;
        case 3:
            y += speed;
            y = min(y, Graphics_ScreenHeight - 16);
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