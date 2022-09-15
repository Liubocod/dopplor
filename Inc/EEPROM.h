#ifndef EEPROM_H
#define EEPROM_H
#include "stm32l4xx_hal.h"
#pragma pack(1)
typedef struct
{
  float DelayValue;
  uint8_t RelayDir;
  uint8_t Threshold;
  uint8_t NoFlowDelay;
  uint8_t SignalVoltage;
  uint8_t FlowDelay;
  uint8_t LcdManualOperationFlag;
  uint8_t EventTypeThreshold;
  uint8_t EventCodeThreshold;
  uint8_t EventLevelThreshold;
  uint8_t EventTypeCondition;
  uint8_t EventCodeCondition;
  uint8_t EventLevelCondition;
  uint8_t NumOfEvent;
  uint8_t Front;
  uint16_t SoftRebootCount;
  uint16_t HardRebootCount;
  uint16_t ParamDefaultFlag;
}ParamTableDef,*pParamTableDef;
#pragma pack()
void EEP_ParamInit();
#endif