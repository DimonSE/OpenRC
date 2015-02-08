/*******************************************************************************
*  Global_Variables.cpp
*
*
*******************************************************************************/

#include <avr/io.h>
#include <avr/eeprom.h>

#include "def.h"
#include "Global_Variables.h"

TRANSMITTER_SETTINGS Settings;
MODEL_SETTINGS       CurModel;

TRANSMITTER_SETTINGS EEMEM EEPROM_SETTINGS;
MODEL_SETTINGS       EEMEM EEPROM_MODEL[MAX_MODELS];


// Входные данные с АЦП
int16_t AD[MAX_ADC];
// Выходные каналы после микшера
int16_t output[MAX_CHANNELS];

const int16_t OUTPUT_MIN = TimerClockPerUSec(TimerPPM_min);
const int16_t OUTPUT_MAX = TimerClockPerUSec(TimerPPM_max);
const int16_t OUTPUT_MID = TimerClockPerUSec(TimerPPM_mid);

// Текущий полётный режим
int8_t FLY_MODE;
// Признак необходимости озвучить нажатие. Устанавливается в прерывании,
// обрабатывается низшией задачей. РУДИМЕНТ.
uint8_t trim_sound_en;
// Если установлен - рисуется значек зачеркнутого двигателя и уровень газа
// устанавливается в minDRates
int8_t Cut_enable;

uint32_t TickCount = 0;