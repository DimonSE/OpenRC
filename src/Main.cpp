/*******************************************************************************
*  Coder.cpp
*
*  Chip type           : ATmega2560
*  Program type        : Application
*  Clock frequency     : 16,000000 MHz
*  Memory model        : Small
*  External SRAM size  : 0
*  Data Stack size     : 1024
*******************************************************************************/

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "def.h"
#include "Graphic.h"
#include "UI_Engine.h"
#include "User_Interface.h"
#include "Tasks.h"
#include "Math.h"
#include "hardware/Hardware.h"
#include "System.h"
#include "Global_Variables.h"

#include <util/delay.h>

//
// Interrupts
//

// Прерывание таймера в котором собственно говоря и идет вычисление импульсов
ISR(TIMER1_COMPA_vect)
{
    static uint8_t  nb  = 0; // Номер текущего канала
    static uint16_t del = 0; // Длительность паузы между пачками PPM импульсов

    if(nb < CurModel.num_ch)
    {
        math_CalcChannel(nb); // Рассчет канального импульса - микширование

        ICR1 = output[nb]; // Заливаем длительность канала
        del += output[nb]; // Добавляем длительность канала к суммарному значению канальных импульсов

        ++nb; // Увеличиваем номер канала
    }
    else
    {
        del = TimerClockPerUSec(TASK_TICK_TIME * USEC_IN_MSEC) - del; // Длительность паузы: количество отсчетов за 20 мсек - время канальных импульсов

        ICR1 = del; // Заливаем длительность паузы
        del  = 0;   // Обнуляем суммарное значение канальных импульсов
        nb   = 0;   // Сбрасываем номер канала

        math_CalcControls(); // Получение статуса управляющих элементов
    }
}

// *****************************************************************************
// ***   Прерывание таймера                                                  ***
// ***   Данное прерывание необходимо для реализации второстепенных задач,   ***
// ***   таких как опрос клавиатуры, пищание при нажатии на кнопки и т.д.    ***
// ***   Предусматривается не возможность повторного входа в прерывание.     ***
// ***   //DEBUG TODO: Возможно стоит переделать в функцию и вызвать в конце ***
// ***   timer1_compa_isr с помощью строчки:                                 ***
// ***   if(nb == 0) timer3_compa_isr();                                     ***
// *****************************************************************************
ISR(TIMER3_COMPA_vect)
{
    static uint8_t MissedIntCount     = 0;
    static bool    InterruptEnterFlag = false;
    
    TickCount++; // Увеличиваем счетчик системного времени в TASK_TICK_TIME или в мс(DEBUG) интервалах

    if(InterruptEnterFlag)
    {
        MissedIntCount++;
        return;
    }

    InterruptEnterFlag = true;
    
    // Копируем количество пропущенных прерываний в дополнительную переменную, т.к. счетчик будет увеличиватся при повторном вызове данного прерывания.
    uint8_t MissedInterruptsCount = MissedIntCount;
    
    MissedIntCount = 0;

    KeyboardDriver_Task();
    Timer_Task(MissedInterruptsCount);
    BatteryDriver_Task();
    TrimmersSave_Task();

    InterruptEnterFlag = false;
}

//
// Public functions
//

int main()
{
    hwr_InitPorts();
    hwr_InitTimers();
    hwr_InitADC();
    
    gfx_Init();

    if(TX_IsFirstStart())
    {
        MsgBox("Transmiter\nis first started.\n Need init.", Font_8x8, "ERROR");
        while(HB_ENTER || HB_BACK);   // Ждем отпускания кнопок, если были нажаты
        while(!HB_ENTER && !HB_BACK); // Ждем нажатия кнопок

        TX_Reset();
    }
        
    if(!Is_EEPROM_CRC_Correct())
    {
        MsgBox("EEPROM CRC ERROR\nReset to factory\ndefaults ?", Font_8x8, "ERROR");

        while(HB_ENTER || HB_BACK);   // Ждем отпускания кнопок, если были нажаты
        while(!HB_ENTER && !HB_BACK); // Ждем нажатия кнопок
        
        if(HB_ENTER)
            TX_Reset();
    }
        
    TX_Init(); // Инициализируем кодер (чтение установок из EEPROM)

    sei();

    uint32_t LastRefreshTime = 0;
    uint8_t  Kbd = 0;

    while(true)
    {
        MainScreen();
        
        Kbd = AskButtons();

        // Останов счета и переинициализация таймера кнопкой ВЛЕВО
        if(Kbd & B_LEFT)
            Timer_Init();

        // Запуск таймера кнопкой ВВЕРХ
        if(Kbd & B_UP)
            Timer_Stop();

        // Останов таймера кнопкой ВНИЗ
        if(Kbd & B_DOWN)
            Timer_Start();

        // Вход в главное меню
        if(Kbd == B_ENTER)
            MainMenu();
    }
}