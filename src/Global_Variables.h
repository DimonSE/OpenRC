/*******************************************************************************
*  Global_Variables.c
*
*******************************************************************************/

#ifndef __Global_Variables_h__
#define __Global_Variables_h__

#include "def.h"

// Main parametrs structure for EEPROM
struct TRANSMITTER_SETTINGS
{
    int16_t size;               // Size of structure. Must be sizeof(TRANSMITTER_SETTINGS)
    int8_t  FirstStartComplete; // First start complete flag
    int8_t  SoundFlag;          // Sound enable flag
    int8_t  ModelNum;           // Used model

    // Данные для нормализации:
    int16_t ADmid[MAX_ADC]; // Центральное занчение
    int16_t ADmin[MAX_ADC]; // 
    int16_t ADmax[MAX_ADC]; // 
    int16_t Kmin[MAX_ADC];  // Коэффициент нормальзации при отклонении стика в меньшую сторону.
    int16_t Kmax[MAX_ADC];  // Коэффициент нормальзации при отклонении стика в большую сторону.
};

// Структуры с настройками передатчика - находятся в EEPROM
extern TRANSMITTER_SETTINGS Settings;

// Структуры с настройками передатчика - находятся в EEPROM
extern TRANSMITTER_SETTINGS EEPROM_SETTINGS;

// *****************************************************************************
// ***   Структуры, описывающие модель   ***************************************
// *****************************************************************************

// *** Описывает управляющий канал *********************************************
// Все Rates - EPA val in 2% unit
struct CONTROLS_SETTINGS
{
    int8_t  reverse;            // Реверс
    uint8_t maxRates;           // Максимальное значение
    uint8_t minRates;           // Минимальное значение
    uint8_t maxDRates;          // Максимальное значение при двойных расходах
    uint8_t minDRates;          // Минимальное значение при двойных расходах. Для газа T-Cut.
    int8_t  from;               // Для виртуальных каналов - из какого реального копировать
    int8_t  nodes[CURVE_NODES]; // Кривая
};

// *** Описывает управляющие каналы и микшер ***********************************
struct DEF_MODE
{
    char              name[MODE_NAME_LEN];                 // Название полётного режима (максимум 12 символов + нуль-терминатор)
    CONTROLS_SETTINGS Control[MAX_CONTROLS];               // Управляющие элементы
    int8_t            Chanels[MAX_CHANNELS][MAX_CONTROLS]; // Настройки микшера
    int8_t            trimmers[MAX_TRIMMERS];              // Значения триммеров
};

// *** Описывает модель ********************************************************
struct MODEL_SETTINGS
{
    int16_t    size;                 // Размер структуры. Должен быть sizeof(MODEL_SETTINGS)
    char       name[MODEL_NAME_LEN]; // Название модели (максимум 12 символов + нуль-терминатор)
    int8_t     type;                 // Тип модель: самолет, вертолет и т.д.
    Modulation modulation;           // Тип модуляции: PPM, IPPM и т.д.
    int16_t    timer;                // Начальное значение таймера
    int8_t     timer_mode;           // Режим счета таймера ВВЕРХ/ВНИЗ
    int8_t     num_ch;               // Количество каналов у модели
    DEF_MODE   Mode[MAX_MODES];      // Управляющие каналы и микшер
};

// Структура с данными текущей модели - находится в SRAM
extern MODEL_SETTINGS CurModel;

// Структуры с данными всех моделей - находятся в EEPROM
extern MODEL_SETTINGS EEPROM_MODEL[MAX_MODELS];

//
// Global variables
//

extern int16_t AD    [MAX_ADC];      // Входные данные с АЦП
extern int16_t output[MAX_CHANNELS]; // Выходные каналы после микшера

extern const int16_t OUTPUT_MIN;
extern const int16_t OUTPUT_MAX;
extern const int16_t OUTPUT_MID;


extern int8_t   FLY_MODE;      // Текущий полётный режим
extern uint8_t  trim_sound_en; // Признак необходимости озвучить нажатие устанавливается в прерывании, обрабатывается низшией задачей.
extern int8_t   Cut_enable;    // Если установлен - рисуется значек зачеркнутого двигателя и уровень газа устанавливается в minDRates
extern uint32_t TickCount;

#endif // __Global_Variables_h__