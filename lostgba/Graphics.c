#include <lostgba/Graphics.h>
#include "LostGbaInternal.h"

static vu16 *Graphics_displayControlRegister = (vu16 *)0x4000000;

void Graphics_SetMode(struct GraphicsSettings settings)
{
    bool sprites1d = true;

    u16 mode = (settings.graphicsMode & LostGBA_AllOnes16(3)) |
               (sprites1d << 6) |
               (settings.enableBG0 << 8) |
               (settings.enableBG1 << 9) |
               (settings.enableBG2 << 10) |
               (settings.enableBG3 << 11) |
               (settings.enableSprites << 12);

    *Graphics_displayControlRegister = mode;
}