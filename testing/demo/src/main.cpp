/*******************************************************************************
*  Demo.c
*
********************************************************************************/

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "../../../src/def.h"
#include "../../../src/Graphic.h"
#include "../../../src/UI_Engine.h"
#include "../../../src/Global_Variables.h"

#include <util/delay.h>

//
// Global variable section
//

uint16_t DataTable[] =
{
    0x0A51, 0x0F71, 0x14A1, 0x1881, 0x0A51, 0x0F71, 0x14A1, 0x1881, 0x0A51, 0x0F71, 0x14A1, 0x1881, 0x0A51, 0x0F71, 0x14A1,
    0x1881, 0x0A51, 0x1061, 0x14A1, 0x1B81, 0x0A51, 0x1061, 0x14A1, 0x1B81, 0x0A51, 0x1061, 0x14A1, 0x1B81, 0x0A51, 0x1061,
    0x14A1, 0x1B81, 0x0A51, 0x0DC1, 0x1261, 0x1721, 0x0A51, 0x0DC1, 0x1261, 0x1721, 0x0A51, 0x0DC1, 0x1261, 0x1721, 0x0A51,
    0x0DC1, 0x1261, 0x1721, 0x0A51, 0x0F71, 0x14A1, 0x1881, 0x0A51, 0x0F71, 0x14A1, 0x1881, 0x0A51, 0x0F71, 0x14A1, 0x1881,
    0x0A51, 0x0F71, 0x14A1, 0x1881, 0x0931, 0x1061, 0x1721, 0x1B81, 0x0931, 0x1061, 0x1721, 0x1B81, 0x0931, 0x1061, 0x1721,
    0x1B81, 0x0931, 0x1061, 0x1721, 0x1B81, 0x0C41, 0x0F71, 0x1261, 0x1EE1, 0x0C41, 0x0F71, 0x1261, 0x1EE1, 0x0C41, 0x0F71,
    0x1261, 0x1EE1, 0x0C41, 0x0F71, 0x1261, 0x1EE1, 0x0831, 0x0C41, 0x14A1, 0x1EE1, 0x0831, 0x0C41, 0x14A1, 0x1EE1, 0x0831,
    0x0B91, 0x14A1, 0x1B81, 0x0831, 0x0B91, 0x14A1, 0x1B81, 0x07B1, 0x0C41, 0x14A1, 0x1EE1, 0x07B1, 0x0C41, 0x14A1, 0x1EE1,
    0x07B1, 0x0DC1, 0x1061, 0x1371, 0x07B1, 0x0DC1, 0x0F71, 0x1371, 0x0A51, 0x0C41, 0x0F71, 0x14A1, 0x0A51, 0x0C41, 0x0F71,
    0x14A1, 0x0A51, 0x0F71, 0x14A1, 0x1881, 0x0A51, 0x0F71, 0x14A1, 0x1881, 0x0931, 0x1261, 0x1881, 0x1B81, 0x0931, 0x1261,
    0x1881, 0x1B81, 0x0931, 0x1061, 0x1721, 0x1B81, 0x0931, 0x1061, 0x1721, 0x1B81, 0x0C41, 0x0F71, 0x1261, 0x1881, 0x0C41,
    0x0F71, 0x1261, 0x1881, 0x0C41, 0x1261, 0x1881, 0x1EE1, 0x0C41, 0x1261, 0x1881, 0x1EE1, 0x0B91, 0x1721, 0x1EE1, 0x22A1,
    0x0B91, 0x1721, 0x1EE1, 0x22A1, 0x0B91, 0x14A1, 0x1D21, 0x22A1, 0x0B91, 0x14A1, 0x1D21, 0x22A1, 0x0F71, 0x1261, 0x1721,
    0x24B1, 0x0F71, 0x1261, 0x1721, 0x24B1, 0x0DC1, 0x1EE1, 0x24B1, 0x2E41, 0x0DC1, 0x1EE1, 0x24B1, 0x2E41, 0x0D01, 0x1EE1,
    0x24B1, 0x3DC1, 0x0D01, 0x1EE1, 0x24B1, 0x3DC1, 0x0C41, 0x1721, 0x1EE1, 0x24B1, 0x0C41, 0x15E1, 0x1EE1, 0x24B1, 0x0B91,
    0x1721, 0x1EE1, 0x22A1, 0x0B91, 0x1721, 0x1EE1, 0x22A1, 0x0B91, 0x1EE1, 0x22A1, 0x2E41, 0x0B91, 0x1EE1, 0x22A1, 0x2E41,
    0x0B91, 0x1721, 0x1D21, 0x22A1, 0x0B91, 0x1721, 0x1D21, 0x22A1, 0x0B91, 0x1D21, 0x22A1, 0x2E41, 0x0B91, 0x1D21, 0x22A1,
    0x2E41, 0x0F71, 0x3DC1, 0x2E41, 0x24B1, 0x1EE1, 0x2E41, 0x24B1, 0x1EE1, 0x1721, 0x3DC1, 0x1EE1, 0x1721, 0x1261, 0x1061,
    0x0F71, 0x0DC1, 0x0C41, 0x3DC1, 0x2E41, 0x24B1, 0x1EE1, 0x2E41, 0x24B1, 0x1EE1, 0x1721, 0x24B1, 0x1EE1, 0x1721, 0x1261,
    0x1151, 0x0F71, 0x0C41, 0x0B91, 0x2E41, 0x22A1, 0x1EE1, 0x1721, 0x22A1, 0x1EE1, 0x1721, 0x14A1, 0x1EE1, 0x1D21, 0x1721,
    0x1151, 0x0F71, 0x0E91, 0x0B91, 0x0931, 0x1261, 0x1721, 0x1261, 0x0F71, 0x1EE1, 0x1721, 0x0F71, 0x09C1, 0x1EE1, 0x1721,
    0x1371, 0x0F71, 0x1B81, 0x1721, 0x0F71, 0x14A1, 0x5261, 0x3DC1, 0x3101, 0x2931, 0x3DC1, 0x3101, 0x2931, 0x1EE1, 0x3101,
    0x2931, 0x1EE1, 0x1881, 0x1721, 0x14A1, 0x1261, 0x1061, 0x5261, 0x3DC1, 0x3101, 0x2931, 0x3DC1, 0x3101, 0x2931, 0x1EE1,
    0x3101, 0x2931, 0x1EE1, 0x1881, 0x1721, 0x14A1, 0x1061, 0x0F71, 0x3DC1, 0x2E41, 0x2931, 0x1EE1, 0x2E41, 0x2931, 0x1EE1,
    0x1B81, 0x2931, 0x26E1, 0x1EE1, 0x1721, 0x14A1, 0x1371, 0x0F71, 0x07B1, 0x0F71, 0x1371, 0x1721, 0x1EE1, 0x26E1, 0x2E41,
    0x3DC1, 0x3101, 0x2931, 0x1EE1, 0x1881, 0x1721, 0x1371, 0x0F71, 0x09C1, 0x0A51, 0x0F71, 0x14A1, 0x1881, 0x0A51, 0x0F71,
    0x14A1, 0x1881, 0x0A51, 0x0F71, 0x14A1, 0x1881, 0x0A51, 0x0F71, 0x14A1, 0x1881, 0x0A51, 0x1061, 0x14A1, 0x1B81, 0x0A51,
    0x1061, 0x14A1, 0x1B81, 0x0A51, 0x1061, 0x14A1, 0x1B81, 0x0A51, 0x1061, 0x14A1, 0x1B81, 0x0A51, 0x0DC1, 0x1261, 0x1721,
    0x0A51, 0x0DC1, 0x1261, 0x1721, 0x0A51, 0x0DC1, 0x1261, 0x1721, 0x0A51, 0x0DC1, 0x1261, 0x1721, 0x0A51, 0x0F71, 0x14A1,
    0x1881, 0x0A51, 0x0F71, 0x14A1, 0x1881, 0x0A51, 0x0F71, 0x14A1, 0x1881, 0x0A51, 0x0F71, 0x14A1, 0x1881, 0x0931, 0x1061,
    0x1721, 0x1B81, 0x0931, 0x1061, 0x1721, 0x1B81, 0x0931, 0x1061, 0x1721, 0x1B81, 0x0931, 0x1061, 0x1721, 0x1B81, 0x0C41,
    0x0F71, 0x1261, 0x1EE1, 0x0C41, 0x0F71, 0x1261, 0x1EE1, 0x0C41, 0x0F71, 0x1261, 0x1EE1, 0x0C41, 0x0F71, 0x1261, 0x1EE1,
    0x0831, 0x0C41, 0x14A1, 0x1EE1, 0x0831, 0x0C41, 0x14A1, 0x1EE1, 0x0831, 0x0B91, 0x14A1, 0x1B81, 0x0831, 0x0B91, 0x14A1,
    0x1B81, 0x0831, 0x0F71, 0x14A1, 0x1881, 0x0831, 0x0F71, 0x14A1, 0x1881, 0x0831, 0x0DC1, 0x14A1, 0x1721, 0x0831, 0x0DC1,
    0x14A1, 0x1721, 0x0831, 0x0C41, 0x14A1, 0x1EE1, 0x0831, 0x0C41, 0x14A1, 0x1EE1, 0x0831, 0x0B91, 0x14A1, 0x1B81, 0x0831,
    0x0B91, 0x14A1, 0x1B81, 0x07B1, 0x0F71, 0x14A1, 0x1881, 0x07B1, 0x0F71, 0x14A1, 0x1881, 0x07B1, 0x0DC1, 0x1371, 0x1721,
    0x07B1, 0x0DC1, 0x1371, 0x1721, 0x0A51, 0x5261, 0x3DC1, 0x3101, 0x2931, 0x1EE1, 0x1881, 0x14A1, 0x0F71, 0x3DC1, 0x3101,
    0x2931, 0x1EE1, 0x1881, 0x14A1, 0x0F71, 0x0C41, 0x3101, 0x2931, 0x3101, 0x2E41, 0x3DC1, 0x4DC1, 0x5C81, 0xA4D1, 0x3DC1,
    0x3101, 0x2931, 0x1EE1, 0x1881, 0x14A1, 0x0F71, 0x0A5F, 0x000F
};

// 
// Main demo function
//

void Demo()
{
    Settings.SoundFlag = ON;

    gfx_ClearBuf();

    gfx_PrintString(div2(TFT_ScreenWidth - Font_4x6.W), div2(TFT_ScreenHeight - Font_4x6.H), "1", COLOR_BLACK, Font_4x6);
    gfx_Refresh();
    _delay_ms(500);

    gfx_PrintString(div2(TFT_ScreenWidth - Font_6x8.W), div2(TFT_ScreenHeight - Font_6x8.H), "2", COLOR_BLACK, Font_6x8);
    gfx_Refresh();
    _delay_ms(500);

    gfx_PrintString(div2(TFT_ScreenWidth - Font_8x8.W), div2(TFT_ScreenHeight - Font_8x8.H), "3", COLOR_BLACK, Font_8x8);
    gfx_Refresh();
    _delay_ms(500);

    gfx_PrintString(div2(TFT_ScreenWidth - Font_8x12.W*8), div2(TFT_ScreenHeight - Font_8x12.H), "Lets GO!", COLOR_BLACK, Font_8x12);

    char str[32];

    int16_t U_bat = read_adc(ADC_BAT) * 2;
    
    sprintf(str, "%2i.%02iV", U_bat / 100, U_bat % 100);
    gfx_PrintString(0, TFT_ScreenHeight - Font_8x12.H, str, COLOR_BLACK, Font_8x12);
    
    gfx_Refresh();

    PlaySound(DataTable, ARRAY_SIZE(DataTable));

    while(IsSoundPlayed())
    {
        gfx_PrintString(0, 0,  "This is font 4x6",  COLOR_BLACK, Font_4x6);
        gfx_PrintString(0, 8,  "This is font 6x8",  COLOR_BLACK, Font_6x8);
        gfx_PrintString(0, 16, "This is font 8x8",  COLOR_BLACK, Font_8x8);
        gfx_PrintString(0, 24, "This is font 8x12", COLOR_BLACK, Font_8x12);

        if(Settings.BacklightFlag == ON)
            gfx_PrintString(0, 36, "Backlight ON ", COLOR_BLACK, Font_8x12);
        else
            gfx_PrintString(0, 36, "Backlight OFF", COLOR_BLACK, Font_8x12);

        U_bat = read_adc(ADC_BAT) * 2;
        
        sprintf(str, "%2i.%02iV", U_bat / 100, U_bat % 100);
        gfx_PrintString(0, TFT_ScreenHeight - Font_8x12.H, str, COLOR_BLACK, Font_8x12);

        gfx_Refresh();

        if(HB_UP || HB_DOWN || HB_LEFT || HB_RIGHT || HB_ENTER)
            break;
    }

    StopSound();

    gfx_ClearBuf();
    
    Tone(1000);
    delay_ms(1000);
    Tone(0);
}

void Demo_AP(uint8_t dummy)
{
    Demo();
}

void IO_Test(void)
{
    asm("cli")

    gfx_ClearBuf();
    gfx_PrintString(div2(TFT_ScreenWidth - 9*Font_8x12.W), div2(TFT_ScreenHeight - 1*Font_8x12.H), "Test Mode", COLOR_BLACK, Font_8x12);
    gfx_Refresh();
    delay_ms(500);

    while(!HB_ENTER)
    {
        gfx_ClearBuf();

        const FontProfile& Font = Font_6x8;

        char str[32];
        uint8_t x = 0, y = 0;
        for(uint8_t i = 0; i < MAX_ADC; ++i)
        {
            x = (i < 4) ? 0 : TFT_ScreenWidth / 2;
            y = Font.H * (i % 4);

            sprintf(str, "AD%d: %4d", i, read_adc(i));

            gfx_PrintString(x, y, str, COLOR_BLACK, Font);
        }

        x = 0;
        y = Font.H * 6;
        gfx_PrintString(x, y, "D: ", COLOR_BLACK, Font);
        x = 3 * Font.W;
        for(uint8_t i = 7; i >= 0; --i)
        {
            const bool bit = PIND & (1 << i);
            gfx_PrintChar(x++, y, bit ? '1' : '0', COLOR_BLACK, Font);
        }

        x = 0;
        y = Font.H * 7;
        gfx_PrintString(x, y, "E: ", COLOR_BLACK, Font);
        x = 3 * Font.W;
        for(uint8_t i = 7, x = 0; i >= 0; --i, ++x)
        {
            const bool bit = PINE & (1 << i);
            gfx_PrintChar(x++, y, bit ? '1' : '0', COLOR_BLACK, Font);
        }

        //DEBUG TODO FOR New hardware configuration
        /*
        x = 0;
        y = Font.H * 5;
        gfx_PrintString(x, y, "A: ", COLOR_BLACK, Font);
        x = 3 * Font.W;
        gfx_PrintChar(x++, y, '2', PINA.2 ? 0xFF : 0, &Font);
        gfx_PrintChar('1', PINA.1 ? 0xFF : 0, &Font);

        gfx_SetXY(Font.W*6, Font.H * 5);
        gfx_PutFlashStr("B: ", 0xFF, &Font);
        for(i=3; i >= 0; i--)
        {
            gfx_PrintChar('0' + i, (PINB & (1 << i)) ? 0xFF : 0, &Font);
        }

        gfx_SetXY(Font.W*13, Font.H * 5);
        gfx_PutFlashStr("G: ", 0xFF, &Font);
        gfx_PrintChar('1', PING&2 ? 0xFF : 0, &Font);
        gfx_PrintChar('0', PING&1 ? 0xFF : 0, &Font);
        */

        gfx_Refresh();
    }

    while(!HB_ENTER);
    _delay_ms(100);
    while(HB_ENTER);

    asm("sei")
}

void IO_Test_AP(uint8_t dummy)
{
    IO_Test();
}


//
// Chron
//

volatile uint8_t ovf;
volatile uint8_t dir;
volatile uint8_t ovf;


ISR(TIMER1_OVF_vect)
{
    ovf = 1;
}


void Chron()
{
    uint8_t flag    = 1;
    bool    refresh = true;

    uint16_t t1[2] = {0, 0};
    uint16_t t2[2] = {0, 0};

    asm("cli")

    // Запуск таймера
    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: 2000,000 kHz
    // Mode: Normal top=FFFFh
    // OC1A output: Discon.
    // OC1B output: Discon.
    // OC1C output: Discon.
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer 1 Overflow Interrupt: On
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: Off
    // Compare B Match Interrupt: Off
    // Compare C Match Interrupt: Off
    TCCR1A = 0x00;
    TCCR1B = 0x02;
    TCNT1H = 0x00;
    TCNT1L = 0x00;
    ICR1H  = 0x00;
    ICR1L  = 0x00;
    OCR1AH = 0x00;
    OCR1AL = 0x00;
    OCR1BH = 0x00;
    OCR1BL = 0x00;
    OCR1CH = 0x00;
    OCR1CL = 0x00;
    
    // Отключение USART0
    UCSR0A = 0x00;
    UCSR0B = 0x00;
    UCSR0C = 0x00;
    UBRR0H = 0x00;
    UBRR0L = 0x00;

    // Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK = 0x04;
    ETIMSK = 0x00;

    asm("sei")

    while(!HB_BACK)
    {
        if(HB_ENTER)
        {
            flag    = 1;
            t1[0]   = 0;
            t1[1]   = 0;
            t2[0]   = 0;
            t2[1]   = 0;
            refresh = true;
        }

        switch(flag)
        {
            case(1):
                if(dir ? !PINE.1 : !PINE.0)
                {
                    t1[0] = 0;
                    TCNT1 = 0;
                    ovf   = 0;
                    flag  = 2;
                }
                break;

            case(2):
                if(dir ? PINE.1 : PINE.0)
                {
                    t1[1] = TCNT1;
                    flag  = 3;
                }
                break;

            case(3):
//                if(dir ? !PINE.1 : !PINE.0) flag = 2; // TEST for checking speed
                if(dir ? !PINE.0 : !PINE.1)
                {
                    t2[0] = TCNT1;
                    flag  = 4;
                }
                break;

            case(4):
                if(dir ? PINE.0 : PINE.1)
                {
                    t2[1] = TCNT1;
                    flag  = 5;
//                    refresh = 1;
                }
                break;

            case(5):
//                if(dir ? !PINE.0 : !PINE.1) flag = 4; // TEST for checking speed
                if(ovf)
                {
                    flag    = 0;
                    refresh = true;
                }
                break;

            default:

                break;
        }

        if(refresh)
        {
            char    str[32];
            uint8_t x = 0;
            uint8_t y = 0;

            const FontProfile& Font = Font_6x8;

            gfx_ClearBuf();

            sprintf(str, "T1S: %5u", t1[0]);
            gfx_PrintString(x, y, str, COLOR_BLACK, Font);

            x = TFT_ScreenWidth / 2;
            y = 0;
            sprintf(str, "T1E: %5u", t1[1]);
            gfx_PrintString(x, y, str, COLOR_BLACK, Font);

            x = 0;
            y = Font.H * 1;
            sprintf(str, "T2S: %5u", t2[0]);
            gfx_PrintString(x, y, str, COLOR_BLACK, Font);

            x = TFT_ScreenWidth / 2;
            y = Font.H * 1;
            sprintf(str, "T2E: %5u", t2[1]);
            gfx_PrintString(x, y, str, COLOR_BLACK, Font);

            x = 0;
            y = Font.H * 2;
            sprintf(str, "Speed 1: %3u m/s", 140000l / t2[0]);
            gfx_PrintString(x, y, str, COLOR_BLACK, Font);

            x = 0;
            y = Font.H * 3;
            sprintf(str, "Speed 2: %3u m/s", 140000l / (t2[1] - t1[1]));
            gfx_PrintString(x, y, str, COLOR_BLACK, Font);

            uint32_t l = ((140000l * t1[1]) / t2[0]) / 200;
            x = 0;
            y = Font.H * 4;
            sprintf(str, "L1S1: %3u.%1u mm", l/10, l%10);
            gfx_PrintString(str, COLOR_BLACK, Font);

            l = ((140000l * t1[1]) / (t2[1] - t1[1])) / 200;
            x = TFT_ScreenWidth / 2;
            y = Font.H * 4;
            sprintf(str, "L1S2: %3u.%1u mm", l/10, l%10);
            gfx_PrintString(x, y, str, COLOR_BLACK, Font);

            l = ((140000l * (t2[1] - t2[0])) / t2[0]) / 200;
            x = 0;
            y = Font.H * 5;
            sprintf(str, "L2S1: %3u.%1u mm", l/10, l%10);
            gfx_PrintString(x, y, str, COLOR_BLACK, Font);

            l = ((140000l * (t2[1] - t2[0])) / (t2[1] - t1[1])) / 200;
            x = TFT_ScreenWidth / 2;
            y = Font.H * 5;
            sprintf(str, "L2S2: %3u.%1u mm", l/10, l%10);
            gfx_PrintString(x, y, str, COLOR_BLACK, Font);

            gfx_Refresh();
            refresh = false;
        }
    }

    // Переинициализация железа
    hwl_InitUSART0(BAUD_RATE); // USART0 initialization
    hwl_InitTimers();          // Timers/Counters initialization
    TIMSK  = 0x10;             // Разрешение выработки прерываний таймером выходных импульсов
    ETIMSK = 0x10;             // Разрешение выработки прерываний таймером задач
}

// ***   Получение состояния переменной   **************************************
char* ChronDirGetStr(char* Buf, char AddParam)
{
    sprintf(Buf, dir ? "REV" : "NOR");
    return Buf;
}

void ChronDirSet(char AddParam)
{
    static const char* Strings[] = { "NOR", "REV" };

    int8_t result = SelectBox(Strings, ARRAY_SIZE(Strings), dir, Font_8x8, "Direction:", Font_8x8);

    if(result != RESULT_INCORRECT)
        dir = result;
}

void Chron_AP(char AddParam)
{
    Chron();
}

void ChronMenu(uint8_t dummy)
{
    CMenu::Item items[] = {
            { "Start",     Chron_AP,    NULL           },
            { "Direction", ChronDirSet, ChronDirGetStr }
        };
    
    CMenu menu(CString("Chron"), items, ARRAY_SIZE(items), Font_8x8);

    menu.Run();
}

//
// Main
//

void MainMenu(void)
{
    CMenu::Item items[] =
    {
        {"IO Test", IO_Test_AP, NULL, 0},
        {"Demo",    Demo_AP,    NULL, 0},
        {"Chron",   ChronMenu,  NULL, 0}
    };
    CMenu menu(CString("Main Menu"), items, ARRAY_SIZE(items), Font_8x8);
    
    menu.Run();
}