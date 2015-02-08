/*******************************************************************************
*  Math.cpp
*
*******************************************************************************/

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>

#include "def.h"
#include "hardware/Hardware.h"
#include "Global_Variables.h"
#include "Tasks.h"

//
// Global variables
//

static int16_t AD_NORM[MAX_ADC];      // Нормализованные данные с АЦП
static int16_t input  [MAX_CONTROLS]; // Входные нормализованные каналы
static int16_t out_cur[MAX_CONTROLS]; // Входные каналы после обработки(применения кривых, расходов, реверса и т.п.)

//
// Public functions
//

// Рассчет времени канальных импульсов для вывода (Микширование)
void math_CalcChannel(int8_t CH)
{
    if(CH < 0 || CH > MAX_CHANNELS)
        return;

    output[CH] = 0;

    // Формирование выходного канала
    for(uint8_t i = 0; i < MAX_CONTROLS; ++i)
    {
        // Если влияние управляющего элемента i на канал CH отсутстует - идем дальше
        if(CurModel.Mode[FLY_MODE].Chanels[CH][i] != 0)
        {
            // Получаем влияние управляющего элемента i на канал CH
            const int32_t Factor = CurModel.Mode[FLY_MODE].Chanels[CH][i];
            // Добавляем управляющий элемент i к каналу CH
            output[CH] += out_cur[i] * Factor / 100;
        }
    }

    // Длительность импульса: значение канала(+-0,5 мс) + центральное положение(1,5 мсек)

    output[CH] += OUTPUT_MID;

    if(output[CH] < OUTPUT_MIN)
        output[CH] = OUTPUT_MIN;

    if(output[CH] > OUTPUT_MAX)
        output[CH] = OUTPUT_MAX;
}

// Расширенный рассчет кривой
int16_t math_InterPolEx(int16_t Val, int16_t Max, int16_t K, int8_t* Nodes, int8_t NodesCount)
{
    if(Nodes == NULL)
        return Val;
    
    if(NodesCount <= 0)
        return Val;

    if(Val < 0)
        Val = 0;

    if(Val >= Max)
        Val = Max - 1;

    int32_t InterPoolSize = Max / (NodesCount - 1); // Вычисляем размер интервала - максимальное значение / количество точек кривой

    uint8_t n  = (Val * (NodesCount - 1)) / Max; // Вычисляем интервал в который попали:  значение умножаем на количество интервалов и делим на максимальное значение
    int16_t n1 = (*(Nodes + n)     * K) / 10;    // Вычисляем значение одной границы интервала
    int16_t n2 = (*(Nodes + n + 1) * K) / 10;    // Вычисляем значение другой границы интервала
    
    // Вычисляем смещение: значение относительно начала интервала умножается на величину интервала(Y) и делится на величину интервала(X)
    int16_t ln = (Val - n * InterPoolSize) * (n2 - n1) / InterPoolSize;

    return(n1 + ln);
}

// Рассчет кривой
static inline int16_t math_CurveInterPol(int16_t Val, int8_t* Nodes)
{
    // Добавляем 0.5 мс - сдвигаем в положительную область
    Val += TimerClockPerUSec(500);
    // Выполняем рассчет кривой с использованием универсальной функции
    return math_InterPolEx(Val, TimerClockPerUSec(1000), TimerClockPerUSec(500) / 10, Nodes, CURVE_NODES);
}

// Общий рассчет кривой
inline int16_t math_InterPol(int16_t Val, int16_t Max, int8_t* Nodes, int8_t NodesCount)
{
    return math_InterPolEx(Val, Max, 10, Nodes, NodesCount);
}

// Изменение триммеров
void math_ChangeTrimmer(int8_t* trim, int8_t dir)
{
    if(dir == 0)
        return;
    
    if((dir > 0) && (*trim + dir >  96))
        return;
    
    if((dir < 0) && (*trim + dir < -96))
        return;
    
    *trim += dir;
    SetTrimmersSaveTime(1500);
    trim_sound_en = 1;
}

// Опрос устройств ввода и рассчет их значений
void math_CalcControls()
{
    static uint8_t TrimMask = 0;

    //опрос переключателя режимов
    if(MODE_KEY2)
        FLY_MODE = 1;
    if(!MODE_KEY1 && !MODE_KEY2)
        FLY_MODE = 0;
    if(MODE_KEY1)
        FLY_MODE = 2;

    uint8_t NewTrimMask = 0;

    // Загоняем все нажатые триммера в битовую маску
    if(trim_0up)   NewTrimMask |= 0x01;
    if(trim_0down) NewTrimMask |= 0x02;
    if(trim_1up)   NewTrimMask |= 0x04;
    if(trim_1down) NewTrimMask |= 0x08;
    if(trim_2up)   NewTrimMask |= 0x10;
    if(trim_2down) NewTrimMask |= 0x20;
    if(trim_3up)   NewTrimMask |= 0x40;
    if(trim_3down) NewTrimMask |= 0x80;
    
    if(NewTrimMask != TrimMask)
    {
        TrimMask = NewTrimMask;
    }
    else
    if(TrimMask != 0) // Если присутствуют нажатые триммера - обрабатываем
    {
        for(uint8_t i = 0; i < 8; ++i)
            if(TrimMask & (1 << i))
                math_ChangeTrimmer(&CurModel.Mode[FLY_MODE].trimmers[i/2], (i & 0x01) ? -1 : 1);
    }

    //опрос кнопки выключения двигателя
    if(Tcut_KEY)
        Cut_enable = 1;
    else
        Cut_enable = 0;

    // Чтение АЦП и нормализация полученных данных
    for(uint8_t i = 0; i < MAX_ADC; ++i)
    {
        AD[i] = read_adc(i);
        //DEBUG TODO: Если ADmid unsigned возникает проблема в вычислениях при отритцательных числах. Не понятно почему так происходит...
        if(AD[i] > Settings.ADmid[i])
            AD_NORM[i] = ((AD[i] - Settings.ADmid[i]) * Settings.Kmax[i]) / 10;
        else
            AD_NORM[i] = ((AD[i] - Settings.ADmid[i]) * Settings.Kmin[i]) / 10;
    }

    // Копирование нормализованных данных с АЦП в управляющие элементы  для дальнейшей обработки
    input[CTRL_AIL]  = AD_NORM[ADC_AIL];
    input[CTRL_ELE]  = AD_NORM[ADC_ELE];
    input[CTRL_THR]  = AD_NORM[ADC_THR];
    input[CTRL_RUD]  = AD_NORM[ADC_RUD];
    input[CTRL_AUX1] = AD_NORM[ADC_AUX1];

#ifdef SW1
    // Опрос и установка значения переключателя SW1 (двухрозиционный)
    if(SW1)
        input[CTRL_SW1] = (int16_t)TimerClockPerUSec(500); // +0.5 мс
    else
        input[CTRL_SW1] = (int16_t)-TimerClockPerUSec(500); // -0.5 мс
#else
    // Опрос и установка значения переключателя SW1 (трехрозиционный)
    if(SW1_1)
        input[CTRL_SW1] = (int16_t)TimerClockPerUSec(500); // +0.5 мс
    if(!SW1_1 && !SW1_2)
        input[CTRL_SW1] = 0; // 0 мс
    if(SW1_2)
        input[CTRL_SW1] = (int16_t)-TimerClockPerUSec(500); // -0.5 мс
#endif

    // Опрос и установка значения переключателя SW2 (двухрозиционный)
    if(SW2)
        input[CTRL_SW2] = (int16_t)TimerClockPerUSec(500); // +0.5 мс
    else
        input[CTRL_SW2] = (int16_t)-TimerClockPerUSec(500); // -0.5 мс

    // Опрос и установка значения переключателя SW3 (двухрозиционный)
    if(SW3)
        input[CTRL_SW3] = (int16_t)TimerClockPerUSec(500); // +0.5 мс
    else
        input[CTRL_SW3] = (int16_t)-TimerClockPerUSec(500); // -0.5 мс

    // Копирование виртуальных каналов из реальных
    input[CTRL_V1] = input[CurModel.Mode[FLY_MODE].Control[CTRL_V1].from];
    input[CTRL_V2] = input[CurModel.Mode[FLY_MODE].Control[CTRL_V2].from];

    // Это значение триммера задаваемого в меню. Всегда равно 0.5 мс. В результате влияние зависит только от заданного коэффициента.
    input[CTRL_TRIM] = TimerClockPerUSec(500);

    // Опрос двойных расходов
    bool Dual[MAX_CONTROLS] = {false};
    Dual[CTRL_AIL] = DUAL_AIL;
    Dual[CTRL_ELE] = DUAL_ELE;
    Dual[CTRL_RUD] = DUAL_RUD;

    for(uint8_t i = 0; i < MAX_CONTROLS; ++i)
    {
        // Для этих каналов нет кривых - мы их просто копируем на выход.
        if((i == CTRL_SW1) ||
           (i == CTRL_SW2) ||
           (i == CTRL_SW3) ||
           (i == CTRL_TRIM))
        {
            out_cur[i] = input[i];
        }
        else
        {
            // Рассчет значения с использованием кривой
            out_cur[i] = math_CurveInterPol(input[i], CurModel.Mode[FLY_MODE].Control[i].nodes);
        }

        // Расходы
        int32_t Factor;
        if(Dual[i])
        {
            if(input[i] < 0)
                Factor = CurModel.Mode[FLY_MODE].Control[i].minDRates;
            else
                Factor = CurModel.Mode[FLY_MODE].Control[i].maxDRates;
        }
        else
        {
            if(input[i] < 0)
                Factor = CurModel.Mode[FLY_MODE].Control[i].minRates;
            else
                Factor = CurModel.Mode[FLY_MODE].Control[i].maxRates;
        }
        if(Factor != 100)
            out_cur[i] = out_cur[i] * Factor / 100;

        // Если включен T.Cut
        if((i == CTRL_THR) && Cut_enable)
        {
            Factor = CurModel.Mode[FLY_MODE].Control[i].minDRates; // minDRates в канале газа - положение при T.Cut
            out_cur[i] = (- TimerClockPerUSec(600)) * Factor / 100;
        }

        // Прибавляем триммера к основным управляющим каналам
        //DEBOG TODO: доделать триммеры через enum
        if(i < MAX_TRIMMERS)
        {
            const int16_t TrimFactor = CurModel.Mode[FLY_MODE].trimmers[i];
            out_cur[i] += TrimFactor * 2; //DEBUG TODO: нужно вычисляемое число, зависящее от частоты
        }

        // Применение реверса канала
        out_cur[i] *= CurModel.Mode[FLY_MODE].Control[i].reverse;
    }
}
