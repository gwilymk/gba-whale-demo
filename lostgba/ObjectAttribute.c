#include <lostgba/ObjectAttribute.h>

struct ObjectAttribute objectAttributeBuffer[ObjectAttributeBuffer_Length];
struct ObjectAffine *objectAffineBuffer = (struct ObjectAffine *)objectAttributeBuffer;

#define SET_BITS(obj, value, mask, shift)                                       \
    do                                                                          \
    {                                                                           \
        obj = ((obj) & ~((mask) << (shift))) | (((value) & (mask)) << (shift)); \
    } while (0)

void ObjectAttribute_SetPos(struct ObjectAttribute *attr, int x, int y)
{
    SET_BITS(attr->attr0, y, 0xFF, 0);
    SET_BITS(attr->attr1, x, 0x1FF, 0);
}

void ObjectAttribute_SetDisplayMode(struct ObjectAttribute *attr, enum ObjectAttributeDisplayMode displayMode)
{
    SET_BITS(attr->attr0, displayMode, 0x3, 8);
}

void ObjectAttribute_SetGraphicsMode(struct ObjectAttribute *attr, enum ObjectAttributeGraphicsMode graphicsMode)
{
    SET_BITS(attr->attr0, graphicsMode, 0x3, 10);
}

void ObjectAttribute_SetMosaicEnabled(struct ObjectAttribute *attr, bool enabled)
{
    SET_BITS(attr->attr0, enabled, 0x1, 12);
}

void ObjectAttribute_SetColourMode(struct ObjectAttribute *attr, enum ObjectAttributeColourMode colourMode)
{
    SET_BITS(attr->attr0, colourMode, 0x3, 13);
}

void ObjectAttribute_SetShape(struct ObjectAttribute *attr, enum ObjectAttributeShape shape)
{
    SET_BITS(attr->attr0, shape, 0x3, 14);
}

void ObjectAttribute_SetHFlip(struct ObjectAttribute *attr, bool hflip)
{
    SET_BITS(attr->attr1, hflip, 0x1, 12);
}

void ObjectAttribute_SetVFlip(struct ObjectAttribute *attr, bool vflip)
{
    SET_BITS(attr->attr1, vflip, 0x1, 13);
}

void ObjectAttribute_SetSize(struct ObjectAttribute *attr, enum ObjectAttributeSize size)
{
    SET_BITS(attr->attr1, size, 0x3, 14);
}

void ObjectAttribute_SetTile(struct ObjectAttribute *attr, u32 tileId)
{
    SET_BITS(attr->attr2, tileId, 0x3FF, 0);
}

void ObjectAttribute_SetPriority(struct ObjectAttribute *attr, u16 priority)
{
    SET_BITS(attr->attr2, priority, 0x3, 10);
}

void ObjectAttribute_SetPaletteBank(struct ObjectAttribute *attr, u16 paletteBank)
{
    SET_BITS(attr->attr2, paletteBank, 0xF, 12);
}

#define OBJECT_ATTRIBUTE_MEMORY_LOCATION ((struct ObjectAttribute *)(void *)0x07000000)

void ObjectAttributeBuffer_CopyBufferToMemory(void)
{
    struct ObjectAttribute *objectAttributeSystemMemoryLocation = OBJECT_ATTRIBUTE_MEMORY_LOCATION;

    for (int i = 0; i < ObjectAttributeBuffer_Length; i++)
    {
        objectAttributeSystemMemoryLocation[i] = objectAttributeBuffer[i];
    }
}