/**********************************
/   This is a library for the TFT display on ST7735 chip
/
/
/ This library based on https://github.com/adafruit/Adafruit-ST7735-Library 
/ written by Limor Fried/Ladyada for Adafruit Industries. Thank him!
/
/ Its Open Source library by GPL license
************************************/

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "../def.h"
#include "TFT_ST7735.h"

#include <util/delay.h>

//
// Public variables
//

uint16_t COLOR_IDX_TO_RGB[COLOR_TABLE_MAX] =
{
  0xFF00, // COLOR_BACKG
  0x0000  // COLOR_FRONT
};

// Display buffer
// *-+-+----> X
// |b|b|
// |y|y|
// |t|t|
// |e|e|
// +-+-+
// |
// \/
//  Y
//
// DisplayBuffer[x][y], in Y coord 1 byte have 8 px
uint8_t DisplayBuffer[TFT_ScreenWidth][TFT_ScreenHeight / 8];

//
// Macros for optimization
//

#define TFT_START_WRITE_COMMAND { TFT_CS = 0; TFT_RS = 0; }
#define TFT_STOP_WRITE_COMMAND  { TFT_CS = 1; }

#define TFT_START_WRITE_DATA { TFT_CS = 0; TFT_RS = 1; }
#define TFT_STOP_WRITE_DATA  { TFT_CS = 1; }

#define TFT_SPI_WRITE(data) { SPDR = data; while( !(SPSR & (1<<SPIF)) ); }

//
// LCD registers mapping
//

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

//
// Private functions
//

void LCD_WriteCommand(const int8_t cmd)
{
  TFT_START_WRITE_COMMAND

  TFT_SPI_WRITE(cmd);

  TFT_STOP_WRITE_COMMAND
}

void LCD_WriteData(const int8_t data)
{
  TFT_START_WRITE_DATA

  TFT_SPI_WRITE(data);

  TFT_STOP_WRITE_DATA
}

void LCD_SetAddrWindow(const uint8_t x0, const uint8_t y0, const uint8_t x1, const uint8_t y1)
{
  // Column addr set
  LCD_WriteCommand(ST7735_CASET);
  LCD_WriteData(0x00);
  LCD_WriteData(y0); // YSTART
  LCD_WriteData(0x00);
  LCD_WriteData(y1); // YEND

  // Row addr set
  LCD_WriteCommand(ST7735_RASET);
  LCD_WriteData(0x00);
  LCD_WriteData(x0); // XSTART 
  LCD_WriteData(0x00);
  LCD_WriteData(x1); // XEND

  LCD_WriteCommand(ST7735_RAMWR); // write to RAM
}

void LCD_Reset()
{
  TFT_RST = 0;
  _delay_ms(10);

  TFT_RST = 1;
  _delay_ms(10);
}

//
// Public functions
//

void LCD_Init()
{
  // SPI initialization
  // SPI Type: Master
  // SPI Clock Rate: Fosc/4
  // SPI Clock Phase: Cycle Half
  // SPI Clock Polarity: Low
  // SPI Data Order: MSB First
  SPCR = 0x50;
  SPSR = 0x00;

  LCD_Reset();
  
  LCD_WriteCommand(ST7735_SLPOUT); //Sleep exit 
  _delay_ms(12);

  //ST7735R Frame Rate
  LCD_WriteCommand(ST7735_FRMCTR1);
  LCD_WriteData(0x01);
  LCD_WriteData(0x2C);
  LCD_WriteData(0x2D); 

  LCD_WriteCommand(ST7735_FRMCTR2);
  LCD_WriteData(0x01);
  LCD_WriteData(0x2C);
  LCD_WriteData(0x2D); 

  LCD_WriteCommand(ST7735_FRMCTR3); 
  LCD_WriteData(0x01);
  LCD_WriteData(0x2C);
  LCD_WriteData(0x2D); 
  LCD_WriteData(0x01);
  LCD_WriteData(0x2C);
  LCD_WriteData(0x2D); 

  // Column inversion
  LCD_WriteCommand(ST7735_INVCTR);
  LCD_WriteData(0x07); 
   
  //ST7735R Power Sequence
  LCD_WriteCommand(ST7735_PWCTR1); 
  LCD_WriteData(0xA2);
  LCD_WriteData(0x02);
  LCD_WriteData(0x84); 
  
  LCD_WriteCommand(ST7735_PWCTR2);
  LCD_WriteData(0xC5); 
  
  LCD_WriteCommand(ST7735_PWCTR3); 
  LCD_WriteData(0x0A);
  LCD_WriteData(0x00); 
  
  LCD_WriteCommand(ST7735_PWCTR4); 
  LCD_WriteData(0x8A);
  LCD_WriteData(0x2A); 
  
  LCD_WriteCommand(ST7735_PWCTR5); 
  LCD_WriteData(0x8A);
  LCD_WriteData(0xEE); 
  
  // VCOM
  LCD_WriteCommand(ST7735_VMCTR1);
  LCD_WriteData(0x0E); 
   
  //MV, MH, MX, RGB mode
  LCD_WriteCommand(ST7735_MADCTL);
  LCD_WriteData(MADCTL_MY | MADCTL_RGB); 

  //ST7735R Gamma Sequence
  LCD_WriteCommand(ST7735_GMCTRP1); 
  LCD_WriteData(0x0f); LCD_WriteData(0x1a); 
  LCD_WriteData(0x0f); LCD_WriteData(0x18); 
  LCD_WriteData(0x2f); LCD_WriteData(0x28); 
  LCD_WriteData(0x20); LCD_WriteData(0x22); 
  LCD_WriteData(0x1f); LCD_WriteData(0x1b); 
  LCD_WriteData(0x23); LCD_WriteData(0x37);
  LCD_WriteData(0x00); 
  LCD_WriteData(0x07); 
  LCD_WriteData(0x02); LCD_WriteData(0x10); 
  
  LCD_WriteCommand(ST7735_GMCTRN1); 
  LCD_WriteData(0x0f); LCD_WriteData(0x1b); 
  LCD_WriteData(0x0f); LCD_WriteData(0x17); 
  LCD_WriteData(0x33); LCD_WriteData(0x2c); 
  LCD_WriteData(0x29); LCD_WriteData(0x2e); 
  LCD_WriteData(0x30); LCD_WriteData(0x30); 
  LCD_WriteData(0x39); LCD_WriteData(0x3f); 
  LCD_WriteData(0x00); LCD_WriteData(0x07); 
  LCD_WriteData(0x03); LCD_WriteData(0x10);  

  LCD_WriteCommand(ST7735_CASET);
  LCD_WriteData(0x00); LCD_WriteData(0x00);
  LCD_WriteData(0x00); LCD_WriteData(0x7f);
  
  LCD_WriteCommand(ST7735_RASET);
  LCD_WriteData(0x00); LCD_WriteData(0x00);
  LCD_WriteData(0x00); LCD_WriteData(0x9f);

  LCD_WriteCommand(0xF0); //Enable test command  
  LCD_WriteData(0x01);
  
  LCD_WriteCommand(0xF6); //Disable ram power save mode 
  LCD_WriteData(0x00); 
   
  LCD_WriteCommand(ST7735_COLMOD); //65k mode 
  LCD_WriteData(0x05);

  //Display on
  LCD_WriteCommand(ST7735_DISPON);

  LCD_Clear();
  LCD_Refresh();
}

void LCD_Clear()
{
  for(uint8_t x = 0; x < TFT_ScreenWidth; ++x)
    for(uint8_t y = 0; y < TFT_ScreenHeight / 8; ++y)
        DisplayBuffer[x][y] = 0;
}

void LCD_Refresh()
{
  LCD_SetAddrWindow(0, 0, TFT_ScreenWidth - 1, TFT_ScreenHeight - 1);

  TFT_START_WRITE_DATA

  uint16_t color;
  for(uint8_t x = 0; x < TFT_ScreenWidth; ++x)
  {
    for(uint8_t y = 0; y < TFT_ScreenHeight / 8; ++y)
    {
      color = COLOR_IDX_TO_RGB[DisplayBuffer[x][y] & 0x01];
      TFT_SPI_WRITE(color >> 8);
      TFT_SPI_WRITE(color);

      color = COLOR_IDX_TO_RGB[(DisplayBuffer[x][y] & 0x02) >> 1];
      TFT_SPI_WRITE(color >> 8);
      TFT_SPI_WRITE(color);

      color = COLOR_IDX_TO_RGB[(DisplayBuffer[x][y] & 0x04) >> 2];
      TFT_SPI_WRITE(color >> 8);
      TFT_SPI_WRITE(color);

      color = COLOR_IDX_TO_RGB[(DisplayBuffer[x][y] & 0x08) >> 3];
      TFT_SPI_WRITE(color >> 8);
      TFT_SPI_WRITE(color);

      color = COLOR_IDX_TO_RGB[(DisplayBuffer[x][y] & 0x10) >> 4];
      TFT_SPI_WRITE(color >> 8);
      TFT_SPI_WRITE(color);

      color = COLOR_IDX_TO_RGB[(DisplayBuffer[x][y] & 0x20) >> 5];
      TFT_SPI_WRITE(color >> 8);
      TFT_SPI_WRITE(color);

      color = COLOR_IDX_TO_RGB[(DisplayBuffer[x][y] & 0x40) >> 6];
      TFT_SPI_WRITE(color >> 8);
      TFT_SPI_WRITE(color);

      color = COLOR_IDX_TO_RGB[(DisplayBuffer[x][y] & 0x80) >> 7];
      TFT_SPI_WRITE(color >> 8);
      TFT_SPI_WRITE(color);
    }
  }

  TFT_STOP_WRITE_DATA
}

void LCD_DrawPixel(const uint8_t x, const uint8_t y, const ColorIdx color)
{
    if (x >= TFT_ScreenWidth || y >= TFT_ScreenHeight)
      return;

    uint8_t i = div8(y); // y / 8;
    uint8_t j = mod8(y); // y % 8;
    DisplayBuffer[x][i] = ((color ? 1 : 0) << j) | (DisplayBuffer[x][i] & ~(1 << j));
}
