/*******************************************************************************
*  Graphic.h
*
*  Радиоуправление: Высокоуравневые функции работы с ЖКИ, заголовок
*
*/// ***************************************************************************

#ifndef __Graphic_h__
#define __Graphic_h__

//
// Public variables
//

struct FontProfile
{
    uint8_t        W; // Width of character
    uint8_t        H; // Height of character
    const uint8_t* FontData;
    uint8_t        BytesPerChar;
    uint8_t        BytesPerColumn;
};

extern FontProfile Font_4x6;
extern FontProfile Font_6x8;
extern FontProfile Font_8x8;
extern FontProfile Font_8x12;
extern FontProfile Font_12x16;

enum Align
{
  ALIGN_LEFT = 0,
  ALIGN_CENTER,
  ALIGN_RIGHT
};

//
// Public functions
//

void gfx_Init();

void gfx_Refresh();

void gfx_ClearBuf();

void gfx_DrawPixel(const uint8_t x, const uint8_t y, const ColorIdx color);
void gfx_DrawLine(const uint8_t x0, const uint8_t y0, const uint8_t x1, const uint8_t y1, const ColorIdx color, const bool IsDashed = false);
void gfx_DrawRectangle(const uint8_t x0, const uint8_t y0, const uint8_t x1, const uint8_t y1, const ColorIdx color, const bool IsDashed = false);
void gfx_FillRectangle(const uint8_t x0, const uint8_t y0, const uint8_t x1, const uint8_t y1, const ColorIdx color);

void gfx_PrintChar  (const uint8_t x, const uint8_t y, const char ch,   const ColorIdx color, const FontProfile & font);
void gfx_PrintString(const uint8_t x, const uint8_t y, const char* str, const ColorIdx color, const FontProfile & font, Align align = ALIGN_LEFT);

#endif // __Graphic_h__