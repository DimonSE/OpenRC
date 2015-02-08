/*******************************************************************************
*  Sound.cpp
*
*******************************************************************************/

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>

#include "def.h"
#include "Global_Variables.h"
#include "Sound.h"
#include "hardware/Hardware.h"

#include <util/delay.h>

// Включение Buzzer'а с частотой Freq
void Tone(uint16_t Freq)
{
    if(Settings.SoundFlag == OFF)
        return;
    
    hwr_SetTone(Freq);
}

// Включение Buzzer'а при изменении триммеров
void trim_sound()
{
    if(trim_sound_en && (Settings.SoundFlag == ON))
    {
        trim_sound_en = 0;
        Tone(1000);
        _delay_ms(300);
        Tone(0);
        _delay_ms(300);
    }
}
