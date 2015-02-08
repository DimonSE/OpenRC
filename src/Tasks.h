/*******************************************************************************
*  Tasks.h
*
*******************************************************************************/

#ifndef __Tasks_h__
#define __Tasks_h__

//
// Structs definitions
//

enum Keyboard
{
    B_UP    = 0x01,
    B_DOWN  = 0x02,
    B_LEFT  = 0x04,
    B_RIGHT = 0x08,
    B_BACK  = 0x10,
    B_ENTER = 0x20,

    B_ANY   = 0xFF
};

//
// Public functions
//

void SetTrimmersSaveTime(uint16_t Time);
void TrimmersSave_Task();


void KeyboardDriver_Task();

void    WaitEmptyButtons(uint16_t Delay = 0);
uint8_t WaitButtonPress(uint8_t mask = B_ANY);
uint8_t AskButtons();


void Timer_Task(uint8_t MissedInterruptsCount);

int16_t GetTimerValue();
int16_t GetTimerSubValue();

void Timer_Init();

void Timer_Start();
void Timer_Stop();

bool IsTimerSound();
void IsTimerSoundOff();


void BatteryDriver_Task();
int16_t GetBatteryStatus();

#endif // __Tasks_h__