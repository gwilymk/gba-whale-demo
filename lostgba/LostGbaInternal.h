/**
 * @file LostGbaInternal.h
 */

#pragma once

#include <lostgba/GbaTypes.h>

/**
 * @brief Utility function to set bits at a certain location
 * 
 * Target gets the bits from shift - shift + length exclusive set to value
 */
void LostGBA_SetBits16(u16 *target, u16 value, u16 length, u16 shift);