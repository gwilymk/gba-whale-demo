#pragma once

#include <stdbool.h>
#include "GbaTypes.h"

struct ObjectAttribute
{
    u16 attr0;
    u16 attr1;
    u16 attr2;
    s16 fill;
} LOSTGBA_ALIGN(4);

void ObjectAttribute_SetPos(struct ObjectAttribute *attr, int x, int y);

enum ObjectAttributeDisplayMode
{
    ObjectAttributeDisplayMode_Normal,
    ObjectAttributeDisplayMode_Affine,
    ObjectAttributeDisplayMode_Hidden,
    ObjectAttributeDisplayMode_DoubleRender,
};
void ObjectAttribute_SetDisplayMode(struct ObjectAttribute *attr, enum ObjectAttributeDisplayMode displayMode);

enum ObjectAttributeGraphicsMode
{
    ObjectAttributeGraphicsMode_Normal,
    ObjectAttributeGraphicsMode_Alpha,
};
void ObjectAttribute_SetGraphicsMode(struct ObjectAttribute *attr, enum ObjectAttributeGraphicsMode graphicsMode);

void ObjectAttribute_SetMosaicEnabled(struct ObjectAttribute *attr, bool enabled);

enum ObjectAttributeColourMode
{
    ObjectAttributeColourMode_4PP,
    ObjectAttributeColourMode_8PP,
};
void ObjectAttribute_SetColourMode(struct ObjectAttribute *attr, enum ObjectAttributeColourMode colourMode);

enum ObjectAttributeShape
{
    ObjectAttributeShape_Square,
    ObjectAttributeShape_Wide,
    ObjectAttributeShape_Tall,
};
void ObjectAttribute_SetShape(struct ObjectAttribute *attr, enum ObjectAttributeShape shape);

enum ObjectAttributeSize
{
    ObjectAttributeSize_8,
    ObjectAttributeSize_16,
    ObjectAttributeSize_32,
    ObjectAttributeSize_64,
};
void ObjectAttribute_SetHFlip(struct ObjectAttribute *attr, bool hflip);
void ObjectAttribute_SetVFlip(struct ObjectAttribute *attr, bool vflip);
void ObjectAttribute_SetSize(struct ObjectAttribute *attr, enum ObjectAttributeSize size);
void ObjectAttribute_SetTile(struct ObjectAttribute *attr, u32 tileId);
void ObjectAttribute_SetPriority(struct ObjectAttribute *attr, u16 priority);
void ObjectAttribute_SetPaletteBank(struct ObjectAttribute *attr, u16 paletteBank);

#define ObjectAttributeBuffer_Length 128

extern struct ObjectAttribute objectAttributeBuffer[ObjectAttributeBuffer_Length];
void ObjectAttributeBuffer_CopyBufferToMemory(void);

struct ObjectAffine
{
    u16 fill0[3];
    s16 pa;
    u16 fill1[3];
    s16 pb;
    u16 fill2[3];
    s16 pc;
    u16 fill3[3];
    s16 pd;
} LOSTGBA_ALIGN(4);
