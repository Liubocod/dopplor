#ifndef LCD_H
#define LCD_H
#include "stm32l4xx_hal.h"
#define	LCDBUFFERSIZE						30
#define LCD_ADDRESS						0x8E    /* LCD Address  */
#define	LCDBUFFERMAXLENGH					40
typedef	union
{
  uint8_t	LCDBufferC[LCDBUFFERMAXLENGH];
  uint16_t      LCDBufferH[LCDBUFFERMAXLENGH/2];
  uint32_t      LCDBufferW[LCDBUFFERMAXLENGH/4];
  float         LCDBufferF[LCDBUFFERMAXLENGH/4];
}LcdDataBufferTypeDef;
extern LcdDataBufferTypeDef    G_LcdDataBuffer;
#endif