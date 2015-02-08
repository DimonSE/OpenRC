/*******************************************************************************
*  Crc.h
*
*******************************************************************************/

#ifndef __Crc_h__
#define __Crc_h__

uint16_t Crc16_eeprom(uint8_t* pcBlock, uint16_t len);

uint16_t Crc16_ram(uint8_t* pcBlock, uint16_t len);

#endif // __Crc_h__