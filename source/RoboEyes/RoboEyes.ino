#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>

#include "EyeAtlas.h"

// --------------------------------------------------
// OLED Configuration
// --------------------------------------------------
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT   64
#define OLED_ADDR      0x3C
#define OLED_RESET     -1

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// --------------------------------------------------
// Tile / Grid Geometry
// --------------------------------------------------
#define TILE_WIDTH      64
#define TILE_HEIGHT     32
#define GRID_COLS        7
#define GRID_ROWS        7
#define NUM_TILES       49

// --------------------------------------------------
// Tile buffer (64x32 / 8 = 256 bytes)
// --------------------------------------------------
uint8_t tileBuffer[TILE_WIDTH * TILE_HEIGHT / 8];

// --------------------------------------------------
// Bit helper
// --------------------------------------------------
inline bool getBit(const uint8_t* buf, uint16_t w, uint16_t x, uint16_t y)
{
  uint16_t byteIndex = (y * w + x) >> 3;
  uint8_t  bitMask   = 0x80 >> (x & 7);
  return buf[byteIndex] & bitMask;
}

// --------------------------------------------------
// Extract tile from PROGMEM atlas
// --------------------------------------------------
void extractTile(uint8_t tileIndex)
{
  uint8_t row = tileIndex / GRID_COLS;
  uint8_t col = tileIndex % GRID_COLS;

  uint16_t srcX = col * TILE_WIDTH;
  uint16_t srcY = row * TILE_HEIGHT;

  uint16_t atlasStride = ATLAS_WIDTH / 8;
  uint16_t tileStride  = TILE_WIDTH / 8;

  for (uint8_t y = 0; y < TILE_HEIGHT; y++) {
    uint16_t srcOffset =
      (srcY + y) * atlasStride + (srcX >> 3);

    uint16_t dstOffset = y * tileStride;

    for (uint8_t x = 0; x < tileStride; x++) {
      tileBuffer[dstOffset + x] =
        pgm_read_byte(&epd_bitmap_Eye_Expressions_64x32_Binary[srcOffset + x]);
    }
  }
}

// --------------------------------------------------
// Draw scaled 2× directly (NO extra buffer)
// --------------------------------------------------
void drawTileScaled()
{
  display.clearDisplay();

  for (uint8_t y = 0; y < TILE_HEIGHT; y++) {
    for (uint8_t x = 0; x < TILE_WIDTH; x++) {
      if (getBit(tileBuffer, TILE_WIDTH, x, y)) {

        uint8_t dx = x * 2;
        uint8_t dy = y * 2;

        display.drawPixel(dx,     dy,     SSD1306_WHITE);
        display.drawPixel(dx + 1, dy,     SSD1306_WHITE);
        display.drawPixel(dx,     dy + 1, SSD1306_WHITE);
        display.drawPixel(dx + 1, dy + 1, SSD1306_WHITE);
      }
    }
  }

  display.display();
}

// --------------------------------------------------
// Setup
// --------------------------------------------------
void setup()
{
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    while (true); // OLED init failed
  }

  display.clearDisplay();
  display.display();
}

// --------------------------------------------------
// Loop
// --------------------------------------------------
void loop()
{
  static uint8_t index = 0;

  extractTile(index);
  drawTileScaled();

  index = (index + 1) % NUM_TILES;
  delay(500);
}
