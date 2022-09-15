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
* 变量范围:


* 返回值说明:float
* 功能介绍:  
无料门限值获取

*
* 关键备注：
* - 
MIN_THRESHOLD略高于静态噪声能量，
判断无料流时需要持续低于该能量值才会更改料流状态
  
* 未来提高:



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
* 变量范围:


* 返回值说明:float
* 功能介绍:  
有料门限值获取函数

*
* 关键备注：
* - 
MIN_THRESHOLD略高于静态噪声能量，
判断无料流时需要持续低于该能量值才会更改料流状态
  
* 未来提高:



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
* 变量范围:


* 返回值说明:
返回延时的周期个数。
* 功能介绍:   
延时周期个数获取函数
多普勒开关的延时是通过给定的采样周期来确定的，
最小计算单位就是一个采样的周期，125ms。
*
* 关键备注：
* - 

  
* 未来提高:



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
* 变量范围:


* 返回值说明:
SystemStateMachineDef ：系统状态机，在此的功能是判断是否加载料流信息。
* 功能介绍:   
多普勒传感器信号能量分析函数，
根据计算好的能量差，与标准（或许是采集到的模拟电压）比较，决定是否提交料流状态加载。
*
* 关键备注：
* - 


* 未来提高:



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
    //有料状态下，判断料流是否停止
    if( G_SystemStateMachine == FLOW_STATE_LOAD)
    {
      /* Judge the sensor state */
      if(SensorEnergyValue > SensorGetFlowThreshold(5,ADC2_Value[1]))
      {
        //大于无料判定阈值时，累加有料标志，清除无料标志
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
        //异常情况默认为有料处理
        FLOW_STATE_HANDLE();
      }
    }
    //Switch the material flow state
    //只有达到料流确定周期时，才会复位累加标志变量，并更改料流状态。
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
* 变量范围:
bool8 Status：要执行的继电器动作状态，注意这种状态在手动模式下是需要根据
变量G_RelayStatus[0]来确定的。

* 返回值说明: 
* 功能介绍:   
继电器吸合函数，
通过本函数处理手动和自动模式下的继电器动作。
*
* 关键备注：
* - 

  
* 未来提高:



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