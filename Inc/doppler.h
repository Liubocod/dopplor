#ifndef __DOPPLER_H
#define __DOPPLER_H
#include "stm32l4xx_hal.h"
//#include "semphr.h"
#define JUDGMENT_LENGTH                 10
typedef enum
{
  NON,
  CONTINOUS_HIGH,
  CONTINOUS_LOW,
  HIGH,
  LOW,
}SignalDef;

extern void DopplerPinDown(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
extern void DopplerPinUp(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
extern void DopplerEnergyFlashing(uint8_t level);
extern void SensorDataHandle();

#endif