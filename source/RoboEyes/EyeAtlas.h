#ifndef EYE_ATLAS_H
#define EYE_ATLAS_H

#include <avr/pgmspace.h>

// Atlas dimensions (must match generator)
#define ATLAS_WIDTH   448
#define ATLAS_HEIGHT  224

// Extern declaration (NO data here)
extern const unsigned char epd_bitmap_Eye_Expressions_64x32_Binary[] PROGMEM;

#endif // EYE_ATLAS_H
