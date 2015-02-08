#include <avr/io.h>
#include <stdlib.h>

#include "../../../src/def.h"
#include "../../../src/Graphic.h"

#include <util/delay.h>

// Draw color 5x5 rectangles
void demo0()
{
  gfx_ClearBuf();
  
  const uint8_t count = 5;
  const uint8_t dx = TFT_ScreenWidth / count, dy = TFT_ScreenHeight / count;

  uint8_t x, y;
  for (x = 0; x < TFT_ScreenWidth;  x += dx)
  for (y = 0; y < TFT_ScreenHeight; y += dy)
  {
    const uint8_t color = rand() % COLOR_TABLE_MAX;
    gfx_FillRectangle(x, y, x + dx, y + dy, ColorIdx(color));
  }

  gfx_Refresh();
  _delay_ms(2000);
}

void demo1()
{
  gfx_ClearBuf();

  gfx_DrawLine(10, 10, TFT_ScreenWidth - 10, 10,   COLOR_BLACK);
  gfx_DrawLine(10, 10, 10,  TFT_ScreenHeight - 10, COLOR_BLACK);
  gfx_DrawLine(10, 10, TFT_ScreenWidth - 10, TFT_ScreenHeight - 10, COLOR_BLACK);
  gfx_DrawLine(TFT_ScreenWidth - 10, 10, TFT_ScreenWidth - 10, TFT_ScreenHeight - 10, COLOR_BLACK);
  gfx_DrawLine(10, TFT_ScreenHeight - 10, TFT_ScreenWidth - 10, TFT_ScreenHeight - 10, COLOR_BLACK);
  
  gfx_Refresh();
  _delay_ms(2000);
}

// Draw random lines
void demo2()
{
  static bool isDashed = false;

  gfx_ClearBuf();

  for (uint8_t i = 0; i < 100; i++)
  {
    gfx_DrawLine(
        rand() % TFT_ScreenWidth,
        rand() % TFT_ScreenHeight,
        rand() % TFT_ScreenWidth,
        rand() % TFT_ScreenHeight,
        ColorIdx(rand() % COLOR_TABLE_MAX),
        isDashed
      );
  }

  gfx_Refresh();
  _delay_ms(2000);

  isDashed = !isDashed;
}

// Draw random rects
void demo3()
{
  static bool isDashed = false;

  gfx_ClearBuf();

  for (uint8_t i = 0; i < 50; i++)
  {
    gfx_DrawRectangle(
        rand() % TFT_ScreenWidth,
        rand() % TFT_ScreenHeight,
        rand() % TFT_ScreenWidth,
        rand() % TFT_ScreenHeight,
        ColorIdx(rand() % COLOR_TABLE_MAX),
        isDashed
      );
  }

  gfx_Refresh();
  _delay_ms(2000);

  isDashed = !isDashed;
}

// Print text
void demo4()
{
  const ColorIdx StartColor = COLOR_BLACK;
  
  static ColorIdx color = StartColor;

  gfx_ClearBuf();

  gfx_PrintString(10, 10, "Hello World!", color, Font_4x6);

  gfx_PrintString(10, 20, "Hello World!", color, Font_6x8);

  gfx_PrintString(10, 30, "Hello World!", color, Font_8x8);

  gfx_PrintString(10, 40, "Hello World!", color, Font_8x12);

  gfx_PrintString(10, 55, "Hello World!", color, Font_12x16);
  
  gfx_Refresh();
  _delay_ms(4000);

  if (color < COLOR_TABLE_MAX - 1)
    color = ColorIdx(int8_t(color) + 1);
  else
    color = StartColor;
}

int main(void)
{
  // PORTB.0, PORTB.1, PORTB.2 and PORTB.3 use by SPI in TFT display
  PORTB = 0xFF;
  DDRB  = 0x0F;

  // PORTL.0, PORTL.1 and PORTL.2 use as GPIO in TFT display
  PORTL = 0xFF;
  DDRL  = 0x07;

  gfx_Init();

  while (1)
  {
    demo0();

    demo1();

    demo2();
    demo2();

    demo3();
    demo3();

    demo4();
    demo4();
    demo4();
  }
}

