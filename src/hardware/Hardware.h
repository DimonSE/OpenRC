/*******************************************************************************
*  Hardware.h
*
*******************************************************************************/

#ifndef __Hardware_h__
#define __Hardware_h__

#include "../def.h"
#include "../Modulation.h"

void hwr_InitPorts();
void hwr_InitTimers();
void hwr_InitUSART0(uint32_t BaudRate);
void hwr_InitADC();

//  АЦП преобразование
// Данная функция используется главным прерыванием. Поэтому использование её в других местах не желательно - есть возможноть порчи результата для вызвавшей
// функции. Главное прерывание опрашивает каналы АЦП из enum и складывает их в мессив AD[]. Значения брать оттуда.
uint16_t read_adc(uint8_t adc_input);

void hwr_SetModulation(const ModulationType modulation);

void hwr_SetTone(uint16_t Freq);

#endif // __Hardware_h__