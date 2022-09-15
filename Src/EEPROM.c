#include "EEPROM.h"
#include "i2c.h"
#include "cmsis_os.h"

#define INIT_FLAG       0xA5A5

static void EepromParamDefaultInit(void );

extern osSemaphoreId I2c2WaitHandle;
ParamTableDef EEP_ParamTable = {0};

void EEP_ParamInit()
{
  int32_t result = os_status_reserved;
  if(HAL_I2C_Mem_Read_DMA(&hi2c2, 0xA0, 0,
                          I2C_MEMADD_SIZE_16BIT, 
                          (uint8_t *)&EEP_ParamTable, 
                            sizeof(ParamTableDef)) != HAL_OK)
  {
    Error_Handler();
  }
  result = osSemaphoreWait(I2c2WaitHandle, osWaitForever);
  if(result == osOK)
  {
    if(EEP_ParamTable.ParamDefaultFlag != INIT_FLAG)
    {
      EepromParamDefaultInit();
      if(HAL_I2C_Mem_Write_DMA(&hi2c2, 0xA0, 0,
                           I2C_MEMADD_SIZE_16BIT, 
                              (uint8_t *)&EEP_ParamTable,
                                sizeof(ParamTableDef)) != HAL_OK)
      {
        Error_Handler();
      }
      result = osSemaphoreWait(I2c2WaitHandle, osWaitForever);
      if(result == osOK)
      {
        if(HAL_I2C_Mem_Read_DMA(&hi2c2, 0xA0, 0,
                                I2C_MEMADD_SIZE_16BIT, 
                                (uint8_t *)&EEP_ParamTable, 
                                  sizeof(ParamTableDef)) != HAL_OK)
        {
          Error_Handler();
        }
        if(EEP_ParamTable.ParamDefaultFlag != INIT_FLAG)
        {
          while(1);
        }
      }
    }
  }
}

/*
void EepromParamDefaultInit(void )
* ������Χ:

* ����ֵ˵��:void 
* ���ܽ���:   
EEPROM����Ĭ��ֵ�趨�����豸Ϊ�״��ϵ��EEPROM����ʹ�õ��������Ҫ��
ϵͳĬ�ϲ���д��EEPROM�н��б��棬��Ͷ��ϵͳ���㡣
*
* �ؼ���ע��
* - 

  
* δ�����:



*/ 

static void EepromParamDefaultInit(void )
{
  EEP_ParamTable.EventCodeCondition = 0;
  EEP_ParamTable.EventCodeThreshold = 0;
  EEP_ParamTable.EventLevelCondition = 0;
  EEP_ParamTable.EventLevelThreshold = 0;
  EEP_ParamTable.EventTypeCondition = 0;
  EEP_ParamTable.EventTypeThreshold = 0;
  EEP_ParamTable.FlowDelay = 1;
  EEP_ParamTable.Front = 0;
  EEP_ParamTable.HardRebootCount = 0;
  EEP_ParamTable.LcdManualOperationFlag = 0;
  EEP_ParamTable.NoFlowDelay = 2;
  EEP_ParamTable.NumOfEvent = 0;
  EEP_ParamTable.ParamDefaultFlag = INIT_FLAG;
  EEP_ParamTable.RelayDir = 1;
  EEP_ParamTable.SignalVoltage = 2;
  EEP_ParamTable.SoftRebootCount = 0;
  EEP_ParamTable.Threshold = 20;
  EEP_ParamTable.DelayValue = 0;
}