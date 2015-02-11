/*******************************************************************************
*  Modulation.cpp
*
*******************************************************************************/

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "Math.h"
#include "Global_Variables.h"
#include "hardware/Hardware.h"
#include "Modulation.h"

//
// Config
//

const char* ModulationTypeName[MODULATION_TYPE_MAX] = 
{
  "PPM",     // MODULATION_TYPE_PPM
  "Inv PPM", // MODULATION_TYPE_INVERSE_PPM
  "PCM"      // MODULATION_TYPE_PCM
};

//
// Interrupts
//

// Interrupt for generate signal on radio module
ISR(TIMER1_COMPA_vect)
{
    switch (CurModel.modulation)
    {
        case MODULATION_TYPE_PPM:
        case MODULATION_TYPE_INVERSE_PPM:
        {
            static uint8_t  cur_ch = 0;
            static uint16_t delay  = 0;

            if(cur_ch < CurModel.num_ch)
            {
                math_CalcChannel(cur_ch); // update current channel value

                ICR1   = output[cur_ch]; // Заливаем длительность канала
                delay += output[cur_ch]; // Добавляем длительность канала к суммарному значению канальных импульсов

                ++cur_ch; // Увеличиваем номер канала
            }
            else
            {
                delay = TimerClockPerUSec(TASK_TICK_TIME * USEC_IN_MSEC) - delay; // Длительность паузы: количество отсчетов за 20 мсек - время канальных импульсов

                ICR1 = delay; // Заливаем длительность паузы
                delay = 0;    // Обнуляем суммарное значение канальных импульсов
                cur_ch = 0;   // Сбрасываем номер канала

                math_CalcControls(); // Получение статуса управляющих элементов
            }

            break;
        }

        case MODULATION_TYPE_PCM:
            //TODO: Release this
            break;
    }
}

void UseModulation(const ModulationType _modulation)
{
    CurModel.modulation = _modulation;

    hwr_SetModulation(CurModel.modulation);
}

const char* GetCurrentModulationName()
{
    return ModulationTypeName[CurModel.modulation];
}