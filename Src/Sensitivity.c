#include "sensitivity.h"
#include "main.h"
#include "adc.h"
#include "doppler.h"
#include "cmsis_os.h"

#define DELAY_INTERVAL                  585
#define DELAY_BISIC_INTERVAL            4
#define MIN_THRESHOLD                   609
#define THRESHOLD_INTERVAL              40.95
#define NO_FLOW_STATE_HANDLE()          {NoMaterialFlowFlag++;\
                                                    MaterialFlowFlag = 0;}
#define FLOW_STATE_HANDLE()             {MaterialFlowFlag++;\
                                                    NoMaterialFlowFlag = 0;}
SystemStateMachineDef G_SystemStateMachine = NO_FLOW_STATE_LOAD;
/*
float SensorGetNoFlowThreshold()
* ������Χ:


* ����ֵ˵��:float
* ���ܽ���:  
��������ֵ��ȡ

*
* �ؼ���ע��
* - 
MIN_THRESHOLD�Ը��ھ�̬����������
�ж�������ʱ��Ҫ�������ڸ�����ֵ�Ż��������״̬
  
* δ�����:



*/ 
float SensorGetNoFlowThreshold(uint8_t Heoi,uint16_t StvyADC_Value)
{
  float HEOI = 0;
  if(Heoi < 5)
  {
    Heoi = 5;
  }
  if(StvyADC_Value < MIN_THRESHOLD)
  {
    HEOI = MIN_THRESHOLD + THRESHOLD_INTERVAL*Heoi;
  }
  else if((StvyADC_Value + THRESHOLD_INTERVAL*Heoi) >= 4095)
  {
    HEOI = 4095;
  }
  else
  {
    HEOI = StvyADC_Value + THRESHOLD_INTERVAL*Heoi;
  }
  return HEOI;
}
/*
float SensorGetFlowThreshold()
* ������Χ:


* ����ֵ˵��:float
* ���ܽ���:  
��������ֵ��ȡ����

*
* �ؼ���ע��
* - 
MIN_THRESHOLD�Ը��ھ�̬����������
�ж�������ʱ��Ҫ�������ڸ�����ֵ�Ż��������״̬
  
* δ�����:



*/ 

float SensorGetFlowThreshold(uint8_t Heoi,uint16_t StvyADC_Value)
{
  float HEOI = 0;
  if(Heoi < 5)
  {
    Heoi = 5;
  }
  if(StvyADC_Value < MIN_THRESHOLD)
  {
    HEOI = MIN_THRESHOLD - THRESHOLD_INTERVAL*Heoi;
  }
  else if(StvyADC_Value - THRESHOLD_INTERVAL*Heoi >= MIN_THRESHOLD)
  {
    HEOI = StvyADC_Value - THRESHOLD_INTERVAL*Heoi;
  }
  else
  {
    HEOI = MIN_THRESHOLD - THRESHOLD_INTERVAL*Heoi;
  }
  
  return HEOI;
}
/*
uint32_t SensorSignalGetDelayPeriod()
* ������Χ:


* ����ֵ˵��:
������ʱ�����ڸ�����
* ���ܽ���:   
��ʱ���ڸ�����ȡ����
�����տ��ص���ʱ��ͨ�������Ĳ���������ȷ���ģ�
��С���㵥λ����һ�����������ڣ�125ms��
*
* �ؼ���ע��
* - 

  
* δ�����:



*/ 
uint16_t SensorSignalGetDelayPeriod(uint16_t DelayADC_Value)
{
  uint16_t SignalLoadPeriod = 32;
  
  if(DelayADC_Value < DELAY_INTERVAL)
  {
    SignalLoadPeriod = DELAY_BISIC_INTERVAL*64;
  }
  else if((DelayADC_Value >= DELAY_INTERVAL)&&(DelayADC_Value < DELAY_INTERVAL*2))
  {
    SignalLoadPeriod = DELAY_BISIC_INTERVAL*32;
  }
  else if((DelayADC_Value >= DELAY_INTERVAL*2)&&(DelayADC_Value < DELAY_INTERVAL*3))
  {
    SignalLoadPeriod = DELAY_BISIC_INTERVAL*16;
  }
  else if((DelayADC_Value >= DELAY_INTERVAL*3)&&(DelayADC_Value < DELAY_INTERVAL*4))
  {
    SignalLoadPeriod = DELAY_BISIC_INTERVAL*8;
  }
  else if((DelayADC_Value >= DELAY_INTERVAL*4)&&(DelayADC_Value < DELAY_INTERVAL*5))
  {
    SignalLoadPeriod = DELAY_BISIC_INTERVAL*4;
  }
  else if((DelayADC_Value >= DELAY_INTERVAL*5)&&(DelayADC_Value < DELAY_INTERVAL*6))
  {
    SignalLoadPeriod = DELAY_BISIC_INTERVAL*2;
  }
  else if((DelayADC_Value >= DELAY_INTERVAL*6)&&(DelayADC_Value < DELAY_INTERVAL*7+1))
  {
    SignalLoadPeriod = DELAY_BISIC_INTERVAL;
  }
  return SignalLoadPeriod;
}
/*
SystemStateMachineDef SensorSignalEnergyAnalysis(float SensorEnergyValue)
* ������Χ:


* ����ֵ˵��:
SystemStateMachineDef ��ϵͳ״̬�����ڴ˵Ĺ������ж��Ƿ����������Ϣ��
* ���ܽ���:   
�����մ������ź���������������
���ݼ���õ���������׼�������ǲɼ�����ģ���ѹ���Ƚϣ������Ƿ��ύ����״̬���ء�
*
* �ؼ���ע��
* - 


* δ�����:



*/ 
SystemStateMachineDef SensorSignalEnergyAnalysis(float SensorEnergyValue)
{
  static uint32_t MaterialFlowFlag = 0;
  static uint32_t NoMaterialFlowFlag = 0;
  uint32_t ManualFlowPeriod = 32;
  uint32_t ManualNoFlowPeriod = 32;
  SystemStateMachineDef StateResult = G_SystemStateMachine;
  

  /* Automatic mode .*/
  ManualFlowPeriod = SensorSignalGetDelayPeriod(ADC2_Value[0]);
  ManualNoFlowPeriod = ManualFlowPeriod;
  
  
  if(G_SystemStateMachine != SENSOR_ABNORMAL)
  {
    /* Illegal times masking */
    if(MaterialFlowFlag > ManualFlowPeriod)
    {
      MaterialFlowFlag = ManualFlowPeriod-1;
      NoMaterialFlowFlag = 0;
    }
    else if(NoMaterialFlowFlag > ManualNoFlowPeriod)
    {
      NoMaterialFlowFlag = ManualNoFlowPeriod-1;
      MaterialFlowFlag = 0;
    }
    //����״̬�£��ж������Ƿ�ֹͣ
    if( G_SystemStateMachine == FLOW_STATE_LOAD)
    {
      /* Judge the sensor state */
      if(SensorEnergyValue > SensorGetFlowThreshold(5,ADC2_Value[1]))
      {
        //���������ж���ֵʱ���ۼ����ϱ�־��������ϱ�־
        FLOW_STATE_HANDLE();
      }
      else
      {
        NO_FLOW_STATE_HANDLE();
      }
    }
    else if( G_SystemStateMachine == NO_FLOW_STATE_LOAD)
    {
      //Judge the sensor state
      if(SensorEnergyValue < SensorGetNoFlowThreshold(5,ADC2_Value[1]))
      {
        NO_FLOW_STATE_HANDLE();
      }
      else
      {
        //�쳣���Ĭ��Ϊ���ϴ���
        FLOW_STATE_HANDLE();
      }
    }
    //Switch the material flow state
    //ֻ�дﵽ����ȷ������ʱ���ŻḴλ�ۼӱ�־����������������״̬��
    if((MaterialFlowFlag != 0)&&(MaterialFlowFlag % ManualFlowPeriod == 0))
    {
      StateResult = FLOW_STATE_LOAD;
      MaterialFlowFlag = 0;
    }
    else if((NoMaterialFlowFlag != 0)&&(NoMaterialFlowFlag % ManualNoFlowPeriod == 0))
    {
      StateResult = NO_FLOW_STATE_LOAD;
      NoMaterialFlowFlag = 0;
    }
  }
  return StateResult;
}
/*
void StvtRelayOnControl()
* ������Χ:
bool8 Status��Ҫִ�еļ̵�������״̬��ע������״̬���ֶ�ģʽ������Ҫ����
����G_RelayStatus[0]��ȷ���ġ�

* ����ֵ˵��: 
* ���ܽ���:   
�̵������Ϻ�����
ͨ�������������ֶ����Զ�ģʽ�µļ̵���������
*
* �ؼ���ע��
* - 

  
* δ�����:



*/ 
void StvtRelayEnableControl(bool8 StatusTamp)
{
  static uint8_t Flag = 3;

  if(Flag != StatusTamp)
  {
    /* Switch relay action . */
    Flag = StatusTamp;
    if(StatusTamp == TRUE)
    {
        DopplerPinDown(RELAY_CP_GPIO_Port,RELAY_CP_Pin);
        DopplerPinUp(RELAY_CTRL_GPIO_Port,RELAY_CTRL_Pin);
        osDelay(5);
        DopplerPinUp(RELAY_LED_GPIO_Port,RELAY_LED_Pin);
        DopplerPinUp(RELAY_CP_GPIO_Port,RELAY_CP_Pin);
        osDelay(5);
        DopplerPinDown(RELAY_CP_GPIO_Port,RELAY_CP_Pin);
     
    }
    else if(StatusTamp == FALSE)
    {
        DopplerPinDown(RELAY_CP_GPIO_Port,RELAY_CP_Pin);
        DopplerPinDown(RELAY_CTRL_GPIO_Port,RELAY_CTRL_Pin);
        osDelay(5);
        DopplerPinDown(RELAY_LED_GPIO_Port,RELAY_LED_Pin);
        DopplerPinUp(RELAY_CP_GPIO_Port,RELAY_CP_Pin);
        osDelay(5);
        DopplerPinDown(RELAY_CP_GPIO_Port,RELAY_CP_Pin);
      
    }
  }
}
void OutputSignalHandle(SystemStateMachineDef SystemStateMachine)
{
  switch(SystemStateMachine)
  {
  case FLOW_STATE_LOAD:
    StvtRelayEnableControl(TRUE);
    break;
  case NO_FLOW_STATE_LOAD:
    StvtRelayEnableControl(FALSE);
    break;
  }
}