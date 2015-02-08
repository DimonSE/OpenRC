#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../../src/def.h"
#include "../../../src/Graphic.h"
#include "../../../src/hardware/Hardware.h"

#include <util/delay.h>

int main()
{
    hwr_InitPorts();

    gfx_Init();

    char str[100];

    while (true)
    {
        gfx_ClearBuf();

        sprintf(str, "PINA = %d", PINA);
        gfx_PrintString(0, 0,  str, COLOR_BLACK, Font_8x8);

        sprintf(str, "PINB = %d", PINB);
        gfx_PrintString(0, 10, str, COLOR_BLACK, Font_8x8);

        sprintf(str, "PINC = %d", PINC);
        gfx_PrintString(0, 20, str, COLOR_BLACK, Font_8x8);

        sprintf(str, "PIND = %d", PIND);
        gfx_PrintString(0, 30, str, COLOR_BLACK, Font_8x8);

        sprintf(str, "PING = %d", PING);
        gfx_PrintString(0, 40, str, COLOR_BLACK, Font_8x8);

        sprintf(str, "PINL = %d", PINL);
        gfx_PrintString(0, 50, str, COLOR_BLACK, Font_8x8);

        gfx_Refresh();
    }
}