/*******************************************************************************
*  Math.h
*
*******************************************************************************/

#ifndef __Math_h__
#define __Math_h__

// Рассчет времени канальных импульсов для вывода (Микширование)
void math_CalcChannel(int8_t CH);

// Расширенный рассчет кривой
int16_t math_InterPolEx(int16_t Val, int16_t Max, int16_t K, int8_t* Nodes, int8_t NodesCount);

// Общий рассчет кривой
int16_t math_InterPol(int16_t Val, int16_t Max, int8_t* Nodes, int8_t NodesCount);

// Опрос устройств ввода и рассчет их значений 
void math_CalcControls();

#endif // __Math_h__