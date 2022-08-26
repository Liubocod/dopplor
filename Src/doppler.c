#include "doppler.h"
#include "main.h"
#include "cmsis_os.h"
typedef struct Node
{
  uint32_t value;
  struct Node* next;
  struct Node* previor;
}NodeDef,*pNodeDef;

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
void DopplerEnergyFlashing(uint8_t level)
{
  static uint16_t FlashingInterval = 0;
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
//升序插入
void InsertSort(pNodeDef HeadNode,bool8 IsAscendingSort)
{
  pNodeDef pNode_1 = NULL;
  pNodeDef pNode_2 = NULL;
  pNodeDef pNode_3 = NULL;
  pNodeDef pNode_4 = NULL;
  
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
}
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
}
uint32_t bufferTest[20] = {0};
//传感器采集数据分析处理
void SensorDataHandle()
{  
  pNodeDef pNode_1 = NULL;

  int32_t i = 0;
  pNodeDef FrontMin = pvPortMalloc(sizeof(NodeDef));
  uint32_t HeapFreeSize = 0;
  
  FrontMin->next = NULL;
  FrontMin->previor = NULL;
 
  FillLinkList(FrontMin,SensorResultDMA);
  HeapFreeSize = xPortGetFreeHeapSize();
  //插入排序法
  InsertSort(FrontMin,TRUE);
  HeapFreeSize = xPortGetFreeHeapSize();
  //筛选
  Filtrate(FrontMin,SensorResultDMA,DMA_BUFF_SIZE,10,TRUE);
  HeapFreeSize = xPortGetFreeHeapSize();
  pNode_1 = FrontMin->next;
  while(pNode_1 != FrontMin)
  {
    bufferTest[i] = pNode_1->value;
    i++;
    pNode_1 = pNode_1->next;
  }
  //归还内存
  LinkListFree(FrontMin);
  
  HeapFreeSize = xPortGetFreeHeapSize();
  if(HeapFreeSize == 5096)
  {
    HeapFreeSize = xPortGetFreeHeapSize();
  }
}





