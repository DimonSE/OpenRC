#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "../../../src/def.h"
#include "../../../src/Graphic.h"

#include <util/delay.h>

//
// Global variables and declarations
//

enum MenuButtons
{
    B_UP    = 0x01,
    B_DOWN  = 0x02,
    B_LEFT  = 0x04,
    B_RIGHT = 0x08,
    B_BACK  = 0x10,
    B_ENTER = 0x20
};

struct Point
{
    uint8_t x;
    uint8_t y;

    Point() { x = 0; y = 0; }
    Point(uint8_t _x, uint8_t _y) { x = _x; y = _y; }
};

const uint8_t CrosswareSize = 20;

static uint8_t KbdStat = 0;

static Point CrosswarePos(TFT_ScreenWidth  / 2, TFT_ScreenHeight / 2);
static Point Lines[100];
static uint8_t LinesCount = 0;

//
// Function define
//

inline void Keyboard_Driver_Task();

//
// Interrupt
//

ISR (TIMER1_COMPA_vect)
{
    static bool InterruptEnterFlag = false;
    
    if(InterruptEnterFlag)
        return;

    // Иначе устанавливаем флаг, что мы уже находимся в данном прерывании
    InterruptEnterFlag = true;
    
    Keyboard_Driver_Task();

    InterruptEnterFlag = false;
    return;
}

//
// Functions
//

inline void Keyboard_Driver_Task()
{
    unsigned char NewKbdStat = 0;
    static unsigned char OldKbdStat = 0;

    if(HB_UP)    NewKbdStat |= B_UP;
    if(HB_DOWN)  NewKbdStat |= B_DOWN;
    if(HB_LEFT)  NewKbdStat |= B_LEFT;
    if(HB_RIGHT) NewKbdStat |= B_RIGHT;
    if(HB_BACK)  NewKbdStat |= B_BACK;
    if(HB_ENTER) NewKbdStat |= B_ENTER;

    if(NewKbdStat != OldKbdStat)   // Если состояние маски изменилось - запоминаем его
        OldKbdStat = NewKbdStat;
    else if(NewKbdStat != KbdStat) // Если не изменилось, но изменилось относительно результата - копируем в результат.
        KbdStat = NewKbdStat;
}

void Update()
{
  switch (KbdStat)
  {
    case B_UP:    { --CrosswarePos.y; break; }
    case B_DOWN:  { ++CrosswarePos.y; break; }
    case B_LEFT:  { --CrosswarePos.x; break; }
    case B_RIGHT: { ++CrosswarePos.x; break; }
    case B_ENTER:
    {
      Lines[++LinesCount] = CrosswarePos;
      break;
    }
    case B_BACK:
    {
      LinesCount = 0;
      break;
    }
  }
}

void Render()
{
  gfx_ClearBuf();
  for (uint8_t posStart = 0, posEnd = 1; posEnd < LinesCount; ++posStart, ++posEnd)
  {
    gfx_DrawLine(Lines[posStart].x, Lines[posStart].y, Lines[posEnd].x, Lines[posEnd].y, COLOR_FRONT);
  }

  gfx_DrawLine(CrosswarePos.x, CrosswarePos.y, CrosswarePos.x - CrosswareSize, CrosswarePos.y, COLOR_FRONT);
  gfx_DrawLine(CrosswarePos.x, CrosswarePos.y, CrosswarePos.x, CrosswarePos.y - CrosswareSize, COLOR_FRONT);
  gfx_DrawLine(CrosswarePos.x, CrosswarePos.y, CrosswarePos.x + CrosswareSize, CrosswarePos.y, COLOR_FRONT);
  gfx_DrawLine(CrosswarePos.x, CrosswarePos.y, CrosswarePos.x, CrosswarePos.y + CrosswareSize, COLOR_FRONT);

  static char str[100];
  sprintf(str, "Key = %i", KbdStat);
  gfx_PrintString(10, 10, str, COLOR_FRONT, Font_8x8);
}

int main(void)
{
  // PORTA use for menu buttons
  PORTA = 0xFF;
  DDRA  = 0x00;

  // PORTB.0, PORTB.1, PORTB.2 and PORTB.3 use by SPI in TFT display
  PORTB = 0xFF;
  DDRB  = 0x0F;

  // PORTL.0, PORTL.1 and PORTL.2 use as GPIO in TFT display
  PORTL = 0xFF;
  DDRL  = 0x07;

  // Timer/Counter 1 initialization
  
  TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode
  TIMSK1 |= (1 << OCIE1A); // Enable CTC interrupt
  sei(); //  Enable global interrupts
  OCR1A   = 12500; // Set CTC compare value to 20Hz at 16MHz AVR clock, with a prescaler of 64
  TCCR1B |= ((1 << CS10) | (1 << CS11)); // Start timer at Fcpu/64  

  gfx_Init();

  gfx_PrintString(10, 60, "Hello!!!", COLOR_FRONT, Font_12x16);
  gfx_Refresh();

  _delay_ms(2000);

  gfx_ClearBuf();
  while (1)
  {
    Update();
    Render();

    gfx_Refresh();
  }
}

