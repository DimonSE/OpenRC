/*******************************************************************************
*  Sound.h
*
*  Радиоуправление: Работа со звуком, заголовок
*******************************************************************************/

#ifndef __Sound_h__
#define __Sound_h__

// Включение Buzzer'а с частотой Freq
void Tone(uint16_t Freq);

// Включение Buzzer'а на Delta милисекунд, с частотой Freq 
#define Beep(Freq, Delta)         \
{                                 \
    if(Settings.SoundFlag == ON)  \
    {                             \
        Tone(Freq);               \
        _delay_ms(Delta);         \
        Tone(0);                  \
    }                             \
}                                 \

// Включение Buzzer'а при изменении триммеров
void trim_sound();

#endif // __Sound_h__