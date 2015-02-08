/*******************************************************************************
*  Graphic.cpp
*
*  Радиоуправление: Высокоуравневые функции работы с ЖКИ
*
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "def.h"
#include "Graphic.h"
#include "Fonts.h"
#include "Utils.h"
#include "hardware/TFT_ST7735.h"

//
// Public variables
//

FontProfile Font_4x6;
FontProfile Font_6x8;
FontProfile Font_8x8;
FontProfile Font_8x12;
FontProfile Font_12x16;

//
// Public functions
//

void gfx_Refresh()
{
    LCD_Refresh();
}

void gfx_ClearBuf()
{
    LCD_Clear();
}

void gfx_DrawPixel(const uint8_t x, const uint8_t y, const ColorIdx color)
{
    LCD_DrawPixel(x, y, color);
}

void gfx_DrawLine(
        const uint8_t  x0,
        const uint8_t  y0,
        const uint8_t  x1,
        const uint8_t  y1,
        const ColorIdx color,
        const bool     IsDashed
    )
{
    uint8_t x = x0;
    uint8_t y = y0;

    int16_t dx = x1 - x;
    int16_t dy = y1 - y;

    int8_t stepx = dx < 0 ? -1 : 1;
    int8_t stepy = dy < 0 ? -1 : 1;

    if (dx < 0)
        dx = -dx;

    if (dy < 0)
        dy = -dy;

    dx = mul2(dx);
    dy = mul2(dy);

    gfx_DrawPixel(x, y, color);

    int16_t fraction;
    bool visible = true;
    if (dx > dy)
    {
        fraction = dy - div2(dx);
        while (x != x1)
        {
            if (fraction >= 0)
            {
                y += stepy;
                fraction -= dx;
            }

            x += stepx;
            fraction += dy;

            if (IsDashed)
                visible = !visible;

            if (visible)
                gfx_DrawPixel(x, y, color);
        }
    }
    else
    {
        fraction = dx - div2(dy);
        while (y != y1)
        {
            if (fraction >= 0)
            {
                x += stepx;
                fraction -= dy;
            }

            y += stepy;
            fraction += dx;

            if (IsDashed)
                visible = !visible;

            if (visible)
                gfx_DrawPixel(x, y, color);
        }
    }
}

void gfx_DrawRectangle(
        const uint8_t  x0,
        const uint8_t  y0,
        const uint8_t  x1,
        const uint8_t  y1,
        const ColorIdx color,
        const bool     IsDashed
    )
{
    gfx_DrawLine(x0, y0, x0, y1, color, IsDashed);
    gfx_DrawLine(x0, y1, x1, y1, color, IsDashed);
    gfx_DrawLine(x1, y0, x1, y1, color, IsDashed);
    gfx_DrawLine(x0, y0, x1, y0, color, IsDashed);
}

void gfx_FillRectangle(
        const uint8_t  x0,
        const uint8_t  y0,
        const uint8_t  x1,
        const uint8_t  y1,
        const ColorIdx color
    )
{
    for(uint8_t i = x0; i <= x1; ++i)
        for(uint8_t j = y0; j <= y1; ++j)
            LCD_DrawPixel(i, j, color);
}

void gfx_PrintChar(const uint8_t x, const uint8_t y, const char ch, const ColorIdx color, const FontProfile & font)
{
    const uint8_t BIT_IN_BYTE = 8;

    uint8_t byte;
    const uint8_t* addr;
    for(uint8_t i = 0; i < font.W; ++i)
    {
        addr = font.FontData + uint8_t(ch) * font.BytesPerChar + (i * font.BytesPerColumn);
        for(uint8_t j = 0; j < font.H; ++j)
        {
            if ((j % BIT_IN_BYTE) == 0)
                byte = pgm_read_byte(addr++);

            if (byte & 0x01)
              LCD_DrawPixel(x + i, y + j, color);
            
            byte = div2(byte);
        }
    }
}

void gfx_PrintString(const uint8_t x, const uint8_t y, const char* str, const ColorIdx color, const FontProfile & font, Align align)
{
    uint8_t _x;

    switch (align)
    {
        case ALIGN_LEFT:
            _x = x;
            break;
        
        case ALIGN_CENTER:
            _x = x - strlen(str) * font.W / 2;
            break;

        case ALIGN_RIGHT:
            _x = x - strlen(str) * font.W;
            break;
    };

    while(*str !='\0')
    {
        gfx_PrintChar(_x, y, *str++, color, font);
        _x += font.W;
    }
}

void gfx_Init(void)
{
    LCD_Init();

    Font_4x6.W = 4;
    Font_4x6.H = 6;
    Font_4x6.FontData = (const uint8_t*)font4x6;
    Font_4x6.BytesPerChar = 4;
    Font_4x6.BytesPerColumn = 1;
    
    Font_6x8.W = 6;
    Font_6x8.H = 8;
    Font_6x8.FontData = (const uint8_t*)font6x8;
    Font_6x8.BytesPerChar = 8;
    Font_6x8.BytesPerColumn = 1;

    Font_8x8.W = 8;
    Font_8x8.H = 8;
    Font_8x8.FontData = (const uint8_t*)font8x8;
    Font_8x8.BytesPerChar = 8;
    Font_8x8.BytesPerColumn = 1;

    Font_8x12.W = 8;
    Font_8x12.H = 12;
    Font_8x12.FontData = (const uint8_t*)font8x12;
    Font_8x12.BytesPerChar = 16;
    Font_8x12.BytesPerColumn = 2;

    Font_12x16.W = 12;
    Font_12x16.H = 16;
    Font_12x16.FontData = (const uint8_t*)font12x16;
    Font_12x16.BytesPerChar = 24;
    Font_12x16.BytesPerColumn = 2;
}