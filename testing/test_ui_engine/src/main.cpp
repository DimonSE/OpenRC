#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "../../../src/def.h"
#include "../../../src/Graphic.h"
#include "../../../src/UI_Engine.h"
#include "../../../src/Tasks.h"

#include <util/delay.h>

static volatile uint8_t KbdStat = 0;

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
    
    KeyboardDriver_Task();

    InterruptEnterFlag = false;
    return;
}

//
// Keyboard driver
//

void KeyboardDriver_Task()
{
    uint8_t NewKbdStat = 0;
    static uint8_t OldKbdStat = 0;

    if(HB_UP)    NewKbdStat |= B_UP;
    if(HB_DOWN)  NewKbdStat |= B_DOWN;
    if(HB_LEFT)  NewKbdStat |= B_LEFT;
    if(HB_RIGHT) NewKbdStat |= B_RIGHT;
    if(HB_BACK)  NewKbdStat |= B_BACK;
    if(HB_ENTER) NewKbdStat |= B_ENTER;

    if(NewKbdStat != OldKbdStat)
        OldKbdStat = NewKbdStat;
    else
    if(NewKbdStat != KbdStat)
        KbdStat = NewKbdStat;
}

// Delay in msec
void WaitEmptyButtons(uint16_t Delay)
{
    if(Delay == 0)
    {
        while(KbdStat)
            ;
    }
    else
    {
        while(KbdStat)
        {
            _delay_ms(1);
            Delay--;
            if(Delay == 0)
                break;
        };
    }

}

uint8_t WaitButtonPress(uint8_t mask)
{
    while(!(KbdStat & mask))
        ;
    
    return KbdStat;
}

uint8_t AskButtons()
{
    return KbdStat;
}

//
// Public functions
//

void ShowResult(const int8_t result)
{
    gfx_ClearBuf();
    char resultStr[12];
    sprintf(resultStr, "Result=%i", result);
    gfx_PrintString(20, TFT_ScreenHeight / 2, resultStr, COLOR_BLACK, Font_8x12);
    gfx_Refresh();

    _delay_ms(1000);
}

// Print MsgBox
void demo0(int8_t dummy)
{
    MsgBox("Hello World!\nThis is a testing\nstring with \nmultiline\nsupport.", Font_8x12, "Test...", Font_8x8, 0);

    WaitButtonPress(B_ANY);
    WaitEmptyButtons();
}

void demo1(int8_t mode)
{
    static const char* SelectBoxSmall[] = {
            "Test1",
            "Test2",
            "And 3"
        };

    static const char* SelectBoxBig[] = {
            "First ",
            "Second",
            "Third ",
            "4     ",
            "And Fv",
            "----6-",
            "---7--",
            "---8--",
            "---9--",
            "--10--",
            "--11--",
            "--12--",
            "--13--",
            "--14--",
            "--15--",
            "--16--"
        };

    const char**  Strings = mode ? SelectBoxBig : SelectBoxSmall;
    const uint8_t Count   = mode ? ARRAY_SIZE(SelectBoxBig) : ARRAY_SIZE(SelectBoxSmall);

    int8_t result = SelectBox(Strings, Count, 0, Font_8x8, "Test Select", Font_6x8);

    ShowResult(result);
}

void demo2(int8_t dummy)
{
    static int16_t num = 5;
    EditNumDlg(&num, PTR_INT16, -20, 20, 100, "Enter number:");

    ShowResult(num);
}

void demo3(int8_t dummy)
{
    static char Str[11] = "Name 1    ";
    EditStrDlg(Str, 10, Font_8x12, "Enter a name:", Font_6x8);
}

void StartDemoMenu()
{
    CMenu::Item items[] = {
            {"Start MsgBox demo",  demo0, NULL, 0},
            {"Start Select small", demo1, NULL, 0},
            {"Start Select big",   demo1, NULL, 1},
            {"Start Edit num",     demo2, NULL, 0},
            {"Start Edit name",    demo3, NULL, 0}
        };
    
    CMenu menu(CString("Test Menu"), items, ARRAY_SIZE(items), Font_8x8);

    menu.Run();
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

  while (1)
  {
    StartDemoMenu();
  }
}