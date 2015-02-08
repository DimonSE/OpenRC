#ifndef __TFT_ST7735_h__
#define __TFT_ST7735_h__

//
// Public variable
//

extern uint8_t DisplayBuffer[TFT_ScreenWidth][TFT_ScreenHeight / 8];

//
// Public functions
//

void LCD_Init();
void LCD_Clear();
void LCD_Refresh();

void LCD_DrawPixel(const uint8_t x, const uint8_t y, const ColorIdx color);

#endif // __TFT_ST7735_h__