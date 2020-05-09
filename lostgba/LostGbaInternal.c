#include "LostGbaInternal.h"

static u16 nOnes(int n)
{
    return (1 << n) - 1;
}

void LostGBA_SetBits16(u16 *target, u16 value, u16 length, u16 shift)
{
    u16 mask = nOnes(length);
    (*target) = (*target & ~(mask << shift)) | ((value & mask) << shift);
}