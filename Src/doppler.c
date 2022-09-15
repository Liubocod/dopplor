#include "doppler.h"
#include "main.h"
#include "cmsis_os.h"
typedef struct Node
{
  uint32_t value;
  struct Node* next;
  struct Node* previor;
}NodeDef,*pNodeDef;

extern osMessageQId myQueue01Handle;
/*
void DopplerPinDown(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,)
* 变量范围:


* 返回值说明:void 
* 功能介绍:   
  拉低所选管脚电平
*
* 关键备注：
* - 

  
* 未来提高:



*/ 
void DopplerPinDown(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_RESET);
}
/*
void DopplerPinUp(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,)
* 变量范围:


* 返回值说明:void 
* 功能介绍:   
  拉高所选管脚电平
*
* 关键备注：
* - 

  
* 未来提高:



*/ 
void DopplerPinUp(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_SET);
}

//多普勒能量灯闪烁控制函数
void DopplerEnergyFlashing(uint32_t level)
{
  static uint32_t FlashingInterval = 0;
  static uint8_t FlashingFlag = 0;
  if(FlashingInterval >= level)
  {
    FlashingInterval = 0;
    if(FlashingFlag == 0)
    {
      DopplerPinUp(SIGNAL_LED_GPIO_Port,SIGNAL_LED_Pin);
      FlashingFlag = 1;
    }
    else
    {
      DopplerPinDown(SIGNAL_LED_GPIO_Port,SIGNAL_LED_Pin);
      FlashingFlag = 0;
    }
  }
  FlashingInterval++;
}
//添加最大值和最小值链表，将ADC采集数据的前十个建立链表
void FillLinkList(pNodeDef HeadNode,uint32_t* buffer)
{
  pNodeDef pNode_1 = pvPortMalloc(sizeof(NodeDef));
  pNodeDef pNode_2 = NULL;
  uint32_t i = 0;
  
  
  pNode_1->value = buffer[0];
  pNode_1->next = HeadNode;
  pNode_1->previor = HeadNode;
  
  HeadNode->next = pNode_1;
  HeadNode->previor = pNode_1;
  pNode_2 = pNode_1;
  //第一步，从ADC采集数组中选十个数，建立最小值链表
  for(i = 0;i < 9; i++)
  {
    pNode_1 = pvPortMalloc(sizeof(NodeDef));    
    if(pNode_1 != NULL)
    {
      pNode_1->value = buffer[i+1];
      pNode_1->next = HeadNode->next;
      pNode_1->previor = HeadNode;
      HeadNode->next = pNode_1;
      pNode_2->previor = pNode_1;
      pNode_2 = pNode_1;
    }
  }
}
//插入法和冒泡法将前十个数据进行排序
void InsertSort(pNodeDef HeadNode,bool8 IsAscendingSort)
{
  pNodeDef pNode_1 = NULL;
  pNodeDef pNode_2 = NULL;
  pNodeDef pNode_3 = NULL;
  pNodeDef pNode_4 = NULL;
  uint8_t linklistlength = 0;
  uint8_t CompareCounter = 0;
  
  if(IsAscendingSort == TRUE)
  {
    pNode_1 = HeadNode->next;
    pNode_2 = pNode_1->next;
    
    while(pNode_2 != HeadNode)
    {
      if(pNode_1->value > pNode_2->value)
      {
        //队列头作为缓冲区
        HeadNode->value = pNode_2->value;
        //向前比较节点指针
        pNode_3 = pNode_1;
        pNode_4 = pNode_2;
        while(HeadNode->value < pNode_3->value)
        {
          //将较大值后移
          pNode_4->value = pNode_3->value;
          //向前比较节点指针前移
          pNode_3 = pNode_3->previor;
          pNode_4 = pNode_4->previor;
        }
        pNode_4->value = HeadNode->value;
      }
      pNode_1 = pNode_1->next;
      pNode_2 = pNode_2->next;
    }
  }
  else if(IsAscendingSort == FALSE)
  {
    //bubbling sort
    for(linklistlength = 0;linklistlength < 9;linklistlength++)
    {
      pNode_1 = HeadNode->next;
      pNode_2 = pNode_1->next;
      for(CompareCounter = 0;CompareCounter < 9 - linklistlength;CompareCounter++)
      {
        if(pNode_1->value < pNode_2->value)
        {
          HeadNode->value = pNode_2->value;
          pNode_2->value = pNode_1->value;
          pNode_1->value = HeadNode->value;
        }
        pNode_1 = pNode_1->next;
        pNode_2 = pNode_2->next;
      }
    }
    
  }
}
//free掉用于链表申请的内存
void LinkListFree(pNodeDef HeadNode)
{
  pNodeDef pNode_1 = NULL;
  pNodeDef pNode_2 = NULL;
  
  pNode_1 = HeadNode->next;
  pNode_2 = pNode_1->next;
  while(pNode_1 != HeadNode)
  {
    HeadNode->next = pNode_2;
    pNode_2->previor = HeadNode;
    vPortFree(pNode_1);
    pNode_1 = pNode_2;
    pNode_2 = pNode_2->next;
  }
  vPortFree(HeadNode);
}
//筛选出ADC采集周期所有数据中的最大值或最小值
void Filtrate(pNodeDef HeadNode,uint32_t *Buffer,
              uint32_t FiltrateLength,uint32_t StartIndex,bool8 IsMinimun)
{
  pNodeDef pNodeData = NULL;
  pNodeDef pNode_1 = NULL;
  pNodeDef pNode_2 = NULL;
  pNodeDef pNode_3 = NULL;
  pNodeDef pNode_4 = NULL;
  bool8 IsInsert = FALSE;
  uint32_t i = 0;
  if(IsMinimun == TRUE)
  {
    for(i = StartIndex;i < FiltrateLength; i++)
    {
      pNodeData = pvPortMalloc(sizeof(NodeDef));
      pNodeData->value = Buffer[i];
      pNode_1 = HeadNode;
      pNode_2 = pNode_1->next;
      IsInsert = FALSE;
      while(pNode_2 != HeadNode)
      {
        if(pNodeData->value < pNode_2->value)
        {
          pNode_1->next = pNodeData;
          pNodeData->previor = pNode_1;
          pNodeData->next = pNode_2;
          pNode_2->previor = pNodeData;
          pNode_3 = HeadNode->previor;
          pNode_4 = pNode_3->previor;
          pNode_4->next = HeadNode;
          HeadNode->previor = pNode_4;
          //数据插入链表，将末尾表项free掉
          vPortFree(pNode_3);
          IsInsert = TRUE;
          break;
        }
        else
        {
          pNode_1 = pNode_1->next;
          pNode_2 = pNode_2->next;
        }
      }
      //没有插入链表，将节点free掉
      if(IsInsert == FALSE)
      {
        vPortFree(pNodeData);
      }
    }
  }
  else  if(IsMinimun == FALSE)
  {
    for(i = StartIndex;i < FiltrateLength; i++)
    {
      pNodeData = pvPortMalloc(sizeof(NodeDef));
      pNodeData->value = Buffer[i];
      pNode_1 = HeadNode;
      pNode_2 = pNode_1->next;
      IsInsert = FALSE;
      while(pNode_2 != HeadNode)
      {
        if(pNodeData->value > pNode_2->value)
        {
          pNode_1->next = pNodeData;
          pNodeData->previor = pNode_1;
          pNodeData->next = pNode_2;
          pNode_2->previor = pNodeData;
          pNode_3 = HeadNode->previor;
          pNode_4 = pNode_3->previor;
          pNode_4->next = HeadNode;
          HeadNode->previor = pNode_4;
          //数据插入链表，将末尾表项free掉
          vPortFree(pNode_3);
          IsInsert = TRUE;
          break;
        }
        else
        {
          pNode_1 = pNode_1->next;
          pNode_2 = pNode_2->next;
        }
      }
      //没有插入链表，将节点free掉
      if(IsInsert == FALSE)
      {
        vPortFree(pNodeData);
      }
    }
  }
}
//链表数据平均值计算
float LinkListAverageValueGet(pNodeDef HeadNode)
{
  pNodeDef pNode_1 = NULL;
  float average = 0,i = 0;
  
  pNode_1 = HeadNode->next;
  while(pNode_1 != HeadNode)
  {
    average += (float)pNode_1->value;
    i++;
    pNode_1 = pNode_1->next;
  }
  average /= i;
  return average;
}
//传感器采集数据分析处理
float SensorDataHandle()
{  
  float AverageMax = 0;
  float AverageMin = 0;
  float energy = 0;
  pNodeDef FrontMin = pvPortMalloc(sizeof(NodeDef));
  pNodeDef FrontMax = pvPortMalloc(sizeof(NodeDef));
  
  FrontMin->next = NULL;
  FrontMin->previor = NULL;
  
  FillLinkList(FrontMin,SensorResultDMA);
  FillLinkList(FrontMax,SensorResultDMA);
  //插入排序法
  InsertSort(FrontMin,TRUE);
  InsertSort(FrontMax,FALSE);
  //筛选
  Filtrate(FrontMin,SensorResultDMA,DMA_BUFF_SIZE,10,TRUE);
  Filtrate(FrontMax,SensorResultDMA,DMA_BUFF_SIZE,10,FALSE);
  //计算平均值
  AverageMax = LinkListAverageValueGet(FrontMax);
  AverageMin = LinkListAverageValueGet(FrontMin);
  energy = AverageMax - AverageMin;
  //归还内存
  LinkListFree(FrontMin);
  LinkListFree(FrontMax);
  return energy;
}
//LED闪烁频率获取函数
uint32_t LedFlashingFrequencyGet(uint32_t Energy)
{
  uint32_t value = 0;
  if(Energy < 100)
  {
    value = 300;
  }
  else if((Energy >= 100)&&(Energy < 500))
  {
    value = 240;
  }
  else if((Energy >= 1000)&&(Energy < 1500))
  {
    value = 200;
  }
  else if((Energy >= 1500)&&(Energy < 2000))
  {
    value = 160;
  }
  else if((Energy >= 2000)&&(Energy < 2500))
  {
    value = 120;
  }
  else if((Energy >= 2500)&&(Energy < 3000))
  {
    value = 100;
  }
  else if((Energy >= 3000)&&(Energy < 3500))
  {
    value = 80;
  }
  else if((Energy >= 3500)&&(Energy < 4000))
  {
    value = 60;
  }
  else
  {
    value = 40;
  }
  return value;
}



