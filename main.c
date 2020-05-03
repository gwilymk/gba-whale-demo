#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>

#include <string.h>

#include <whale.h>
#include <tilemap.h>

typedef struct
{
    u16 attr0;
    u16 attr1;
    u16 attr2;
    s16 fill;
} ALIGN(4) ObjectAttr;

typedef struct
{
    u16 fill0[3];
    s16 pa;
    u16 fill1[3];
    s16 pb;
    u16 fill2[3];
    s16 pc;
    u16 fill3[3];
    s16 pd;
} ALIGN(4) ObjAffine;

ObjectAttr objBuffer[128];
ObjAffine *const objAffineBuffer = (ObjAffine *)objBuffer;

ObjectAttr *objectSetAttr(ObjectAttr *obj, u16 a0, u16 a1, u16 a2)
{
    obj->attr0 = a0;
    obj->attr1 = a1;
    obj->attr2 = a2;

    return obj;
}

void objectSetPos(ObjectAttr *obj, int x, int y)
{
    obj->attr0 = (obj->attr0 & ~0xFF) | (y & 0xFF);
    obj->attr1 = (obj->attr1 & ~0x1FF) | (x & 0x1FF);
}

#define ObjectMode_Normal 0
#define ObjectMode_Affine 1
#define ObjectMode_Hidden 2
#define ObjectMode_DoubleRender 3

void objectSetObjectMode(ObjectAttr *obj, u16 mode)
{
    obj->attr0 = (obj->attr0 & ~(0x3 << 8)) | (mode << 8);
}

#define ObjectGfxMode_Normal 0
#define ObjectGfxMode_Alpha 1

void objectSetGfxMode(ObjectAttr *obj, u16 mode)
{
    obj->attr0 = (obj->attr0 & ~(0x3 << 10)) | (mode << 10);
}

void objectSetMosaicEnabled(ObjectAttr *obj, bool enabled)
{
    obj->attr0 = (obj->attr0 & ~(0x1 << 12)) | (enabled << 12);
}

#define ObjectColourMode_4PP 0
#define ObjectColourMode_8PP 1

void objectSetColourMode(ObjectAttr *obj, u16 mode)
{
    obj->attr0 = (obj->attr0 & ~(0x1 << 13)) | (mode << 13);
}

#define ObjectShape_Square 0
#define ObjectShape_Wide 1
#define ObjectShape_Tall 2

void objectSetShape(ObjectAttr *obj, u16 shape)
{
    obj->attr0 = (obj->attr0 & ~(0x3 << 14)) | (shape << 14);
}

#define ObjectSize_8 0
#define ObjectSize_16 1
#define ObjectSize_32 2
#define ObjectSize_64 3

void objectSetSize(ObjectAttr *obj, u16 size)
{
    obj->attr1 = (obj->attr1 & ~(0x3 << 14)) | ((size & 0x3) << 14);
}

void objectSetHFlip(ObjectAttr *obj, bool hflip)
{
    obj->attr1 = (obj->attr1 & ~(0x1 << 12)) | (hflip << 12);
}

void objectSetTile(ObjectAttr *obj, u32 tileid)
{
    obj->attr2 = (obj->attr2 & ~(0x3FF)) | (tileid & 0x3FF);
}

void objectSetPriority(ObjectAttr *obj, u16 priority)
{
    obj->attr2 = (obj->attr2 & ~(0x3 << 10)) | ((priority & 0x3) << 10);
}

void objectSetPaletteBank(ObjectAttr *obj, u16 palette)
{
    obj->attr2 = (obj->attr2 & ~(0xF << 12)) | ((palette & 0xF) << 12);
}

void setupSprites()
{
    memcpy(SPRITE_PALETTE, whalePal, whalePalLen);
    memcpy(TILE_BASE_ADR(4), whaleTiles, whaleTilesLen);

    for (int i = 0; i < 128; i++)
    {
        objectSetObjectMode(objBuffer + i, ObjectMode_Hidden);
    }
}

void setupTilemap()
{
    memcpy(BG_PALETTE, tilemapPal, tilemapPalLen);
    memcpy(TILE_BASE_ADR(0), tilemapTiles, tilemapTilesLen);
}

#define OAM_MEM ((u32 *)0x07000000)

void objectCopyBuffer()
{
    memcpy(OAM_MEM, objBuffer, sizeof(ObjectAttr) * 128);
}

u32 randomNumber()
{
    static u32 state = 12023908;

    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;

    return state;
}

void updateTilemapEntries()
{
    for (int i = 0; i < 32 * 32; i++)
    {
        int r = randomNumber();

        s16 tileToUse = !(r & 0x4);

        s16 screenEntry = tileToUse;
        ((s16 *)MAP_BASE_ADR(30))[i] = screenEntry;
    }
}

int main()
{
    irqInit();
    irqEnable(IRQ_VBLANK);

    REG_IME = 1; // allow interrupts

    SetMode(MODE_0 | BG0_ENABLE | OBJ_ENABLE | OBJ_1D_MAP);
    REG_BG0CNT = BG_16_COLOR | BG_SIZE_0 | TILE_BASE(0) | MAP_BASE(30);

    setupTilemap();
    updateTilemapEntries();

    setupSprites();
    objectCopyBuffer();

    int x = 96;
    int y = 32;

    ObjectAttr *whale = &objBuffer[0];
    objectSetShape(whale, ObjectShape_Square);
    objectSetSize(whale, ObjectSize_16);
    objectSetPaletteBank(whale, 0);
    objectSetObjectMode(whale, ObjectMode_Normal);
    objectSetGfxMode(whale, ObjectGfxMode_Normal);

    int tile = 0;

    setRepeat(1, 1);

    int bobbing = 0;

#define BOBBING_MAX_DELAY 20
    int bobbingTime = BOBBING_MAX_DELAY;

    bool blowing = false;
#define BLOWING_FRAMES 8
#define BLOWING_FRAME_SKIP 4
    int blowingFrame = 0;

#define TILE_UPDATE_DELAY 100
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
            tile = blowing ? 4 + blowingFrame / BLOWING_FRAME_SKIP : 0;
            objectSetHFlip(whale, false);
            break;
        case 1:
            y -= speed;
            tile = blowing ? 22 + blowingFrame / BLOWING_FRAME_SKIP : 3;
            objectSetHFlip(whale, false);
            break;
        case 2:
            x += speed;
            tile = blowing ? 4 + blowingFrame / BLOWING_FRAME_SKIP : 2;
            objectSetHFlip(whale, blowing);
            break;
        case 3:
            y += speed;
            tile = blowing ? 13 + blowingFrame / BLOWING_FRAME_SKIP : 1;
            objectSetHFlip(whale, false);
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

        objectSetTile(whale, tile * 4);
        objectSetPos(whale, x, y + bobbing);

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
        objectCopyBuffer();
    }
}