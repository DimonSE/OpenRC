/*******************************************************************************
*  Tasks.cpp
*
*******************************************************************************/
#include <avr/io.h>
#include <stdio.h>

#include "def.h"
#include "Global_Variables.h"
#include "Sound.h"
#include "System.h"
#include "Tasks.h"

#include <util/delay.h>

//
// Private global variables
//

static bool     NeedSaveTrimmers = false;
static uint32_t TimeToTrimmersSave;

static volatile uint8_t KbdStat = 0;

static int16_t TimerValue;
static int16_t TimerSubValue;
static bool    IsCountingFlag;
static bool    bIsTimerSound;

static int16_t U_Bat = 0;


//
// Private functions
//

uint32_t GetRunningTime()
{
    return TickCount * TASK_TICK_TIME;
}

//
// Public functions
//


// Time in msec
void SetTrimmersSaveTime(uint16_t Time)
{
    NeedSaveTrimmers = true;
    TimeToTrimmersSave = GetRunningTime() + Time;
}

void TrimmersSave_Task()
{   
    if(!NeedSaveTrimmers || TimeToTrimmersSave > GetRunningTime())
        return;

    NeedSaveTrimmers = false;
    
    MODEL_Save_Trimmers();
}

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

void Timer_Task(uint8_t MissedInterruptsCount)
{
    const uint8_t TICKS_IN_SEC = 1000 / TASK_TICK_TIME;

    // Нужны для пищания при достижении таймером значения
    static uint8_t counter;
    static uint8_t precounter;

    // Подсчет значения таймера
    if(IsCountingFlag == ON)
    {
        // Увеличиваем временную переменную: количество пропущенных прерываний + текущее
        TimerSubValue += int16_t(MissedInterruptsCount) + 1;

        // Если временная переменная больше 50(количество прерываний в секунду) - прошла 1 сек
        if(TimerSubValue >= TICKS_IN_SEC)
        {
            TimerSubValue -= TICKS_IN_SEC; // Отнимаем 50(количество прерываний в секунду) - 1 сек

            // Изменяем значение таймера, в зависимости от текущего режима
            if(CurModel.timer_mode & 0x02)
                TimerValue--; // Timer Mode DOWN
            else
                TimerValue++; // Timer Mode UP
        }

        // Если задан подсчет ВВЕРХ, значение таймера равно заданному, включен подсчет и заданное значение не равно 0
        if((CurModel.timer_mode & 0x01) && (TimerValue == CurModel.timer) && (IsCountingFlag == ON) && (TimerValue != 0))
        {
            // Включаем писк таймера
            bIsTimerSound = ON;

            // Эти переменные нужны для писка таймера
            counter = 0;
            precounter = 0;
        }

        // Если задан подсчет ВНИЗ, значение таймера равно 0 и включен подсчет
        if((CurModel.timer_mode & 0x02) && (TimerValue == 0) && (IsCountingFlag == ON))
        {
            // Останавливаем подсчет таймера
            IsCountingFlag = OFF;
            // Сбрасываем значение таймера на первоначальное
            TimerValue = CurModel.timer;
            TimerSubValue = 0;

            // Включаем писк таймера
            bIsTimerSound = ON;

            // Эти переменные нужны для писка таймера
            counter = 0;
            precounter = 0;
        }
    }

    // Писк таймера
    if(bIsTimerSound == ON)
    {
        if(precounter == 20 || precounter == 0)
        {
            precounter = 0;
            counter++;
            if(counter % 2 == 0)
                Tone(1000); // Включение пищалки
            else
                Tone(0);    // Выключение пищалки
        }
        if(counter == 25)
            bIsTimerSound = OFF;
        precounter++;
    }
}

int16_t GetTimerValue()
{
    return TimerValue;
}
int16_t GetTimerSubValue()
{
    return TimerSubValue * TASK_TICK_TIME;
}

void Timer_Init()
{
    // Устанавливаем начальное значение таймера в зависимости от режима
    if(CurModel.timer_mode & 0x02)
        TimerValue = CurModel.timer; // Timer Mode DOWN
    else
        TimerValue = 0;              // Timer Mode UP
    
    TimerSubValue = 0;

    IsCountingFlag = OFF;
    bIsTimerSound  = OFF;
}

void Timer_Start()
{
    IsCountingFlag = ON;
}

void Timer_Stop()
{
    IsCountingFlag = OFF;
}

bool IsTimerSound()
{
    return bIsTimerSound;
}

void IsTimerSoundOff()
{
    bIsTimerSound = OFF;
}

void BatteryDriver_Task()
{
    U_Bat = AD[ADC_BAT] * U_BAT_K;
}

int16_t GetBatteryStatus()
{
    return U_Bat;
}