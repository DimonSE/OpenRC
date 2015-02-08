/*******************************************************************************
*  Utils.h
*
*
*******************************************************************************/

#ifndef __Utils_h__
#define __Utils_h__

#define mul2(x) ((x) << 1)
#define mul4(x) ((x) << 2)
#define mul8(x) ((x) << 3)

#define div2(x) ((x) >> 1)
#define div4(x) ((x) >> 2)
#define div8(x) ((x) >> 3)

#define mod2(x) ((x) & 0b00000001)
#define mod4(x) ((x) & 0b00000011)
#define mod8(x) ((x) & 0b00000111)

#define RGBto565(r, g, b) ( (((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3) )

struct _io_reg
{
  uint8_t bit0:1;
  uint8_t bit1:1;
  uint8_t bit2:1;
  uint8_t bit3:1;
  uint8_t bit4:1;
  uint8_t bit5:1;
  uint8_t bit6:1;
  uint8_t bit7:1;
};

#define REGISTER_BIT(register, bit) ((volatile _io_reg*)&(register))->bit

#endif /* __Utils_h__ */