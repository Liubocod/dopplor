#ifndef SENSITIVITY_H
#define SENSITIVITY_H
#include "stm32l4xx_hal.h"
#include "main.h"
typedef enum 
{
  FLOW_STATE_LOAD,
  NO_FLOW_STATE_LOAD,
  SENSOR_ABNORMAL,
}SystemStateMachineDef;
extern SystemStateMachineDef G_SystemStateMachine;
extern void StvtRelayEnableControl(bool8 StatusTamp);
extern SystemStateMachineDef SensorSignalEnergyAnalysis(float SensorEnergyValue);
extern void OutputSignalHandle(SystemStateMachineDef SystemStateMachine);
#endif