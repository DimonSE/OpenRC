/*******************************************************************************
*  System.h
*
*******************************************************************************/

#ifndef __System_h__
#define __System_h__

uint16_t Get_EEPROM_CRC();

bool Is_EEPROM_CRC_Correct();
void Recalc_EEPROM_CRC();

bool TX_IsFirstStart();

void TX_Init();
void TX_Reset();
void TX_SaveSettings();

void MODEL_Init(int8_t ModelNum);
void MODEL_Save_Trimmers();
void MODEL_Save(int8_t To);
void MODEL_Copy(int8_t From, int8_t To);
void MODEL_Reset(int8_t ModelNum);

void MODE_Copy(int8_t From, int8_t To);

void Calibration();

#endif // __System_h__